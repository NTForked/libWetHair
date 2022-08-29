//
// This file is part of the libWetHair open source project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2017 Yun (Raymond) Fei, Henrique Teles Maia, Christopher Batty,
// Changxi Zheng, and Eitan Grinspun
//

#ifndef LIBWETHAIR_CORE_DER_STRAND_PARAMETERS_H_
#define LIBWETHAIR_CORE_DER_STRAND_PARAMETERS_H_

#include <math.h>  // exp

#include "Definitions.h"
#include "Dependencies/BendingProducts.h"
#include "Dependencies/ElasticStrandUtils.h"

namespace libwethair {

struct StrandEquilibriumParameters {
  StrandEquilibriumParameters(const std::vector<Vec3>& vertices,
                              scalar curl_radius, scalar curl_density,
                              scalar dL, scalar root_length, scalar valid)
      : m_vertices(vertices),
        m_curl_radius(curl_radius),
        m_curl_density(curl_density),
        m_dL(dL),
        m_root_length(root_length),
        m_valid(valid),
        m_dirty(false) {}

  mutable std::vector<Vec3> m_vertices;
  mutable double m_curl_radius;
  mutable double m_curl_density;
  mutable double m_dL;
  mutable double m_root_length;
  mutable bool m_valid;
  mutable bool m_dirty;
};

// -0.021586124150231 gives exactly one for max, and ~0.02 for min, instead of
// [~0.04, ~1.02]
struct StrandParameters {
  StrandParameters(scalar radius, scalar YoungsModulus, scalar shearModulus,
                   scalar stretchingMultiplier, scalar density,
                   scalar viscosity, scalar baseRotation, scalar dt,
                   bool accumViscous = true, bool accumViscousBend = true,
                   bool variableRadiusHair = false, scalar straightHairs = 1.,
                   const Vec3& color = Vec3(0, 0, 0))
      : m_density(density),
        m_viscosity(viscosity),
        // m_rootRadiusMultiplier( 1. ),
        // m_tipRadiusMultiplier( 1. ),
        m_physicalRadius(radius),
        m_baseRotation(baseRotation),
        m_bendingMatrixBase(m_physicalRadius, m_baseRotation),
        m_youngsModulus(YoungsModulus),
        m_shearModulus(shearModulus),
        m_stretchingMultiplier(stretchingMultiplier),
        m_ks(m_physicalRadius, m_youngsModulus),
        m_kt(m_physicalRadius, m_shearModulus),
        m_accumulateWithViscous(accumViscous),
        m_accumulateViscousOnlyForBendingModes(accumViscousBend),
        m_variableRadiusHair(variableRadiusHair),
        m_straightHairs(straightHairs),
        m_color(color) {
    computeViscousForceCoefficients(dt);
  }

  scalar interpolatedRadiusMultiplier(int vtx, int numVertices) const {
    // return ( s * 0.5 + ( 1. - s ) * 1.0 ); // fixed constant linear
    // return ( s * m_tipRadiusMultiplier + ( 1. - s ) * m_rootRadiusMultiplier
    // ); // linear

    if (m_variableRadiusHair) {
      const scalar s = vtx / (numVertices - 1.);
      return (exp(-3.4612 * s)) * m_straightHairs +
             (1. - m_straightHairs);  // animal hair taper fit function
    }
    return 1.;  // constant
  }

  Vec3 getColor() const { return m_color; }

  scalar getKs(int vtx, int numVertices) const {
    const scalar interpol = interpolatedRadiusMultiplier(vtx, numVertices);
    return interpol * interpol * m_ks.get() * m_stretchingMultiplier;
  }

  scalar getKt(int vtx, int numVertices) const {
    const scalar interpol = interpolatedRadiusMultiplier(vtx, numVertices);
    const scalar interpol2 = interpol * interpol;

    return interpol2 * interpol2 * m_kt.get();
  }

  scalar getRadius(int vtx, int numVertices) const {
    return interpolatedRadiusMultiplier(vtx, numVertices) *
           m_physicalRadius.get();
  }

  scalar bendingCoefficient(int vtx, int numVertices) const {
    const scalar interpol = interpolatedRadiusMultiplier(vtx, numVertices);
    const scalar interpol2 = interpol * interpol;

    return interpol2 * interpol2 * m_youngsModulus.get();
  }

