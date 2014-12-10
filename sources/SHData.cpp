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

bool cmp_fseek_locs(const fseek_loc &a, const fseek_loc &b) { return a.first < b.first; };

QHoundData::~QHoundData() {
	csv->close();
	delete(csv);
}
QHoundData::QHoundData() {
	db = QSqlDatabase::addDatabase("QSQLITE", "SHData");
	locs.clear();
	freqs.clear();
	temps.clear();
	csv = new QFile();
	// setAttribute( QwtRasterData::WithoutGaps, true );
}
void QHoundData::setupMaxMin() {
	setInterval( Qt::XAxis, QwtInterval( locs.front().first, locs.back().first ) );
    setInterval( Qt::YAxis, QwtInterval( freqs.front(), freqs.back() ) );
    setInterval( Qt::ZAxis, QwtInterval( -150, 15 ) );
}
QStringList QHoundData::SQLTables(void) {
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
int QHoundData::closest(fseek_locs haystack, double d_needle) const {
	/**Seaches for needle in haystack.  Returns the index to the closest needle in haystack*/
	int index=0;
	fseek_loc needle = std::make_pair(d_needle, 0);
	std::pair<fseek_locs::iterator, fseek_locs::iterator> loc = std::equal_range(haystack.begin(), haystack.end(),  needle, cmp_fseek_locs);
	//first check if we are even in bounds again (first and second at end=over bounds, second at beginning = under bounds)
	if ( (loc.first == haystack.begin() && loc.second == haystack.begin()) | (loc.first == haystack.end() && loc.second == haystack.end()) ) return -1;

	// if both are pointing to the same value, we must be missing one.  Back up so we can compare values
	if (loc.first == loc.second) std::advance(loc.first, -1);

	if (loc.second == haystack.end()) //exact hit.
		index = std::distance(haystack.begin(), loc.first);
	else //value is somewhere between loc.first and second.  See which value is closer
		index = std::distance(haystack.begin(), (needle.first - (*loc.first).first > (*loc.second).first - needle.first) ? loc.second : loc.first);
	// qDebug() << "\tAsked for" << qSetRealNumberPrecision(15) << needle.first << "Lower Bound=" << qSetRealNumberPrecision(15) << (*loc.first).first << " Upper Bound=" << qSetRealNumberPrecision(15) << (*loc.second).first << " Index=" << index;
	return index;
}
int QHoundData::closest(vdouble haystack, double needle) const {
	/**Seaches for needle in haystack.  Returns the index to the closest needle in haystack*/
	int index = -1;
	//finds upper and lower bounds
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
		index = std::distance(haystack.begin(), (needle - *loc.first) > (*loc.second - needle) ? loc.second : loc.first);
	//qDebug() << "\tAsked for" << qSetRealNumberPrecision(15) << needle << "Lower Bound=" << qSetRealNumberPrecision(15) << *loc.first << " Upper Bound=" << qSetRealNumberPrecision(15) << *loc.second << " Index=" << index;
	return index;
}

QDateTime QHoundData::dateTimeFromIndex(unsigned int index) {
	if ( (locs.size() == 0) || (index >= locs.size())) return QDateTime();
	return QDateTime::fromMSecsSinceEpoch((qint64) (locs.at(index).first * 1000));
}
QString QHoundData::timestampFromIndex(unsigned int index) {
	if ( (locs.size() == 0) || (index >= locs.size())) return "";
	return dateTimeFromIndex(index).toString("yyyy-MM-dd HH:mm:ss.zzz");
}
QwtInterval QHoundData::limits(RangeType which) {
	QwtInterval rtn(-std::numeric_limits<double>::max(), -std::numeric_limits<double>::max());
	switch (which) {
		case TIME:
			rtn = QwtInterval(locs.front().first, locs.back().first); break;
		case FREQ:
			rtn = QwtInterval(freqs.front(), freqs.back()); break;
	}
	return rtn;
}
int QHoundData::getNumSweeps() {
	return locs.size();
}

bool QHoundData::openCSV(QString csvfilename) {
	/**Open the CSV file for random access.*/
	if (db.isOpen()) db.close();
	if (csv->isOpen()) csv->close();
	currentTable="";
	csv->setFileName(csvfilename);
	if (!csv->open(QIODevice::ReadOnly | QIODevice::Text)) return false;
	locs.clear(); locs.reserve(RESERVE_SIZE);
	temps.clear(); temps.reserve(RESERVE_SIZE);
	freqs.clear(); freqs.reserve(20000);
	QStringList headers = QString(csv->readLine()).split(",");
	for(int i=2; i<headers.size(); i++) //insert frequencies into list so we can pull them easily.
		freqs.push_back(headers[i].toDouble());
	single_sweep_length = freqs.size();
	while(!csv->atEnd()){ //populate locs with timestamps and freqs
		qint64 pos = csv->pos();
		QStringList line = QString(csv->readLine()).split(",");
		if(line.size() != single_sweep_length + 2) continue; //not enough or too much data
		temps.push_back(line.at(1).toDouble());
		locs.push_back(std::make_pair(QDateTime::fromString(line.at(0).left(23), "yyyy-MM-dd HH:mm:ss.zzz").toMSecsSinceEpoch() / 1000.0, pos));
	}
	setupMaxMin();
	return true;
}
bool QHoundData::openSQL(QString sqlfilename) {
	if (db.isOpen()) db.close();
	if (csv->isOpen()) csv->close();
	locs.clear(); locs.reserve(RESERVE_SIZE);
	temps.clear(); temps.reserve(RESERVE_SIZE);
	freqs.clear(); freqs.reserve(20000);
	db.setDatabaseName(sqlfilename);
	qDebug() << "QHoundData::openSQL> Oke";
 	return db.open();
}

bool QHoundData::setSQLTable(QString newTable) {
	QStringList sqlTables = SQLTables();
	if (! sqlTables.contains(newTable)) return false;
	currentTable = newTable;
	QSqlQuery query(db);
	//fetch headers
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

	if (! query.exec("SELECT count(rowid) FROM [" + newTable + "]")) {
		qDebug() << "Unable to query the number of rows:" << query.lastError();
		return false;
	}
	int num_rows = RESERVE_SIZE;
	while(query.next()) {
		num_rows = query.value(0).toInt();
	}

	//now query the rowids and timestamps
	if (! query.exec("SELECT timestamp, rowid, temperature FROM [" + newTable + "] WHERE header_row == 'false' ORDER BY rowid ASC")) {
		qDebug() << "Unable to pull CSV data:" << query.lastError();
		return false;
	}
	//clear vectors we are going to populate
	locs.clear(); locs.reserve(num_rows);
	temps.clear(); temps.reserve(num_rows);
	while(query.next()) {
		temps.push_back(query.value(2).toDouble());
		locs.push_back(std::make_pair(QDateTime::fromString(query.value(0).toString().left(23), "yyyy-MM-dd HH:mm:ss.zzz").toMSecsSinceEpoch() / 1000.0, query.value(1).toLongLong()));
	}
	setupMaxMin();
	return !locs.empty();
}
double QHoundData::value(double time, double freq ) const {
	int tindex = closest(locs, time);
	int findex = closest(freqs, freq);
	if ((tindex < 0) | (findex < 0))
		return -std::numeric_limits<double>::max();
	if (csv->isOpen()) {
		csv->seek(locs.at(tindex).second);
		QString d = csv->readLine();
		QStringList line = d.split(",");
		return line.at(findex + 2).toDouble();
	}
	if (db.isOpen()) {
		QSqlQuery query(db);
		if (query.exec("SELECT csv FROM [" + currentTable + "] WHERE rowid == '" + QString::number(locs.at(tindex).second)  + "'")) {
			query.next();
			QStringList c = query.value(0).toString().split(",");
			return c.at(findex).toDouble();
		} else {
			qDebug() << "Unable to pull DB data :" << query.lastError();
		}
	}
	//something bad happened.
	return -std::numeric_limits<double>::max();
}
fsweep QHoundData::getSweep(int index) {
	fsweep rtn(single_sweep_length); rtn.clear();
	if (csv->isOpen()) {
		csv->seek(locs.at(index).second);
		QStringList c = QString(csv->readLine()).split(",");
		for(int i=2; i<c.size(); i++) {
			rtn.push_back( QPointF(freqs.at(i-2),c.at(i).toDouble()) );
		}
	}
	if (db.isOpen()) {
		QSqlQuery query(db);
		if (query.exec("SELECT csv FROM [" + currentTable + "] WHERE rowid == '" + QString::number(locs.at(index).second)  + "'")) {
			//yank data
			query.next();
			QStringList c = query.value(0).toString().split(",");
			assert((unsigned int) c.size() == freqs.size());
			for(int i=0; i<c.size(); i++) {
				rtn.push_back( QPointF(freqs.at(i),c.at(i).toDouble()) );
			}
		} else {
			qDebug() << "Unable to pull sweep from database:" << query.lastError();
		}
	}
	return rtn;
}

QStringList QHoundData::sqlMetadata() {
	QStringList rtn;
	if (!db.isOpen()) return rtn;
	if (currentTable == "") return rtn;
	QSqlQuery query(db);
	if (query.exec("SELECT m_RBWSetpoint, m_VBWSetpoint, m_ZSMode, m_UseExtRef, m_PreampOn, m_stepFreq, m_stepAmpl, m_refLevel, m_refLevelOffset, m_refUnitsmV, m_decimation, m_sweepMode, m_sweepTime, m_FFTSize, m_channelBW, m_channelSpacing, m_serialNumber, m_HzPerPt from sweep_metadata WHERE data_table='" + currentTable + "'")) {
		query.next();
		for(int i=0; i<18; i++)
			rtn << query.value(i).toString();
	} else {
		qDebug() << "Unable to pull metadata :" << query.lastError();
	}
	return rtn;
}

// fsweep QHoundData::getSweep(int index) {
// 	fsweep rtn(single_sweep_length); rtn.clear();
// 	vdouble powers;
// 	for(int i=0; i<single_sweep_length; i++)
// 		rtn.append( QPointF(freqs.at(i), sweep_data[index*single_sweep_length + i]));
// 	return rtn;
// }
// fsweep QHoundData::getSweep(int index) {
// 	fsweep rtn(single_sweep_length); rtn.clear();
// 	vdouble powers;
// 	for(int i=0; i<single_sweep_length; i++)
// 		rtn.append( QPointF(freqs.at(i), sweep_data[index*single_sweep_length + i]));
// 	return rtn;
// }
// bool QHoundData::openCSV(QString csvfilename) {
// 	if (db.isOpen()) db.close();
// 	QFile csv(csvfilename);
// 	if (!csv.open(QIODevice::ReadOnly | QIODevice::Text))
// 		return false;
// 	sweep_data.clear();
// 	timestamps.clear();
// 	temperatures.clear();
// 	freqs.clear();
// 	QStringList headers = QString(csv.readLine()).split(",");
// 	for(int i=2; i<headers.size(); i++) //insert frequencies into list so we can pull them easily.
// 		freqs.push_back(headers[i].toDouble());
// 	single_sweep_length = freqs.size();

// 	while(!csv.atEnd()) {
// 		QStringList row = QString(csv.readLine()).split(",");
// 		if (row.size() != single_sweep_length + 2) continue; //extra parameters or missing.
// 		timestamps.push_back( QDateTime::fromString(row[0].left(23), "yyyy-MM-dd HH:mm:ss.zzz").toMSecsSinceEpoch() / 1000.0);
// 		temperatures.push_back(row[1].toDouble());
// 		for(int i=2;  i < row.size(); i++)
// 			sweep_data.push_back( row.at(i).toDouble() );
// 	}
// 	if (sweep_data.size() > 0 ) return true;
// 	setupMaxMin();
// 	return false;
// }
// bool QHoundData::openSQL(QString sqlfilename) {
// 	db.setDatabaseName(sqlfilename);
// 	if (db.isOpen()) //close
// 		db.close();
// 	return db.open();
// }
// bool QHoundData::setSQLTable(QString newTable) {
// 	QStringList sqlTables = SQLTables();
// 	if (! sqlTables.contains(newTable))
// 		return false;
// 	currentTable = newTable; //extract the data now
// 	//get table structure.
// 	QSqlQuery query(db);
// 	//first, fetch headers
// 	if (! query.exec("SELECT csv FROM [" + newTable + "] WHERE header_row == 'true'")) {
// 		qDebug() << "Unable to pull CSV headers:" << query.lastError();
// 		return false;
// 	}
// 	while(query.next()) {
// 		freqs.clear();
// 		foreach(QString i, query.value(0).toString().split(","))
// 			freqs.push_back(i.toDouble());
// 		single_sweep_length = freqs.size();
// 	}

// 	//now pull in the actual data
// 	if (! query.exec("SELECT timestamp, temperature, csv FROM [" + newTable + "] WHERE header_row == 'false' ORDER BY rowid ASC")) {
// 		qDebug() << "Unable to pull CSV data:" << query.lastError();
// 		return false;
// 	}
// 	//clear vectors we are going to populate
// 	timestamps.clear();
// 	temperatures.clear();
// 	sweep_data.clear();
// 	while(query.next()) {
// 		QStringList row = query.value(2).toString().split(",");
// 		if (row.size() != single_sweep_length) continue; //extra parameters or missing values.
// 		timestamps.push_back( QDateTime::fromString(query.value(0).toString().left(23), "yyyy-MM-dd HH:mm:ss.zzz").toMSecsSinceEpoch() / 1000.0);
// 		temperatures.push_back( query.value(1).toDouble());
// 		foreach(QString i, row)
// 			sweep_data.push_back(i.toDouble());
// 	}
// 	setupMaxMin();
// 	return !sweep_data.empty();
// }
// double QHoundData::value(double time, double freq ) const {
// 	/** Use time and freq to lookup the offsets from the large linear array stored in the stxxl vector*/
// 	int sweep_index, freq_index;

// 	//make sure we have populated the data arrays
// 	if ( (timestamps.size() == 0) | (sweep_data.size() == 0) | (freqs.size() == 0)) return -std::numeric_limits<double>::max();

// 	sweep_index = closest(timestamps, time);
// 	if (sweep_index == -1) return -std::numeric_limits<double>::max();

// 	freq_index = closest(freqs, freq);
// 	if (freq_index == -1) return -std::numeric_limits<double>::max();

// 	return sweep_data[sweep_index*single_sweep_length + freq_index];
// }