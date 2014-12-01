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

 int main(int c, char **v) {
 	std::cout << c << v[c-1] << std::endl;
 	QHoundData data;

 	/* CSV tests*/
 	bool ok = data.openCSV("test.csv");
 	std::cout << "openCSV: " << (ok ? "Ok": "Fail") << std::endl;
 	std::cout << "1417406248.762, 399999783.0 " << data.lvalue(1417406248.762, 399999783.0) << " s/b -83.5439" << std::endl; //top left point
 	std::cout << "1417406248.762, 406000216.0 " << data.lvalue(1417406248.762, 406000216.0) << " s/b -92.3722" << std::endl;//top right
 	std::cout << "1417406252.373, 399999783.0 " << data.lvalue(1417406252.373, 399999783.0) << " s/b -86.8954" << std::endl;//bottom left
 	std::cout << "1417406252.373, 406000216.0 " << data.lvalue(1417406252.373, 406000216.0) << " s/b -84.871" << std::endl;//bottom right
 	

 	//SQL tests
 	bool qok = data.openSQL("test.db");
 	qDebug() << "Opened SQL database: " << qok;
 	qDebug() << data.tables();
 	data.setTable("fast_20141130L205728");
 	std::cout << "1417406248.762, 399999783.0 " << data.lvalue(1417406248.762, 399999783.0) << " s/b -83.5439" << std::endl; //top left point
 	std::cout << "1417406248.762, 406000216.0 " << data.lvalue(1417406248.762, 406000216.0) << " s/b -92.3722" << std::endl;//top right
 	std::cout << "1417406252.373, 399999783.0 " << data.lvalue(1417406252.373, 399999783.0) << " s/b -86.8954" << std::endl;//bottom left
 	std::cout << "1417406252.373, 406000216.0 " << data.lvalue(1417406252.373, 406000216.0) << " s/b -84.871" << std::endl;//bottom right
 	
 }