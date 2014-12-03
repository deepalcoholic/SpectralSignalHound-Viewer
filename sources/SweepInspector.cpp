/*
demod display that listens for json replies over udp port 57000

$Revision: 815 $
$Date: 2014-10-27 13:20:22 -0600 (Mon, 27 Oct 2014) $
$Author: npotts $
$HeadURL: https://proxy.subversion.ucar.edu/AVAPS/trunk/cpp-apps/CommonElements/sources/doer-demod-display.cpp $
$Id: doer-demod-display.cpp 815 2014-10-27 19:20:22Z npotts $
*/
#include "SweepInspector.h"

SweepInspector::~SweepInspector() {
  /** destroy stuffs  */
  delete(d_curve);
  delete(canvas);
  delete(zoomer);
  delete(panner);
}
SweepInspector::SweepInspector(QWidget *parent) : QWidget(parent), data(NULL), d_curve(NULL) {
  setupUi(this);
  plot->setObjectName( "SweepData" );
  plot->setTitle( "RF Sweep" );
  plot->setAxisTitle( QwtPlot::xBottom, "Frequency");
  plot->setAxisTitle( QwtPlot::yLeft, QString( "Power Level (dBm)"));
  plot->setAutoReplot(true);

  canvas = new QwtPlotCanvas();
  canvas->setPalette( Qt::black );
  canvas->setBorderRadius(0);
  plot->setCanvas(canvas);

  zoomer = new QwtPlotZoomer( canvas );
  zoomer->setRubberBandPen( QColor( Qt::white ) );
  zoomer->setTrackerPen( QColor( Qt::white ) );
  zoomer->setMousePattern( QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier );
  zoomer->setMousePattern( QwtEventPattern::MouseSelect3, Qt::RightButton );

  panner = new QwtPlotPanner( canvas );
  panner->setMouseButton( Qt::MidButton );

  //connects
  connect(timeIndex, SIGNAL(valueChanged(int)), this, SLOT(loadSweep(int)));
  connect(timeIndex, SIGNAL(sliderMoved(int)), this, SLOT(sliderMoved(int)));
}
void SweepInspector::setpData(QHoundData *newd) {
  data = newd;
  //setup the bar
  timeIndex->setMaximum(data->getNumSweeps()-1);
  timeIndex->setValue(0);
}
void SweepInspector::sliderMoved(int index) {
  if (data == NULL) 
    timestamp->setText("");
  else
    timestamp->setText(data->timestampFromIndex(index));
}
void SweepInspector::loadSweep(int index) {
  //load in new sweep values from data
  if (data == NULL) return;
  sliderMoved(index);
  if (d_curve) {
    d_curve->attach(NULL);
    delete(d_curve);
  }
  plot->detachItems(QwtPlotItem::Rtti_PlotItem, true);
  plot->detachItems(QwtPlotItem::Rtti_PlotCurve);
  plot->detachItems();
  plot->replot();
  plot->repaint();
  
  fsweep sweep = data->getSweep(index);

  d_curve = new QwtPlotCurve( timestamp->text() ); //Qwt will delete() this when its done with it
  d_curve->setRenderHint( QwtPlotItem::RenderAntialiased );
  d_curve->setStyle( QwtPlotCurve::Lines );
  d_curve->setPen( QColor( Qt::yellow ), 2, Qt::SolidLine );
  d_curve->setSamples( sweep );
  d_curve->attach(plot);

  range freqs = data->limits(FREQ);
  plot->setAxisScale(QwtPlot::xBottom, freqs.first, freqs.second, (freqs.second - freqs.first)/ 5.0);
  plot->setAxisScale(QwtPlot::yLeft, -135, 20, 10.0);
  plot->replot();
  plot->repaint();

  //set maximum zoom out
  zoomer->setZoomBase(QRectF(QPointF(freqs.first, -60), QPointF(freqs.second, -135)));
}
