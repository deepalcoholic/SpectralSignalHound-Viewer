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
#ifndef QT_STATIC_CONST
#define QT_STATIC_CONST static const
#endif
#include <cmath>
#include <QtWidgets>
#include <QtCore>
#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_renderer.h>
#include <qwt_plot_textlabel.h>
#include <qwt_plot_zoomer.h>
#include <qwt_scale_engine.h>
#include <qwt_scale_draw.h>
#include <qwt_symbol.h>
#include "helpers.h"
#include "SHData.h"

class SweepInspector: public QwtPlot {
Q_OBJECT
public:
  ~SweepInspector();
  explicit SweepInspector(QWidget *parent = 0);
  void setpData(QHoundData *);
  void save(QString);
public slots:
  void loadSweep(int);
  void zoomed(const QRectF &);

signals:
  void zoominated(const QRectF &);

private:
  QHoundData * data;
  QwtPlotCurve *d_curve;
  QwtPlotCanvas *canvas;
  QwtPlotZoomer *zoomer;
  QwtPlotPanner *panner;
  //FreqdBmPicker *picker;
  QwtPlotPicker *picker;
  QwtPlotGrid *grid;
};
