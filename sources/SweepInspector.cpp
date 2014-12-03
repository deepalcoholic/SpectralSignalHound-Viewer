/*
demod display that listens for json replies over udp port 57000

$Revision: 815 $
$Date: 2014-10-27 13:20:22 -0600 (Mon, 27 Oct 2014) $
$Author: npotts $
$HeadURL: https://proxy.subversion.ucar.edu/AVAPS/trunk/cpp-apps/CommonElements/sources/doer-demod-display.cpp $
$Id: doer-demod-display.cpp 815 2014-10-27 19:20:22Z npotts $
*/
#include "SweepInspector.h"

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

SweepInspector::~SweepInspector() {
  /** destroy stuffs  */
  delete(d_curve);
  delete(canvas);
  delete(zoomer);
  delete(panner);
  delete(picker);
  delete(grid);
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

  //Allow zooming / panning
  zoomer = new QwtPlotZoomer( canvas );
  zoomer->setRubberBandPen( QColor( Qt::white ) );
  zoomer->setTrackerPen( QColor( Qt::white ) );
  panner = new QwtPlotPanner( canvas );
  panner->setMouseButton( Qt::MidButton );

  //Show the X/Y markers that follow the mouse
  picker = new QwtPlotPicker( QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn, canvas);
  picker->setStateMachine( new QwtPickerTrackerMachine() );
  picker->setRubberBandPen( QColor( Qt::cyan ) );
  picker->setRubberBand( QwtPicker::CrossRubberBand );
  picker->setTrackerPen( QColor( Qt::cyan ) );

  //Setup grid
  grid = new QwtPlotGrid();
  grid->enableXMin( true );
  grid->enableYMin( true );
  QColor color(Qt::gray); color.setAlpha(128);
  grid->setMajorPen( color, 1, Qt::DotLine );
  grid->setMinorPen( color, 1, Qt::DotLine );
  grid->attach( plot );

  //format in kHz, MHz, GHz, not raw values
  plot->setAxisScaleDraw(QwtPlot::xBottom, new FreqScaleDraw);

  //tmax.setFont( QFont( "Helvetica", 10, QFont::Bold ) ); tmax.setColor( Qt::green );
  //tmin.setFont( QFont( "Helvetica", 10, QFont::Bold ) ); tmin.setColor( Qt::green );
  //tavg.setFont( QFont( "Helvetica", 10, QFont::Bold ) ); tavg.setColor( Qt::green );

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
  //update text despite not letting go
  if (data == NULL) 
    timestamp->setText("");
  else
    timestamp->setText(data->timestampFromIndex(index));
}
void SweepInspector::loadSweep(int index) {
  //load in new sweep values from data
  if (data == NULL) return;
  sliderMoved(index);

  //remove old data and get new
  if (d_curve) d_curve->attach(NULL);
  plot->detachItems();
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
  plot->setTitle( QString("RF Sweep @ %1").arg(timestamp->text()) );
  //set maximum zoom out
  zoomer->setZoomBase(QRectF(QPointF(freqs.first, -60), QPointF(freqs.second, -135)));
  zoomer->zoomBase();


  //find max, min, and average values and drop it on plot as well
  double max = freqs.first, min = freqs.first, average=0;
  for(int i=0; i < sweep.size(); i++) {
    max = std::max(max, sweep.at(i).y());
    min = std::min(min, sweep.at(i).y());
    average += sweep.at(i).y();
  } average /= sweep.size();

  // tmax.setText(QString("Max: %1 dBm").arg(max));
  // tmin.setText(QString("Min: %1 dBm").arg(min));
  // tavg.setText(QString("Avg: %1 dBm").arg(average));

  plot->replot();
  plot->repaint();
}