  BendingMatrixBase& getBendingMatrixBase() { return m_bendingMatrixBase; }

  Mat2 bendingMatrix(int vtx, int numVertices) const {
    return bendingCoefficient(vtx, numVertices) * m_bendingMatrixBase.get();
  }

  const Mat2& bendingMatrixBase() const { return m_bendingMatrixBase.get(); }

  void computeViscousForceCoefficients(scalar dt) {
    const scalar m_radius = m_physicalRadius.get();

    // Force coefficients are computed without the varying radius multiplier;
    // correct interpolation will be applied when they are accessed
    m_viscousKs = M_PI * m_radius * m_radius * 3 * m_viscosity / dt;
    m_viscousKt = M_PI_4 * m_radius * m_radius *
                  (m_radius * m_radius + m_radius * m_radius) * m_viscosity /
                  dt;
    m_viscousBendingCoefficientBase = 3 * m_viscosity / dt;
  }

  scalar viscousBendingCoefficient(int vtx, int numVertices) const {
    const scalar interpol = interpolatedRadiusMultiplier(vtx, numVertices);
    const scalar interpol2 = interpol * interpol;

    return interpol2 * interpol2 * m_viscousBendingCoefficientBase;
  }

  Mat2 viscousBendingMatrix(int vtx, int numVertices) const {
    return viscousBendingCoefficient(vtx, numVertices) *
           m_bendingMatrixBase.get();
  }

  scalar getViscousKs(int vtx, int numVertices) const {
    const scalar interpol = interpolatedRadiusMultiplier(vtx, numVertices);

    return interpol * interpol * m_viscousKs * m_stretchingMultiplier;
  }

  scalar getViscousKt(int vtx, int numVertices) const {
    const scalar interpol = interpolatedRadiusMultiplier(vtx, numVertices);
    const scalar interpol2 = interpol * interpol;

    return interpol2 * interpol2 * m_viscousKt;
  }

  void printParameters() {
    std::cout << "density: " << m_density << std::endl;
    std::cout << "viscosity: " << m_viscosity << std::endl;
    std::cout << "radius: " << getRadius(0, 10) << std::endl;
    std::cout << "baseRotation: " << m_baseRotation.get() << std::endl;
    std::cout << "YoungsModulus: " << m_youngsModulus.get() << std::endl;
    std::cout << "shearModulus: " << m_shearModulus.get() << std::endl;
    std::cout << "m_viscousBendingCoefficientBase: "
              << m_viscousBendingCoefficientBase << std::endl;
    std::cout << "m_viscousKt: " << m_viscousKt << std::endl;
    std::cout << "m_viscousKs: " << m_viscousKs << std::endl;
    std::cout << "m_ks: " << m_ks.get() << std::endl;
    std::cout << "m_kt: " << m_kt.get() << std::endl;
    std::cout << "accumViscousBend: " << m_accumulateWithViscous << std::endl;
    std::cout << "accumulateViscousOnlyForBendingModes: "
              << m_accumulateViscousOnlyForBendingModes << std::endl;
    std::cout << "bendingMatrixBase: " << bendingMatrixBase() << std::endl;
  }

  double m_density;
  double m_viscosity;

  // double m_rootRadiusMultiplier;
  // double m_tipRadiusMultiplier;

  // Computed viscous force coefficients. MUST be called at the beginning of
  // each time step if dt has changed.
  double m_viscousBendingCoefficientBase;
  scalar m_viscousKt;
  scalar m_viscousKs;

  // Dependencies
  mutable PhysicalRadius m_physicalRadius;
  mutable BaseRotation m_baseRotation;
  mutable BendingMatrixBase m_bendingMatrixBase;
  mutable YoungsModulus m_youngsModulus;
  mutable ShearModulus m_shearModulus;
  mutable double m_stretchingMultiplier;
  mutable ElasticKs m_ks;
  mutable ElasticKt m_kt;

  mutable Vec3 m_color;

  bool m_accumulateWithViscous;
  bool m_accumulateViscousOnlyForBendingModes;
  bool m_variableRadiusHair;
  scalar m_straightHairs;
};

}  // namespace libwethair

#endif  // LIBWETHAIR_CORE_DER_STRAND_PARAMETERS_H_
