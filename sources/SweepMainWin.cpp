/*
 * Copyright (c) 2014, Nick Potts
 * All rights reserved.
 *
 *  This Source Code Form is subject to the terms of the 
 *  Mozilla Public License, v. 2.0. If a copy of the MPL 
 *  was not distributed with this file, You can obtain 
 *  one at http://mozilla.org/MPL/2.0/.
*/

#include "SweepMainWin.h"


SweepMainWin::~SweepMainWin() { }
SweepMainWin::SweepMainWin(QMainWindow *parent) : QMainWindow(parent), data(NULL) {
  setupUi(this);

  //connect actions
  connect(act_open_csv, SIGNAL(triggered()), this, SLOT(openCSV()));
  connect(act_open_db, SIGNAL(triggered()), this, SLOT(openDB()));
  connect(act_saveCsv, SIGNAL(triggered()), this, SLOT(saveCSV()));
  connect(act_savePng, SIGNAL(triggered()), this, SLOT(saveImg()));
  connect(actionLog_Freq, SIGNAL(toggled(bool)), this, SLOT(logFreq(bool)));

  //disable all the SQL stuff until we actual open a database
  sqlfields << _dbv_0 << _dbv_1<< _dbv_2<< _dbv_3<< _dbv_4<< _dbv_5<< _dbv_6<< _dbv_7<< _dbv_8<< _dbv_9<< _dbv_10<< _dbv_11<< _dbv_12<< _dbv_13<< _dbv_14<< _dbv_15<< _dbv_16<< _dbv_17;
  sqlLabels << _db_0 << _db_1 << _db_2 << _db_3 << _db_4 << _db_5 << _db_6 << _db_7 << _db_8 << _db_9 << _db_10 << _db_11 << _db_12 << _db_13 << _db_14 << _db_15 << _db_16 << _db_17;
  muteSql();
  openedFile = new QLabel("No File Opened");
  statusbar->addPermanentWidget(openedFile);

  data = new QHoundData();
  plot->setpData(data);

  timeIndex->setMinimum(0);
  connect(timeIndex, SIGNAL(valueChanged(int)), plot, SLOT(loadSweep(int)));
}
void SweepMainWin::muteSql(bool mute) {
  //disable the SQL only fields
  foreach(QLineEdit *i, sqlfields) {
    if (mute)
      i->setText("");
    i->setDisabled(mute);
    i->setHidden(mute);
  }
  foreach(QLabel *i, sqlLabels) {
    i->setDisabled(mute);
    i->setHidden(mute); 
  }
  sqlbox->setDisabled(mute);
  sqlbox->setHidden(mute);
}
void SweepMainWin::updateMetadata() {
  //get max and mins setup
  timeIndex->setMaximum(data->getNumSweeps()-1);
  QDateTime begin = data->dateTimeFromIndex(0), end=data->dateTimeFromIndex(timeIndex->maximum());
  _min_time->setMinimumDateTime(begin);
  _min_time->setMaximumDateTime(end);
  _min_time->setDateTime(begin);
  _max_time->setMinimumDateTime(begin);
  _max_time->setMaximumDateTime(end);
  _max_time->setDateTime(end);
  
  //same with Frequency
  //get max and mins setup
  QwtInterval f = data->limits(FREQ);
  _min_freq->setValue(f.minValue());
  _max_freq->setValue(f.maxValue());

  qint64 dur = begin.msecsTo(end);
  _runtime->setText(QString::number(dur/1000.0));
  _samples->setText(QString::number(data->getSweep(0).size()));
  _sweeps->setText(QString::number(data->getNumSweeps()));

  //fetch metadata if it exists
  QStringList metadata = data->sqlMetadata();
  for(int i=0; i<metadata.size(); i++)
    sqlfields.at(i)->setText(metadata.at(i));
  sqlTables->clear();
  sqlTables->addItems(data->SQLTables());
  muteSql( ! ((metadata.size() == 17 && sqlTables->count() != 0)) ) ;

  timeIndex->setValue(0);
}
void SweepMainWin::openCSV(void) {
  /*Open a previously recorded CSV data file*/
  QString fname =  QFileDialog::getOpenFileName(this,"Open Signal Hound CSV Recording", QString(),"Spectral Signal Hound CSV Recordings (*.csv *.dat *.sshdata);;All Files (*)", 0, QFileDialog::ReadOnly);
  if (fname == "") return;
  if (!data->openCSV(fname)) {
    openedFile->setText(QString("Unable to open %1").arg(fname));
  }
  openedFile->setText(fname);
  updateMetadata();
}
void SweepMainWin::openDB(void) {

}
void SweepMainWin::saveCSV(void) {

}
void SweepMainWin::saveImg(void) {

}
void SweepMainWin::logFreq(bool doLog) {

}