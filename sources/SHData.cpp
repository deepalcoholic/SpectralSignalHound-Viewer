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
	qDebug() << "single_sweep_length = " << single_sweep_length;

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
		qDebug() << "single_sweep_length = " << single_sweep_length;
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
		qDebug() << query.value(0).toString().left(23);
		timestamps.push_back( QDateTime::fromString(query.value(0).toString().left(23), "yyyy-MM-dd HH:mm:ss.zzz").toMSecsSinceEpoch() / 1000.0);
		temperatures.push_back( query.value(1).toDouble());
		//qDebug() << "sweep_size" << sweep_data.size();
		foreach(QString i, row) {
			sweep_data.push_back(i.toDouble());
		}
		//qDebug() << "sweep_size now" << sweep_data.size();
	}
	qDebug() << sweep_data[0];
	return !sweep_data.empty();
}
QStringList QHoundData::tables(void) {
	QStringList rtn;
	if (!db.isOpen())
		return rtn;
	QSqlQuery query(db);
	//get a list of tables
	query.exec("SELECT name FROM sqlite_master WHERE type='table' ORDER BY name");
	while (query.next()) {
		if (query.value(0).toString() != "sweep_metadata")
			rtn << query.value(0).toString();
	}
	return rtn;
}
double QHoundData::value(double time, double freq ) const {
	return time+freq;
}
double QHoundData::lvalue(double time, double freq ) {
	/** Use time and freq to lookup the offsets from the large linear array stored in the stxxl vector*/
	unsigned int sweep_index, freq_index, upper_index, lower_index;
	std::pair<vdouble::iterator, vdouble::iterator> loc;

	//make sure we have populated the data arrays
	if ( (timestamps.size() == 0) | (sweep_data.size() == 0) | (freqs.size() == 0)) {
		qDebug() << __FILE__ << ":" <<  __LINE__ << " Arrays are of invalid size";
		return -std::numeric_limits<double>::max();
	}

	if ( (time < timestamps.front()) | (time > timestamps.back()) ) { //no data in the array
		qDebug() << __FILE__ << ":" <<  __LINE__ << " Requested time is out of specified data ranges";
		return -std::numeric_limits<double>::max();
	}

	if ((freq < freqs.front()) | (freq > freqs.back())) { //out of bounds errors
		qDebug() << __FILE__ << ":" <<  __LINE__ << " Requested frequency is outside of the sampled range";
		return -std::numeric_limits<double>::max();
	}

	//find the closest time
	loc = std::equal_range(timestamps.begin(), timestamps.end(), time); //finds upper and lower bounds
	lower_index = loc.first  - timestamps.begin();
	upper_index = loc.second - timestamps.begin();
	qDebug() << "Time Upper: " << upper_index << " Lower:" << lower_index;
	assert(upper_index - lower_index < 2);
	sweep_index = lower_index;// (upper_index + lower_index) >> 1;

	//find the closest freq
	loc = std::equal_range(freqs.begin(), freqs.end(), freq);
	lower_index = loc.first  - freqs.begin();
	upper_index = loc.second - freqs.begin();
	qDebug() << "Freq Upper: " << upper_index << " Lower:" << lower_index;
	assert(upper_index - lower_index < 2);
	freq_index =  lower_index; //(upper_index + lower_index) >> 1;

	return sweep_data[sweep_index*single_sweep_length + freq_index];
}