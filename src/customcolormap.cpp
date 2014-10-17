#include "customcolormap.h"

CustomColorMap::CustomColorMap() : QwtLinearColorMap()
{
    m_colormap = STANDARD;
    initColorMap();
}

CustomColorMap::CustomColorMap(colormap selected_colormap) : QwtLinearColorMap()
{
    m_colormap = selected_colormap;
    initColorMap();
}

void CustomColorMap::initColorMap(){
    setMode(ScaledColors);
    switch (m_colormap)
    {
    case STANDARD:
        setColorInterval(Qt::darkCyan, Qt::red);
        addColorStop(0.1,Qt::cyan);
        addColorStop(0.6, Qt::green);
        addColorStop(0.95, Qt::yellow);
        break;
    case GRAY:
        setColorInterval(Qt::black, Qt::white);
        break;
    case JET:
        double pos;
        setColorInterval(QColor(0,0,189), QColor(132,0,0));
        pos = 1.0/13.0*1.0; addColorStop(pos, QColor(0,0,255));
        pos = 1.0/13.0*2.0; addColorStop(pos, QColor(0,66,255));
        pos = 1.0/13.0*3.0; addColorStop(pos, QColor(0,132,255));
        pos = 1.0/13.0*4.0; addColorStop(pos, QColor(0,189,255));
        pos = 1.0/13.0*5.0; addColorStop(pos, QColor(0,255,255));
        pos = 1.0/13.0*6.0; addColorStop(pos, QColor(66,255,189));
        pos = 1.0/13.0*7.0; addColorStop(pos, QColor(132,255,132));
        pos = 1.0/13.0*8.0; addColorStop(pos, QColor(189,255,66));
        pos = 1.0/13.0*9.0; addColorStop(pos, QColor(255,255,0));
        pos = 1.0/13.0*10.0; addColorStop(pos, QColor(255,189,0));
        pos = 1.0/13.0*12.0; addColorStop(pos, QColor(255,66,0));
        pos = 1.0/13.0*13.0; addColorStop(pos, QColor(189,0,0));
        break;
    }
}
