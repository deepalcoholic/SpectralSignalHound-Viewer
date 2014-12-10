/*
 * Copyright (c) 2014, Nick Potts
 * All rights reserved.
 *
 *  This Source Code Form is subject to the terms of the 
 *  Mozilla Public License, v. 2.0. If a copy of the MPL 
 *  was not distributed with this file, You can obtain 
 *  one at http://mozilla.org/MPL/2.0/.
*/

#pragma once

#include <limits> //for error values
#include <iterator> //for std::distance
#include <vector>
#include <assert.h>
#include <algorithm>
// #include <stxxl/vector>
// #include <stxxl/random>
// #include <stxxl/sort>
#include <QDebug>
#include <QFile>
#include <QtSql>
#include <QDateTime>
#include <QVector>
#include <QPointF>
#include <qwt_interval.h>
#include <qwt_raster_data.h>

#define RESERVE_SIZE 350000

// typedef stxxl::VECTOR_GENERATOR<double>::result stxxl_vdouble;
typedef std::vector<double> vdouble;
typedef QVector<QPointF> fsweep;
typedef std::pair<double, double> range;
typedef std::pair<double, qint64> fseek_loc;
typedef std::vector<fseek_loc> fseek_locs;
enum RangeType {TIME, FREQ};

class QHoundData: public QwtRasterData {
  public:
    ~QHoundData();
    explicit QHoundData();
    bool openCSV(QString);
    bool openSQL(QString);
    bool setSQLTable(QString);
    QStringList SQLTables();
    virtual double value(double, double) const;
    QString timestampFromIndex(unsigned int);
    QwtInterval limits(RangeType); 
    fsweep getSweep(int);
    int getNumSweeps();

  private:
    int closest(fseek_locs, double) const;
    int closest(vdouble, double) const;
    void setupMaxMin();
    QSqlDatabase db;
    QFile *csv;
    QString currentTable;
    fseek_locs locs;
    vdouble freqs;
    vdouble temps;

    

    int single_sweep_length;


    // stxxl_vdouble sweep_data;
    // vdouble freqs;
    // vdouble temperatures;
    // vdouble timestamps;
};
