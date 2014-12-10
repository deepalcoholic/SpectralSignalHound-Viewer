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
#include <QtWidgets>
#include <QtCore>
#include <qwt_color_map.h>
#include <qwt_picker_machine.h>
#include <qwt_plot.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_renderer.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_plot_textlabel.h>
#include <qwt_plot_zoomer.h>
#include <qwt_scale_widget.h>
#include <qwt_symbol.h>
#include "helpers.h"
#include "SHData.h"

class SpectralInspector:public QWidget, public Ui::SpectralInspector {
Q_OBJECT
public:
  ~SpectralInspector();
  explicit SpectralInspector(QWidget *parent = 0);
  void setpData(QHoundData *);
  void save(QString);
public slots:
  
private:
  QHoundData * data;
  QwtPlotSpectrogram *spectrogram;
  QwtPlotCanvas *canvas;
  QwtPlotZoomer *zoomer;
  QwtPlotPanner *panner;
  QwtPlotPicker *picker;
  QwtPlotGrid *grid;
};
