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

 int main(int c, char **v) {
 	QApplication app(c, v);
 	SweepMainWin *sv = new SweepMainWin();
    sv->show();
    return app.exec();
 }