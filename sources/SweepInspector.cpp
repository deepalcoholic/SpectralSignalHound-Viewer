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
}
SweepInspector::SweepInspector(QWidget *parent) : QWidget(parent), data(NULL), d_curve(NULL) {
  setupUi(this);
  plot->setObjectName( "SweepData" );
  plot->setTitle( "RF Sweep" );
  plot->setAxisTitle( QwtPlot::xBottom, "Frequency" );
  //plot->setAxisScaleDiv(QwtPlot::xBottom, yearScaleDiv() );
  //plot->setAxisScaleDraw(QwtPlot::xBottom, new YearScaleDraw() );
  plot->setAxisTitle( QwtPlot::yLeft, QString( "Power Level (dBm)"));
  plot->setAutoReplot(true);

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
  if (d_curve) d_curve->attach(NULL);
  plot->detachItems(QwtPlotItem::Rtti_PlotItem, true);
  plot->detachItems(QwtPlotItem::Rtti_PlotCurve);
  plot->detachItems();
  //plot->data->clear();
  plot->replot();
  plot->repaint();
  qDebug() << plot->autoReplot();
  
  fsweep sweep = data->getSweep(index);

  d_curve = new QwtPlotCurve( timestamp->text() ); //Qwt will delete() this when its done with it
  d_curve->setRenderHint( QwtPlotItem::RenderAntialiased );
  d_curve->setStyle( QwtPlotCurve::Lines );
  d_curve->setPen( QColor( Qt::yellow ), 2, Qt::SolidLine );
  QwtSymbol *symbol = new QwtSymbol( QwtSymbol::XCross );
  symbol->setSize( 4 );
  symbol->setPen( QColor( Qt::yellow ), 2, Qt::SolidLine );
  //d_curve->setSymbol( symbol );
  d_curve->setSamples( sweep );
  d_curve->attach(plot);

  range freqs = data->limits(FREQ);
  double step = (freqs.second - freqs.first)/ 5.0;

  plot->setAxisScale(QwtPlot::xBottom, freqs.first, freqs.second, step);
  plot->setAxisScale(QwtPlot::yLeft, -135, 20, 10.0);
  plot->replot();
  plot->repaint();
}
