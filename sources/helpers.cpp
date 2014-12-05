/*
 * Copyright (c) 2014, Nick Potts
 * All rights reserved.
 *
 *  This Source Code Form is subject to the terms of the 
 *  Mozilla Public License, v. 2.0. If a copy of the MPL 
 *  was not distributed with this file, You can obtain 
 *  one at http://mozilla.org/MPL/2.0/.
*/

#include "helpers.h"

/*
This is basically various subclassed crap that makes the plots look and feel better
*/

FreqScaleDraw::FreqScaleDraw() : QwtScaleDraw() {}
FreqScaleDraw::~FreqScaleDraw() {}
QwtText FreqScaleDraw::label(double v) const {
  //convert 1320 to 1.3KHz, 15012402 to 15.0MHz ext
  QString rtn;
  switch ( (int) std::log10(v)) {
    case 9: rtn = QString("%1 GHz").arg(v/1e9, 0, 'f', 2); break;
    case 8:
    case 7:
    case 6: rtn = QString("%1 MHz").arg(v/1e6, 0, 'f', 2); break;
    case 5:
    case 4:
    case 3: rtn = QString("%1 kHz").arg(v/1e3, 0, 'f', 2); break;
    case 2:
    case 1:
    case 0: rtn = QString("%1 Hz").arg(v, 0, 'f', 2); break;
  }
  return QwtText(rtn);
}


TimeScaleDraw::TimeScaleDraw() : QwtScaleDraw() {}
TimeScaleDraw::~TimeScaleDraw() {}
QwtText TimeScaleDraw::label(double v) const { //Convert Double date to a string
  return QwtText(QDateTime::fromMSecsSinceEpoch((qint64) (v*1000)).toString("yyyy-MM-dd\nHH:mm:ss.zzz"));
}

FreqdBmPicker::FreqdBmPicker( QWidget *canvas ): QwtPlotPicker( canvas ) {
  setTrackerMode( QwtPicker::AlwaysOn );
  setRubberBand( QwtPlotPicker::CrossRubberBand );
  setStateMachine( new QwtPickerTrackerMachine() );
  setTrackerPen(QColor(Qt::cyan));
  setRubberBandPen(QColor(Qt::cyan));
}
QwtText FreqdBmPicker::trackerTextF( const QPointF &pos ) const {
  QString rtn;
  qDebug() << "here2" << pos;
  switch ( (int) std::log10(pos.x())) {
    case 9: rtn = QString("(%1GHz, %2dBm)").arg(pos.x()/1e9, 0, 'f', 2).arg(pos.y(), 0, 'f', 1); break;
    case 8:
    case 7:
    case 6: rtn = QString("(%1MHz, %2dBm)").arg(pos.x()/1e6, 0, 'f', 2).arg(pos.y(), 0, 'f', 1); break;
    case 5:
    case 4:
    case 3: rtn = QString("(%1kHz, %2dBm)").arg(pos.x()/1e3, 0, 'f', 2).arg(pos.y(), 0, 'f', 1); break;
    case 2:
    case 1:
    case 0: rtn = QString("(%1Hz, %2dBm)").arg(pos.x(), 0, 'f', 2).arg(pos.y(), 0, 'f', 1); break;
  }
  return QwtText(rtn);
}

// TimeFreqPicker::TimeFreqPicker(int x, int y, RubberBand rb, DisplayMode dm, QWidget* w): QwtPlotPicker(x, y, rb, dm, w) {
//   setRubberBand( rb );
//   setTrackerMode( dm );
//   //setStateMachine( new QwtPickerTrackerMachine() );
//   setTrackerPen(QColor(Qt::black));
//   setRubberBandPen(QColor(Qt::black));
//   QwtPicker::setEnabled( true );
// }
TimeFreqPicker::TimeFreqPicker( QWidget *canvas ): QwtPlotPicker( canvas ) {
  setTrackerMode( QwtPicker::AlwaysOn );
  setRubberBand( QwtPlotPicker::CrossRubberBand );
  setStateMachine( new QwtPickerTrackerMachine() );
  setTrackerPen(QColor(Qt::black));
  setRubberBandPen(QColor(Qt::black));
  QwtPicker::setEnabled( true );
  qDebug() << "Picker created" << this;
}
QwtText TimeFreqPicker::trackerTextF( const QPointF &pos ) const {
  QString rtn = QDateTime::fromMSecsSinceEpoch((qint64) (pos.x()*1000)).toString("yyyy-MM-dd  %2\nHH:mm:ss.zzz");
  switch ( (int) std::log10(pos.y())) {
    case 9: rtn = rtn.arg(pos.y()/1e9, 0, 'f', 2); break;
    case 8:
    case 7:
    case 6: rtn = rtn.arg(pos.y()/1e6, 0, 'f', 2); break;
    case 5:
    case 4:
    case 3: rtn = rtn.arg(pos.y()/1e3, 0, 'f', 2); break;
    case 2:
    case 1:
    case 0: rtn = rtn.arg(pos.y(), 0, 'f', 2); break;
  }
  QwtText r(rtn);
  QColor bgColor(Qt::black);
  bgColor.setAlpha(160);
  r.setBackgroundBrush(QBrush(bgColor));
  return r;
}