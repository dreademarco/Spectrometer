#ifndef COLORMAP_H
#define COLORMAP_H
#include <qwt_color_map.h>
#include <qwt_interval.h>

class ColorMap : public QwtLinearColorMap
{
public:
    enum colormap{standard, gray, jet};
    ColorMap();
    ColorMap(colormap selected_colormap);
private:
    void initColorMap();
    colormap m_colormap;
    //QwtLinearColorMap colorMapQwt;
};

#endif // COLORMAP_H
