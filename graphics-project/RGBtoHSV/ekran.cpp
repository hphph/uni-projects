#include "ekran.h"
#include <QPainter>
#include <cmath>
#include <QVector2D>

Ekran::Ekran(QWidget *parent) : QWidget{parent}
{
    im = QImage(500, 500, QImage::Format_RGB32);
    sliderState = rSlider;
    im.fill(0);
}

void Ekran::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawImage(0, 0, im);
}

void Ekran::DrawPixel(int x, int y, uchar r, uchar g, uchar b)
{
    if(x < 0 || y < 0 || x >= im.width() || y >= im.height()) return;
    uchar *pix = im.scanLine(y) + 4*x;
    pix[0] = b;
    pix[1] = g;
    pix[2] = r;
}

void Ekran::DrawPixel(QVector2D pos, RGBColor color)
{
    DrawPixel(pos.x(), pos.y(), color.red, color.green, color.blue);
}

void Ekran::DrawPixel(QVector2D pos, HSVColor color)
{
    RGBColor rgbColor = HSVtoRGB(color);
    DrawPixel(pos, rgbColor);
}

void Ekran::OnSlidersUpdate()
{
    for(int y = 0; y<im.height(); y++)
    {
        for(int x = 0; x<im.width(); x++)
        {
            switch(sliderState)
            {
            case(rSlider):
                DrawPixel(QVector2D(x, y), RGBColor(rgbSliders[0]->value(), 255 * ((double)(im.height()-y)/im.height()), 255 * ((double)x/im.width())));
                break;
            case(gSlider):
                DrawPixel(QVector2D(x, y), RGBColor(255 * ((double)(im.height()-y)/im.height()), rgbSliders[1]->value(), 255 * ((double)x/im.width())));
                break;
            case(bSlider):
                DrawPixel(QVector2D(x, y), RGBColor(255 * ((double)(im.height()-y)/im.height()), 255 * ((double)x/im.width()), rgbSliders[2]->value()));
                break;
            case(hSlider):
                DrawPixel(QVector2D(x, y), HSVColor(hsvSliders[0]->value(), 100 * ((double)x/im.width()), 100 * ((double)(im.height()-y)/im.height())));
                break;
            case(sSlider):
                DrawPixel(QVector2D(x, y), HSVColor(360 * ((double)(im.height()-y)/im.height()), hsvSliders[1]->value(), 100 * ((double)x/im.width())));
                break;
            case(vSlider):
                DrawPixel(QVector2D(x, y), HSVColor(360 * ((double)(im.height()-y)/im.height()), 100 * ((double)x/im.width()), hsvSliders[2]->value()));
                break;
            }


        }
    }
    update();
}

void Ekran::OnRSliderUpdate()
{
    sliderState = rSlider;
}

void Ekran::OnGSliderUpdate()
{
    sliderState = gSlider;
}

void Ekran::OnBSliderUpdate()
{
    sliderState = bSlider;
}

void Ekran::OnHSliderUpdate()
{
    sliderState = hSlider;
}

void Ekran::OnSSliderUpdate()
{
    sliderState = sSlider;
}

void Ekran::OnVSliderUpdate()
{
    sliderState = vSlider;
}

Ekran::HSVColor Ekran::RGBtoHSV(RGBColor color)
{
    HSVColor newColor(0, 0, 0);
    uchar deltaC = color.max() - color.min();
    if(color.max() == color.min()) newColor.hue = 0;
    else if(color.max() == color.red) newColor.hue = 60 * (short)((color.green - color.blue)/deltaC);
    else if(color.max() == color.green) newColor.hue = 60 * (short)(2 + (color.blue - color.red)/deltaC);
    else newColor.hue = 60 * (short)(4 + (color.red - color.green)/deltaC);
    if(newColor.hue < 0) newColor.hue += newColor.hue + 360;

    newColor.value = color.max();
    newColor.saturation = deltaC/newColor.value;
    return newColor;
}

Ekran::RGBColor Ekran::HSVtoRGB(HSVColor color)
{
    double V = color.value/100.0;
    double S = color.saturation/100.0;
    double H = color.hue/60.0;

    double C = V*S;
    double X = C*(1 - fabs((std::fmod(H,2.0))-1));

    struct RGBdouble
    {
        double r, g, b;
        RGBdouble(double r, double g, double b): r(r), g(g), b(b) {};
    };

    RGBdouble temp(0,0,0);
    if(H >= 0 && H < 1)
        temp = RGBdouble(C,X,0);
    else if(H >= 1 && H < 2)
        temp = RGBdouble(X,C,0);
    else if(H >= 2 && H < 3)
        temp = RGBdouble(0, C, X);
    else if(H >= 3 && H < 4)
        temp = RGBdouble(0, X, C);
    else if(H >= 4 && H < 5)
        temp = RGBdouble(X, 0, C);
    else if(H >= 5 && H < 6)
        temp = RGBdouble(C, 0 ,X);

    double m = V - C;
    RGBColor returncoor = RGBColor(255.0 * (m + temp.r), 255.0 * (m + temp.g), 255.0 * (m + temp.b));
    return returncoor;
}

void Ekran::LateUpdate()
{
    for(int i = 0; i<3; i++)
    {
        connect(rgbSliders[i], &QAbstractSlider::valueChanged, this, &Ekran::OnSlidersUpdate);
        connect(hsvSliders[i], &QAbstractSlider::valueChanged, this, &Ekran::OnSlidersUpdate);
    }
    connect(rgbSliders[0], &QAbstractSlider::valueChanged, this, &Ekran::OnRSliderUpdate);
    connect(rgbSliders[1], &QAbstractSlider::valueChanged, this, &Ekran::OnGSliderUpdate);
    connect(rgbSliders[2], &QAbstractSlider::valueChanged, this, &Ekran::OnBSliderUpdate);
    connect(hsvSliders[0], &QAbstractSlider::valueChanged, this, &Ekran::OnHSliderUpdate);
    connect(hsvSliders[1], &QAbstractSlider::valueChanged, this, &Ekran::OnSSliderUpdate);
    connect(hsvSliders[2], &QAbstractSlider::valueChanged, this, &Ekran::OnVSliderUpdate);
}
