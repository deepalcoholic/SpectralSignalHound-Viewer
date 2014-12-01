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

bool mygreater (double i, double j) { return (i>j); }

QHoundData::QHoundData() {
	db = QSqlDatabase::addDatabase("QSQLITE", "SHData");
	//TODO MOVE THESE ELSEWHERE
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
		timestamps.push_back( QDateTime::fromString(row[0].left(23), "yyyy-MM-dd HH:mm:ss.zzz").toMSecsSinceEpoch() / 1000.0);
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
	return db.open();
}
bool QHoundData::setTable(QString newTable) {
	QStringList sqlTables = tables();
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
		timestamps.push_back( QDateTime::fromString(query.value(0).toString().left(23), "yyyy-MM-dd HH:mm:ss.zzz").toMSecsSinceEpoch() / 1000.0);
		temperatures.push_back( query.value(1).toDouble());
		foreach(QString i, row) {
			sweep_data.push_back(i.toDouble());
		}
	}
	return !sweep_data.empty();
}
QStringList QHoundData::tables(void) {
	QStringList rtn;
	if (!db.isOpen()) return rtn;
	QSqlQuery query(db);
	//get a list of tables
	query.exec("SELECT name FROM sqlite_master WHERE type='table' ORDER BY name");
	while (query.next())
		if (query.value(0).toString() != "sweep_metadata")
			rtn << query.value(0).toString();
	return rtn;
}
double QHoundData::value(double time, double freq ) const {
	return time+freq;
}
int QHoundData::closest(vdouble haystack, double needle) {
	/**Seaches for needle in haystack.  Returns the index to the closest needle in haystack*/
	//finds upper and lower bounds
	int index = -1;
	std::pair<vdouble::iterator, vdouble::iterator> loc = std::equal_range(haystack.begin(), haystack.end(), needle);
	//first check if we are even in bounds again (first and second at end=over bounds, second at beginning = under bounds)
	if ( (loc.first == haystack.begin() && loc.second == haystack.begin()) | (loc.first == haystack.end() && loc.second == haystack.end()) )
		return -1;
	// if both are pointing to the smae value, we must be missing one.  Back up so we can compare values
	if (loc.first == loc.second)
		std::advance(loc.first, -1);

	if (loc.second == haystack.end()) //exact hit.
		index = std::distance(haystack.begin(), loc.first);
	else //value is somewhere between loc.first and second.  See which value is closer
		index = std::distance(haystack.begin(), 
			(needle - *loc.first) > (*loc.second - needle) ? loc.second : loc.first);
	//qDebug() << "\tAsked for" << qSetRealNumberPrecision(15) << needle << "Lower Bound=" << qSetRealNumberPrecision(15) << *loc.first << " Upper Bound=" << qSetRealNumberPrecision(15) << *loc.second << " Index=" << index;
	return index;
}
double QHoundData::lvalue(double time, double freq ) {
	/** Use time and freq to lookup the offsets from the large linear array stored in the stxxl vector*/
	int sweep_index, freq_index;
	

	//make sure we have populated the data arrays
	if ( (timestamps.size() == 0) | (sweep_data.size() == 0) | (freqs.size() == 0)) {
		qDebug() << __FILE__ << ":" <<  __LINE__ << " Arrays are of invalid size";
		return -std::numeric_limits<double>::max();
	}

	sweep_index = closest(timestamps, time);
	if (sweep_index == -1)
		return -std::numeric_limits<double>::max();

	freq_index = closest(freqs, freq);
	if (freq_index == -1)
		return -std::numeric_limits<double>::max();

	return sweep_data[sweep_index*single_sweep_length + freq_index];
}