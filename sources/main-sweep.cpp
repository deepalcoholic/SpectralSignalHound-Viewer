/*
 * Copyright (c) 2014, Nick Potts
 * All rights reserved.
 *
 *  This Source Code Form is subject to the terms of the 
 *  Mozilla Public License, v. 2.0. If a copy of the MPL 
 *  was not distributed with this file, You can obtain 
 *  one at http://mozilla.org/MPL/2.0/.
*/

 #include "SHData.h"
 #include "SweepInspector.h"
 // #include "SpectralInspector.h"

 int main(int c, char **v) {
 	QHoundData data;
	qDebug() << "Opened SQL database: " << (data.openSQL("test.db") ? "Ok": "Fail");
 	QStringList tables = data.SQLTables();
 	qDebug() << "Available Tables" <<  tables;
 	//qDebug() << "Setting Table to fast_20141130L205728: " << (data.setSQLTable("fast_20141203L173406") ? "Ok": "Fail");
 	qDebug() << "Setting Table to FAST_20141204L112314: " << (data.setSQLTable(tables.last()) ? "Ok": "Fail");

 	//std::cout << "openCSV: " << (data.openCSV("test.csv") ? "Ok": "Fail") << std::endl;
 	QApplication *app = new QApplication(c, v);

    SweepInspector *si = new SweepInspector();
    // SpectralInspector *si = new SpectralInspector();
    si->setpData(&data);
    si->show();

    return app->exec();
 }