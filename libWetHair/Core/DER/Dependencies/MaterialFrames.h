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

#ifndef LIBWETHAIR_CORE_DER_DEPENDENCIES_MATERIAL_FRAMES_H_
#define LIBWETHAIR_CORE_DER_DEPENDENCIES_MATERIAL_FRAMES_H_

#include "ReferenceFrames.h"

/**
 * Unit: no dimension
 */
template <int FrameN>
class MaterialFrames : public DependencyNode<Vec3Array> {
 public:
  MaterialFrames(TrigThetas& trigThetas, ReferenceFrames1& referenceFrames1,
                 ReferenceFrames2& referenceFrames2)
      : DependencyNode<Vec3Array>(0, referenceFrames1.size()),
        m_trigThetas(trigThetas),
        m_referenceFrames1(referenceFrames1),
        m_referenceFrames2(referenceFrames2) {
    m_trigThetas.addDependent(this);
    m_referenceFrames1.addDependent(this);
    m_referenceFrames2.addDependent(this);
  }

  virtual const char* name() const;

 protected:
  virtual void compute();
  Vec3 linearMix(const Vec3& u, const Vec3& v, scalar s, scalar c);

  TrigThetas& m_trigThetas;
  ReferenceFrames1& m_referenceFrames1;
  ReferenceFrames2& m_referenceFrames2;
};

#endif  // LIBWETHAIR_CORE_DER_DEPENDENCIES_MATERIAL_FRAMES_H_
