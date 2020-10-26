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

#ifndef LIBWETHAIR_CORE_DER_FORCES_STRETCHING_FORCE_H_
#define LIBWETHAIR_CORE_DER_FORCES_STRETCHING_FORCE_H_

#include "../StrandForce.h"
#include "ViscousOrNotViscous.h"

template <typename ViscousT = NonViscous>
class StretchingForce {
 public:
  StretchingForce() {}

  virtual ~StretchingForce() {}

 public:
  static const IndexType s_first =
      0;  // The first index on which this force can apply
  static const IndexType s_last = 1;  // The last index (counting from the end)

  typedef Eigen::Matrix<scalar, 6, 1> LocalForceType;
  typedef Eigen::Matrix<scalar, 6, 6> LocalJacobianType;
  typedef VecX ForceVectorType;

  static std::string getName() { return ViscousT::getName() + "stretching"; }

  static scalar localEnergy(const StrandForce& strand, const IndexType vtx);

  static void computeLocal(LocalForceType& localF, const StrandForce& strand,
                           const IndexType vtx);

  static void computeLocal(LocalJacobianType& localJ, const StrandForce& strand,
                           const IndexType vtx);

  static void addInPosition(ForceVectorType& globalForce, const IndexType vtx,
                            const LocalForceType& localForce);

  static void accumulateCurrentE(scalar& energy, StrandForce& strand);
  static void accumulateCurrentF(VecX& force, StrandForce& strand);

  static void accumulateIntegrationVars(
      const unsigned& pos_start, const unsigned& j_start,
      const unsigned& tildek_start, const unsigned& global_start_dof,
      StrandForce& strand, VectorXs& lambda, TripletXs& J, TripletXs& tildeK,
      TripletXs& stiffness, VectorXs& Phi, const int& lambda_start);
};

#endif  // LIBWETHAIR_CORE_DER_FORCES_STRETCHING_FORCE_H_
