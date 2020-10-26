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

#ifndef LIBWETHAIR_CORE_BENDINGFORCE_H_
#define LIBWETHAIR_CORE_BENDINGFORCE_H_

#include "ViscousOrNotViscous.h"

class StrandForce;
struct StrandParameters;

template <typename ViscousT = NonViscous>
class BendingForce {
 public:
  BendingForce() {}

  virtual ~BendingForce() {}

 public:
  static const IndexType s_first =
      1;  // The first index on which this force can apply
  static const IndexType s_last = 1;  // The last index (counting from the end)

  typedef Eigen::Matrix<scalar, 11, 1> LocalForceType;  // Vec11
  typedef Eigen::Matrix<scalar, 11, 11> LocalJacobianType;

  static std::string getName() { return ViscousT::getName() + "bending"; }

  static scalar localEnergy(const StrandForce& strand, const IndexType vtx);

  static void computeLocal(LocalForceType& localF, const StrandForce& strand,
                           const IndexType vtx);

  static void computeLocal(LocalJacobianType& localJ, const StrandForce& strand,
                           const IndexType vtx);

  static void addInPosition(VecX& globalForce, const IndexType vtx,
                            const LocalForceType& localForce);

  static void accumulateCurrentE(scalar& energy, StrandForce& strand);
  static void accumulateCurrentF(VecX& force, StrandForce& strand);

  static void accumulateIntegrationVars(
      const unsigned& pos_start, const unsigned& j_start,
      const unsigned& tildek_start, const unsigned& global_start_dof,
      StrandForce& strand, VectorXs& lambda, TripletXs& J, TripletXs& tildeK,
      TripletXs& stiffness, VectorXs& Phi, const int& lambda_start);
};

#endif  // LIBWETHAIR_CORE_BENDINGFORCE_H_
