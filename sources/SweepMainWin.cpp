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


SweepMainWin::~SweepMainWin() {
	delete(browser);

}
SweepMainWin::SweepMainWin(QMainWindow *parent) : QMainWindow(parent), data(NULL) {
  setupUi(this);

  //connect actions
  connect(act_open_csv, SIGNAL(triggered()), this, SLOT(openCSV()));
  connect(act_open_db, SIGNAL(triggered()), this, SLOT(openDB()));
  connect(act_saveCsv, SIGNAL(triggered()), this, SLOT(saveCSV()));
  connect(act_savePng, SIGNAL(triggered()), this, SLOT(saveImg()));
  connect(act_manual,  SIGNAL(triggered()), this, SLOT(showManual()));
  connect(act_about,  SIGNAL(triggered()), this, SLOT(about()));
  connect(actionReset_Zoom, SIGNAL(triggered()), this, SLOT(resetZoom()));

  connect(_min_freq, SIGNAL(valueChanged(double)), this, SLOT(applyZoomWindow()));
  connect(_max_freq, SIGNAL(valueChanged(double)), this, SLOT(applyZoomWindow()));
  connect(_min_pwr, SIGNAL(valueChanged(double)), this, SLOT(applyZoomWindow()));
  connect(_max_pwr, SIGNAL(valueChanged(double)), this, SLOT(applyZoomWindow()));

  //disable all the SQL stuff until we actual open a database
  sqlfields << _dbv_0 << _dbv_1<< _dbv_2<< _dbv_3<< _dbv_4<< _dbv_5<< _dbv_6<< _dbv_7<< _dbv_8<< _dbv_9<< _dbv_10<< _dbv_11<< _dbv_12<< _dbv_13<< _dbv_14<< _dbv_15<< _dbv_16<< _dbv_17;
  sqlLabels << _db_0 << _db_1 << _db_2 << _db_3 << _db_4 << _db_5 << _db_6 << _db_7 << _db_8 << _db_9 << _db_10 << _db_11 << _db_12 << _db_13 << _db_14 << _db_15 << _db_16 << _db_17;
  muteSql();
  openedFile = new QLabel("No File Opened");
  statusbar->addPermanentWidget(openedFile);

  data = new QHoundData();
  plot->setpData(data);
  connect(plot, SIGNAL(zoominated(const QRectF &)), this, SLOT(zoominated(const QRectF &)));

  timeIndex->setMinimum(0);
  index->setMinimum(0);
  connect(timeIndex, SIGNAL(valueChanged(int)), plot, SLOT(loadSweep(int)));

  QFile html(":/manual.html");
  html.open(QIODevice::ReadOnly);
  QString d = html.readAll();
  browser = new QTextBrowser(0);
  browser->setVisible(false);
  browser->setHtml(d);
}
void SweepMainWin::zoominated(const QRectF &rect) {
  //zoom window changed, update freq selectors
  qreal fmin, fmax, pmin, pmax;
  rect.getCoords(&fmin, &pmin, &fmax, &pmax);
  _min_freq->setValue(fmin);
  _max_freq->setValue(fmax);
  _min_pwr->setValue(pmin);
  _max_pwr->setValue(pmax);
  applyZoomWindow();
}
void SweepMainWin::applyZoomWindow() {
  plot->setAxisScale(QwtPlot::xBottom, _min_freq->value(), _max_freq->value(), (_max_freq->value() - _min_freq->value())/ 5.0);
  plot->setAxisScale(QwtPlot::yLeft, _min_pwr->value(), _max_pwr->value(), (_max_pwr->value() - _min_pwr->value()) / 10.0);
}
void SweepMainWin::resetZoom() {
  //zoom all the way out
  _min_freq->setValue(_min_freq->minimum()); _max_freq->setValue(_max_freq->maximum()); 
  _min_pwr->setValue(_min_pwr->minimum()); _max_pwr->setValue(_max_pwr->maximum());
}
void SweepMainWin::muteSql(bool mute) {
  //disable the SQL only fields
  foreach(QLineEdit *i, sqlfields) {
    if (mute) i->setText("");
    i->setDisabled(mute);
    i->setHidden(mute);
  }
  foreach(QLabel *i, sqlLabels) {
    i->setDisabled(mute);
    i->setHidden(mute); 
  }

  sqlbox->setDisabled(sqlTables->count() == 0);
  sqlbox->setHidden(sqlTables->count() == 0);
}
void SweepMainWin::updateMetadata() {
  //get max and mins setup
  timeIndex->setMaximum(data->getNumSweeps());
  index->setMaximum(data->getNumSweeps());
  QDateTime begin = data->dateTimeFromIndex(0), end=data->dateTimeFromIndex(timeIndex->maximum()-1);
  _min_time->setDateTimeRange(begin, end);
  _max_time->setDateTimeRange(begin, end);
  _min_time->setDateTime(begin);
  _max_time->setDateTime(end);
  
  //same with Frequency
  //get max and mins setup
  QwtInterval f = data->limits(FREQ);
  _min_freq->setRange(f.minValue(), f.maxValue());
  _max_freq->setRange(f.minValue(), f.maxValue());

  _min_freq->setValue(f.minValue());
  _max_freq->setValue(f.maxValue());

  qint64 dur = begin.msecsTo(end);
  _runtime->setText(QString::number(dur/1000.0));
  _sweeps->setText(QString::number(data->getNumSweeps()));
  if (data->getNumSweeps() != 0)
    _samples->setText(QString::number(data->getSweep(0).size()));
  
  //fetch metadata if it exists
  QStringList metadata = data->sqlMetadata();
  for(int i=0; i<metadata.size(); i++)
    sqlfields.at(i)->setText(metadata.at(i));
  //fix RBW / VBW fields
  _dbv_0->setText(SignalHoundRWBLookup(_dbv_0->text()));
  _dbv_1->setText(SignalHoundRWBLookup(_dbv_1->text()));
  muteSql( ! ((metadata.size() == 18 && sqlTables->count() != 0)) ) ;
  timeIndex->setValue(0);
  plot->loadSweep(0);
  applyZoomWindow();
}
void SweepMainWin::openCSV(void) {
  /*Open a previously recorded CSV data file*/
  QString fname =  QFileDialog::getOpenFileName(this,"Open Signal Hound CSV Recording", QString(),"Spectral Signal Hound CSV Recordings (*.csv *.dat *.sshdata);;All Files (*)", 0, QFileDialog::ReadOnly);
  if (fname == "") return;
  if (!data->openCSV(fname)) {
    openedFile->setText(QString("Unable to open %1").arg(fname));
    return;
  }
  openedFile->setText(fname);
  sqlTables->clear();
  updateMetadata();
}
void SweepMainWin::openDB(void) {
   /*Open a previously recorded CSV data file*/
  QString fname =  QFileDialog::getOpenFileName(this,"Open Signal Hound Database Recording", QString(),"Spectral Signal Hound Database Recordings (*.db *.dat *.sshdata *.sqlite *.sql);;All Files (*)", 0, QFileDialog::ReadOnly);
  if (fname == "") return;
  if (!data->openSQL(fname)) {
    openedFile->setText(QString("Unable to open %1").arg(fname));
    return;
  }
  openedFile->setText(fname);
  sqlTables->disconnect();
  sqlTables->clear();
  sqlTables->addItems(data->SQLTables());
  updateMetadata();
  connect(sqlTables, SIGNAL(currentIndexChanged(QString)), this, SLOT(nextSqlTable(QString)));
  sqlTables->setCurrentIndex(-1);
  sqlTables->setCurrentIndex(0);
}
void SweepMainWin::nextSqlTable(QString newtable) {
  if (newtable == "") return;
  if (!data->setSQLTable(newtable)) {
    return;
  }
  updateMetadata(); //reconfigure
}
void SweepMainWin::saveCSV(void) {
  //This is really straight forward
  QString fname = QFileDialog::getSaveFileName(this, "Save Sweep Data", QString(), "Spectral Signal Hound CSV Recordings (*.csv *.dat *.sshdata);;All Files (*)");
  if (fname == "") return;
  QFile csvf(fname);
  if (!csvf.open(QIODevice::WriteOnly | QIODevice::Text)) return;
  QTextStream csv(&csvf);
  csv << "Sweep Time: " << data->timestampFromIndex(timeIndex->value()) << "\n";
  csv << "Sweep Index:" << timeIndex->value() << "\nFrequency (Hz), Power (dBm)"  "\n";
  fsweep sweep = data->getSweep(timeIndex->value());
  foreach(QPointF d, sweep)
    csv <<  QString::number(d.x(), 'f', 2) << "," <<  QString::number(d.y(), 'f', 4) << "\n";
  csvf.close();
}
void SweepMainWin::saveImg(void) {
   QPixmap qPix = plot->grab();
   if(qPix.isNull()){
      qDebug() << "Failed to capture the plot for saving";
      return;
  }
  QString types(  "Portable Network Graphics file (*.png);;JPEG file (*.jpeg);;Bitmap file (*.bmp)");
  QString filter;             // Type of filter
  QString jpegExt=".jpeg", pngExt=".png", tifExt=".tif", bmpExt=".bmp", tif2Ext="tiff";   // Suffix for the files
  QString fn = QFileDialog::getSaveFileName(this,"Save Image", QString(),types, &filter);
  if (fn.isEmpty()) return;
  if (fn.endsWith(jpegExt)) fn.remove(jpegExt); // Remove file extension is already there
  if (fn.endsWith(pngExt)) fn.remove(pngExt);
  if (fn.endsWith(bmpExt)) fn.remove(bmpExt);
  if (filter.contains(jpegExt)) qPix.save(fn + ".jpeg", "JPEG");
  if (filter.contains(pngExt) ) qPix.save(fn + ".png", "PNG");
  if (filter.contains(bmpExt) ) qPix.save(fn + ".bmp", "BMP");
}
void SweepMainWin::showManual(void) {
  //show manual
  browser->setVisible(!browser->isVisible());
  if (browser->isVisible())
    browser->setMinimumSize(1000, 600);
}
void SweepMainWin::about(void) {
	SweepAbout abt;
	abt.exec();
}