/*
 * Copyright (c) 2014, Nick Potts
 * All rights reserved.
 *
 *  This Source Code Form is subject to the terms of the 
 *  Mozilla Public License, v. 2.0. If a copy of the MPL 
 *  was not distributed with this file, You can obtain 
 *  one at http://mozilla.org/MPL/2.0/.
*/

#include "SweepMainWin.h"


SweepMainWin::~SweepMainWin() { }
SweepMainWin::SweepMainWin(QMainWindow *parent) : QMainWindow(parent), data(NULL) {
  setupUi(this);
}