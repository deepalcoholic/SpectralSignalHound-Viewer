/*
 * Copyright (c) 2014, Nick Potts
 * All rights reserved.
 *
 *  This Source Code Form is subject to the terms of the 
 *  Mozilla Public License, v. 2.0. If a copy of the MPL 
 *  was not distributed with this file, You can obtain 
 *  one at http://mozilla.org/MPL/2.0/.
*/

#include "SpectralInspector.h"

class TimeFreqPicker2: public QwtPlotPicker {
public:
  TimeFreqPicker2(int x, int y, RubberBand rb, DisplayMode dm, QWidget* w): QwtPlotPicker(x, y, rb, dm, w) {
    setRubberBand( rb );
    setTrackerMode( dm );
    //setStateMachine( new QwtPickerTrackerMachine() );
    setTrackerPen(QColor(Qt::black));
    setRubberBandPen(QColor(Qt::black));
    QwtPicker::setEnabled( true );
  }
  TimeFreqPicker2( QWidget *canvas ): QwtPlotPicker( canvas ) {
    setTrackerMode( QwtPicker::AlwaysOn );
    setRubberBand( QwtPlotPicker::CrossRubberBand );
    //setStateMachine( new QwtPickerTrackerMachine() );
    setTrackerPen(QColor(Qt::black));
    setRubberBandPen(QColor(Qt::black));
    QwtPicker::setEnabled( true );
  }
  QwtText trackerTextF( const QPointF &pos ) const {
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
    qDebug() << rtn;
    QColor bgColor(Qt::black);
    bgColor.setAlpha(160);
    r.setBackgroundBrush(QBrush(bgColor));
    return r;
  }
};

SpectralInspector::~SpectralInspector() {
  /** destroy stuffs  */
  qDebug() << "here";
  plot->detachItems();
}
SpectralInspector::SpectralInspector(QWidget *parent) : QWidget(parent), data(NULL), spectrogram(NULL) {
  setupUi(this);
  plot->setObjectName( "Spectral" );
  plot->setTitle( "" );
  plot->setAxisTitle( QwtPlot::xBottom, "Time");
  plot->setAxisTitle( QwtPlot::yLeft, QString( "Frequency"));
  plot->setAutoReplot(true);

  plot->enableAxis(QwtPlot::xBottom, true);
  plot->enableAxis(QwtPlot::yLeft, true);
  plot->enableAxis(QwtPlot::xTop, false);
  plot->enableAxis(QwtPlot::yRight, true);

  canvas = new QwtPlotCanvas();
  canvas->setBorderRadius(0);
  plot->setCanvas(canvas);

  //Show the X/Y markers that follow the mouse
  picker = new QwtPlotPicker(canvas);
  picker->setTrackerMode( QwtPicker::AlwaysOn );
  picker->setRubberBand( QwtPlotPicker::CrossRubberBand );
  picker->setStateMachine( new QwtPickerTrackerMachine() );
  picker->setTrackerPen(QColor(Qt::black));
  picker->setRubberBandPen(QColor(Qt::black));

  zoomer = new QwtPlotZoomer( canvas );
  zoomer->setRubberBandPen( QColor( Qt::white ) );
  zoomer->setTrackerPen( QColor( Qt::white ) );
  zoomer->setMousePattern( QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier );
  zoomer->setMousePattern( QwtEventPattern::MouseSelect3, Qt::RightButton );
  const QColor c( Qt::darkBlue );
  zoomer->setRubberBandPen( c );
  zoomer->setTrackerPen( c );

  panner = new QwtPlotPanner( canvas );
  panner->setMouseButton( Qt::MidButton );
  panner->setAxisEnabled( QwtPlot::yRight, false );
  
  //Setup grid
  grid = new QwtPlotGrid();
  grid->enableXMin( true );
  grid->enableYMin( true );
  QColor color(Qt::gray); color.setAlpha(128);
  grid->setMajorPen( color, 1, Qt::DotLine );
  grid->setMinorPen( color, 1, Qt::DotLine );
  grid->attach( plot );

  //format in kHz, MHz, GHz, not raw values
  plot->setAxisScaleDraw(QwtPlot::yLeft, new FreqScaleDraw);
  plot->setAxisScaleDraw(QwtPlot::xBottom, new TimeScaleDraw);
}
void SpectralInspector::setpData(QHoundData *newd) {
  data = newd;

  if (spectrogram) spectrogram->attach(NULL);
  plot->detachItems();

  spectrogram = new QwtPlotSpectrogram();
  spectrogram->setRenderThreadCount( 0 ); // use system specific thread count
  spectrogram->setCachePolicy( QwtPlotRasterItem::PaintCache );

  spectrogram->setData(data);
  spectrogram->attach(plot);

  QwtInterval xlims = data->limits(TIME), ylims = data->limits(FREQ), zlims=QwtInterval(-135, 20);

    // A color bar on the right axis
  QwtScaleWidget *rightAxis = plot->axisWidget( QwtPlot::yRight );
  rightAxis->setTitle("Power (dBm)");
  rightAxis->setColorBarEnabled( true );
  plot->setAxisScale( QwtPlot::yRight, -135, 20);
  plot->plotLayout()->setAlignCanvasToScales( true );

  QwtLinearColorMap *cmap = new QwtLinearColorMap(Qt::blue, Qt::red);
  cmap->addColorStop( 0.25, Qt::cyan );
  cmap->addColorStop( 0.5, Qt::green );
  cmap->addColorStop( 0.75, Qt::yellow );
  spectrogram->setColorMap( cmap );
  rightAxis->setColorMap( zlims, cmap );

  plot->setAxisScale(QwtPlot::xBottom, xlims.minValue(), xlims.maxValue(), (xlims.maxValue() - xlims.minValue())/ 5.0);
  plot->setAxisScale(QwtPlot::yLeft, ylims.minValue(), ylims.maxValue(), (ylims.maxValue() - ylims.minValue())/ 5.0);
  
  zoomer->setZoomBase(QRectF(QPointF(xlims.minValue(), ylims.maxValue()), QPointF(xlims.maxValue(), ylims.minValue())));
  zoomer->zoomBase();
}
void SpectralInspector::save(QString filename) {
  QwtPlotRenderer renderer;
  renderer.exportTo( plot, filename);
}
