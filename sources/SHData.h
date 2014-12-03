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
#include <algorithm>
#include <stxxl/vector>
#include <stxxl/random>
#include <stxxl/sort>
#include <QDebug>
#include <QFile>
#include <QtSql>
#include <QDateTime>
#include <QVector>
#include <QPointF>
#include <qwt_raster_data.h> //QWT data type

typedef stxxl::VECTOR_GENERATOR<double>::result stxxl_vdouble;
typedef std::vector<double> vdouble;
typedef QVector<QPointF> fsweep;
typedef std::pair<double, double> range;
enum RangeType {TIME, FREQ};

class QHoundData: public QwtRasterData {
  public:
    explicit QHoundData();
    bool openCSV(QString);
    bool openSQL(QString);
    bool setSQLTable(QString);
    QStringList SQLTables();
    virtual double value(double, double) const;
    QString timestampFromIndex(int);
    range limits(RangeType); 
    fsweep getSweep(int);
    int getNumSweeps();

  private:
    int closest(vdouble, double) const;
    void setupMaxMin();
    QSqlDatabase db;
    QString currentTable;

    int single_sweep_length;
    stxxl_vdouble sweep_data;
    vdouble freqs;
    vdouble temperatures;
    vdouble timestamps;
};
