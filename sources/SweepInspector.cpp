/*
 * Copyright (c) 2014, Nick Potts
 * All rights reserved.
 *
 *  This Source Code Form is subject to the terms of the 
 *  Mozilla Public License, v. 2.0. If a copy of the MPL 
 *  was not distributed with this file, You can obtain 
 *  one at http://mozilla.org/MPL/2.0/.
*/

#include "SweepInspector.h"

SweepInspector::~SweepInspector() {
  /** destroy stuffs  */
  detachItems();
  delete(d_curve);
  delete(canvas);
  delete(panner);
  delete(picker);
  delete(grid);
  delete(zoomer);
}
SweepInspector::SweepInspector(QWidget *parent) : QwtPlot(parent), data(NULL), d_curve(NULL), picker(NULL) {
  setObjectName( "SweepData" );
  setTitle( "RF Sweep" );
  setAxisTitle( QwtPlot::xBottom, "Frequency");
  setAxisTitle( QwtPlot::yLeft, QString( "Power Level (dBm)"));
  setAutoReplot(true);

  enableAxis(QwtPlot::xBottom, true);
  enableAxis(QwtPlot::yLeft, true);
  enableAxis(QwtPlot::xTop, false);
  enableAxis(QwtPlot::yRight, false);

  canvas = new QwtPlotCanvas();
  canvas->setPalette( Qt::black );
  canvas->setBorderRadius(0);
  setCanvas(canvas);

  //Allow zooming / panning
  zoomer = new QwtPlotZoomer( canvas );
  zoomer->setRubberBandPen( QColor( Qt::white ) );
  zoomer->setTrackerPen( QColor( Qt::white ) );
  connect(zoomer, SIGNAL(zoomed(const QRectF &)), this, SLOT(zoomed(const QRectF &)));

  panner = new QwtPlotPanner( canvas );
  panner->setMouseButton( Qt::MidButton );

  //Show the X/Y markers that follow the mouse
  picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn, canvas);
  picker->setStateMachine( new QwtPickerTrackerMachine() );
  picker->setRubberBandPen( QColor( Qt::cyan ) );
  picker->setTrackerPen( QColor( Qt::cyan ) );

  //FreqdBmPicker *er = new FreqdBmPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn, canvas);

  //Setup grid
  grid = new QwtPlotGrid();
  grid->enableXMin( true );
  grid->enableYMin( true );
  QColor color(Qt::gray); color.setAlpha(128);
  grid->setMajorPen( color, 1, Qt::DotLine );
  grid->setMinorPen( color, 1, Qt::DotLine );
  grid->attach( this );

  //format in kHz, MHz, GHz, not raw values
  setAxisScaleDraw(QwtPlot::xBottom, new FreqScaleDraw);
  setAxisAutoScale(QwtPlot::xBottom, true);
  //setAxisScale(QwtPlot::xBottom, 1, 4.4e9, 4.4e9 / 5.0);
  //setAxisScale(QwtPlot::yLeft, -135, 20, 20.0);

  setAxisScale(QwtPlot::yRight, 0, 10, 1);
  setAxisScale(QwtPlot::xTop, 0, 10, 1);
  
  repaint();
  replot();
}
void SweepInspector::setpData(QHoundData *newd) {
  data = newd;
}
void SweepInspector::loadSweep(int index) {
  //load in new sweep values from data
  if (data == NULL) return;
  if ( (index < 0)  || (index >= data->getNumSweeps())) return;

  //remove old data and get new
  if (d_curve) d_curve->attach(NULL);
  detachItems();
  fsweep sweep = data->getSweep(index);

  d_curve = new QwtPlotCurve( data->timestampFromIndex(index) ); //Qwt will delete() this when its done with it
  d_curve->setRenderHint( QwtPlotItem::RenderAntialiased );
  d_curve->setStyle( QwtPlotCurve::Lines );
  d_curve->setPen( QColor( Qt::yellow ), 2, Qt::SolidLine );
  d_curve->setSamples( sweep );
  d_curve->attach(this);

  QwtInterval frange = data->limits(FREQ);
  //setAxisScale(QwtPlot::xBottom, frange.minValue(), frange.maxValue(), (frange.maxValue() - frange.minValue())/ 5.0);
  //setAxisScale(QwtPlot::yLeft, -135, 20, 10.0);
  setTitle( QString("RF Sweep @ %1").arg(data->timestampFromIndex(index)) );
  //set maximum zoom out
  zoomer->setZoomBase(QRectF(QPointF(frange.minValue(), 40), QPointF(frange.maxValue(), -135)));
  zoomer->zoomBase();

  //find max, min, and average values and drop it on plot as well
  double max = sweep.first().y(), min = sweep.first().y(), avg=0;
  for(int i=0; i < sweep.size(); i++) {
    max = std::max(max, sweep.at(i).y());
    min = std::min(min, sweep.at(i).y());
    avg += sweep.at(i).y();
  } avg /= sweep.size();

  //add markers onto the plot
  QwtPlotMarker *one = new QwtPlotMarker(), *two = new QwtPlotMarker();
  one->attach(this); one->setAxes(QwtPlot::xTop, QwtPlot::yRight);
  two->attach(this); two->setAxes(QwtPlot::xTop, QwtPlot::yRight);

  QwtText tone = QwtText(QString("Max: %1 dBm\nMin: %2 dBm\nAvg: %3 dBm").arg(max).arg(min).arg(avg));
  tone.setFont( QFont( "Helvetica", 10, QFont::Bold ) );
  tone.setColor( Qt::green );
  tone.setRenderFlags(Qt::AlignTop | Qt::AlignLeft);

  one->setLabel(tone);
  one->setValue(0, 10);
  one->setLabelAlignment(Qt::AlignBottom | Qt::AlignRight);

  QwtText ttwo(data->plotText());
  ttwo.setFont( QFont( "Helvetica", 10, QFont::Bold ) );
  ttwo.setColor( Qt::white );
  ttwo.setRenderFlags(Qt::AlignBottom | Qt::AlignRight);

  two->setLabel(ttwo);
  two->setValue(10, 10);
  two->setLabelAlignment(Qt::AlignBottom | Qt::AlignLeft);

  replot();
  repaint();
}
void SweepInspector::save(QString filename) {
  QwtPlotRenderer renderer;
  renderer.exportTo( this, filename);
}
void SweepInspector::zoomed(const QRectF &rect) {
  emit zoominated(rect);
}
