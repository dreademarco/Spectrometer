#ifndef COLORMAP_H
#define COLORMAP_H
#include <qwt_color_map.h>
#include <qwt_interval.h>

class CustomColorMap : public QwtLinearColorMap
{
public:
    enum colormap{STANDARD, GRAY, JET, YELLOW};
    CustomColorMap();
    CustomColorMap(colormap);
private:
    void initColorMap();
    colormap m_colormap;
    //QwtLinearColorMap colorMapQwt;
};

#endif // COLORMAP_H
