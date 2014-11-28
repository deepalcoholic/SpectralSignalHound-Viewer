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

QHoundData::QHoundData() {
	db = QSqlDatabase::addDatabase("QSQLITE", "SHData");
	setInterval( Qt::XAxis, QwtInterval( 0, 1 ) );
    setInterval( Qt::YAxis, QwtInterval( 1, 2 ) );
    setInterval( Qt::ZAxis, QwtInterval( -150, 10 ) );
}
bool QHoundData::openCSV(QString csvfilename) {
	db.close();
	QFile csv(csvfilename);
	if (!csv.open(QIODevice::ReadOnly | QIODevice::Text))
		return false;
	sweep_data.clear();
	timestamps.clear();
	temperatures.clear();
	freqs.clear();
	QStringList headers = QString(csv.readLine()).split(",");
	for(int i=2; i<headers.size(); i++) //insert frequencies into list so we can pull them easily.
		freqs.push_back(headers[i].toDouble());
	single_sweep_length = freqs.size();

	while(!csv.atEnd()) {
		QStringList row = QString(csv.readLine()).split(",");
		if (row.size() != single_sweep_length + 2) continue; //extra parameters or missing.
		timestamps.push_back( QDateTime::fromString(row[0], "YYYY-MM-DD HH:mm:ss.zzz").toMSecsSinceEpoch() / 1000.0);
		temperatures.push_back(row[1].toDouble());
		for(int i=2;  i < row.size(); i++)
			sweep_data.push_back( row.at(i).toDouble() );
	}
	if (sweep_data.size() > 0 ) return true;
	return false;
}
bool QHoundData::openSQL(QString sqlfilename) {
	db.setDatabaseName(sqlfilename);
	if (db.isOpen()) //close
		db.close();
	if (db.open())
		return false;
	QSqlQuery query(db);
	//get a list of tables
	sqlTables.clear();
	db.exec("SELECT name FROM sqlite_master WHERE type='table' ORDER BY name");
	while (query.next()) {
		if (query.value(0).toString() != "sweep_metadata")
			sqlTables << query.value(0).toString();
	}
	currentTable = "";
	return true;
}
bool QHoundData::setTable(QString newTable) {
	if (! sqlTables.contains(newTable))
		return false;
	currentTable = newTable; //extract the data now

	//get table structure.
	QSqlQuery query(db);
	//first, fetch headers
	if (! query.exec("SELECT csv FROM [" + newTable + "] WHERE header_row == 'true'")) {
		qDebug() << "Unable to pull CSV headers:" << query.lastError();
		return false;
	}
	while(query.next()) {
		freqs.clear();
		foreach(QString i, query.value(0).toString().split(","))
			freqs.push_back(i.toDouble());
		single_sweep_length = freqs.size();
	}

	//now pull in the actual data
	if (! query.exec("SELECT timestamp, temperature, csv FROM [" + newTable + "] WHERE header_row == 'false' ORDER BY rowid ASC")) {
		qDebug() << "Unable to pull CSV data:" << query.lastError();
		return false;
	}
	//clear vectors we are going to populate
	timestamps.clear();
	temperatures.clear();
	sweep_data.clear();
	while(query.next()) {
		QStringList row = query.value(2).toString().split(",");
		if (row.size() != single_sweep_length) continue; //extra parameters or missing values.
		timestamps.push_back( QDateTime::fromString(query.value(0).toString(), "YYYY-MM-DD HH:mm:ss.zzz").toMSecsSinceEpoch() / 1000.0 );
		temperatures.push_back( query.value(1).toDouble());
		foreach(QString i, row) {
			sweep_data.push_back(i.toDouble());
		}
	}
	return !sweep_data.empty();
}
QStringList QHoundData::tables(void) {
	if (db.isOpen())
		return sqlTables;
	return QStringList();
}
double QHoundData::value( double time, double freq ) const {
	//use time and freq to lookup the offsets from the large linear array stored in the stxxl vector

    /*const double c = 0.642;
    const double v1 = x * x + ( y - c ) * ( y + c );
    const double v2 = x * ( y + c ) + x * ( y + c );
    return 1.0 / ( v1 * v1 + v2 * v2 );*/
    return 0;
}
