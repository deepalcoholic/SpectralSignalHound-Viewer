/*
demod display that listens for json replies over udp port 57000

$Revision: 725 $
$Date: 2014-05-22 15:28:57 -0600 (Thu, 22 May 2014) $
$Author: npotts $
$HeadURL: https://proxy.subversion.ucar.edu/AVAPS/trunk/cpp-apps/CommonElements/sources/doer-demod-display.h $
$Id: doer-demod-display.h 725 2014-05-22 21:28:57Z npotts $
*/

#pragma once
#define QT_STATIC_CONST static const
#include "ui_SweepInspector.h"
#include "SweepInspector.moc"
#include <QtWidgets>
#include <QtCore>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_canvas.h>

#include "SHData.h"

class SweepInspector:public QWidget, public Ui::sweepinspector {
Q_OBJECT
public:
  ~SweepInspector();
  explicit SweepInspector(QWidget *parent = 0);
  void setpData(QHoundData *);
public slots:
  void sliderMoved(int);
  void loadSweep(int);
private:
  QHoundData * data;
  QwtPlotCurve *d_curve;
  QwtPlotCanvas *canvas;
  QwtPlotZoomer* zoomer;
  QwtPlotPanner *panner;
  
};
