/*
 * Copyright (c) 2014, Nick Potts
 * All rights reserved.
 *
 *  This Source Code Form is subject to the terms of the 
 *  Mozilla Public License, v. 2.0. If a copy of the MPL 
 *  was not distributed with this file, You can obtain 
 *  one at http://mozilla.org/MPL/2.0/.
*/


#include <stxxl/vector>
#include <stxxl/random>
#include <stxxl/sort>
#include <vector>
#include <algorithm>
#include <QDateTime>
#include <QFile>
#include <QtSql>
#include <QDebug>
#include <qwt_raster_data.h>


typedef stxxl::VECTOR_GENERATOR<double>::result vdouble;

class QHoundData: public QwtRasterData {
  public:
    explicit QHoundData();
    bool openCSV(QString);
    bool openSQL(QString);
    bool setTable(QString);
    QStringList tables();
    virtual double value( double x, double y ) const;
  private:
    QSqlDatabase db;
    QStringList sqlTables;
    QString currentTable;

    int single_sweep_length;
    vdouble sweep_data;
    std::vector<double> freqs;
    std::vector<double> temperatures;
    std::vector<double> timestamps;
};
