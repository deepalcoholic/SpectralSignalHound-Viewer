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

#include <cmath>
#include <qwt_text.h>
#include <qwt_color_map.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>
#include <qwt_scale_draw.h>
#include <QDateTime>

//formats 403239422 as 403.2MHz
class FreqScaleDraw : public QwtScaleDraw {
public:
    FreqScaleDraw();
    virtual ~FreqScaleDraw();
    virtual QwtText label(double) const;
};

class TimeScaleDraw : public QwtScaleDraw {
public:
    TimeScaleDraw();
    virtual ~TimeScaleDraw();
    virtual QwtText label(double) const;
};

class FreqdBmPicker : public QwtPlotPicker {
public:
	FreqdBmPicker( QWidget *canvas );
	//FreqdBmPicker(int xAxis, int yAxis, RubberBand rubberBand, DisplayMode trackerMode, QwtPlotCanvas* canvas);
private:
	QwtText trackerTextF( const QPointF &pos ) const;
};

class TimeFreqPicker : public QwtPlotPicker {
public:
	TimeFreqPicker(int xAxis, int yAxis, RubberBand rubberBand, DisplayMode trackerMode, QWidget* canvas);
	TimeFreqPicker( QWidget *canvas );
private:
	QwtText trackerTextF( const QPointF &pos ) const;
};
