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


#include "sorter.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <tbb/tbb.h>

using namespace std;

Sorter::Sorter( int ni_, int nj_, int nk_ )
: ni(ni_), nj(nj_), nk(nk_)
{
  resize(ni, nj, nk);
}

Sorter::~Sorter() {
}

void Sorter::resize( int ni_, int nj_, int nk_ )
{
  array_sup.resize(ni_ * nj_ * nk_);
  ni = ni_; nj = nj_; nk = nk_;
}
