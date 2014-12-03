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

 int main(int c, char **v) {
 	QHoundData data;

 	qDebug() << "Opened SQL database: " << (data.openSQL("test.db") ? "Ok": "Fail");
 	qDebug() << "Available Tables" << data.SQLTables();
 	qDebug() << "Setting Table to fast_20141130L205728: " << (data.setSQLTable("fast_20141130L205728") ? "Ok": "Fail");

 	//std::cout << "openCSV: " << (data.openCSV("test.csv") ? "Ok": "Fail") << std::endl;

 	QApplication *app = new QApplication( c, v);

    SweepInspector *si = new SweepInspector();
    si->setpData(&data);
    si->show();

    return app->exec();

    /*
 	// CSV tests
 	std::cout << "openCSV: " << (data.openCSV("test.csv") ? "Ok": "Fail") << std::endl;
 	std::cout << "1417406248.8, 399999783.0 " << data.value(1417406248.8, 399999783.0) << " s/b -83.5439" << std::endl; //top left point
 	std::cout << "1417406248.8, 406000216.0 " << data.value(1417406248.8, 406000216.0) << " s/b -92.3722" << std::endl;//top right
 	std::cout << "1417406252.3, 399999783.0 " << data.value(1417406252.3, 399999783.0) << " s/b -86.8954" << std::endl;//bottom left
 	std::cout << "1417406252.3, 406000216.0 " << data.value(1417406252.3, 406000216.0) << " s/b -84.871" << std::endl;//bottom right

 	//SQL tests
 	qDebug() << "Opened SQL database: " << (data.openSQL("test.db") ? "Ok": "Fail");
 	qDebug() << "Available Tables" << data.tables();
 	qDebug() << "Setting Table to fast_20141130L205728: " << (data.setSQLTable("fast_20141130L205728") ? "Ok": "Fail");
 	std::cout << "1417406248.8, 399999783.0 " << data.value(1417406248.8, 399999783.0) << " s/b -83.5439" << std::endl; //top left point
 	std::cout << "1417406248.8, 406000216.0 " << data.value(1417406248.8, 406000216.0) << " s/b -92.3722" << std::endl;//top right
 	std::cout << "1417406252.3, 399999783.0 " << data.value(1417406252.3, 399999783.0) << " s/b -86.8954" << std::endl;//bottom left
 	std::cout << "1417406252.3, 406000216.0 " << data.value(1417406252.3, 406000216.0) << " s/b -84.871" << std::endl;//bottom right
	*/
 }