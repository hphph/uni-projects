#include "ekran.h"
#include <cmath>

Ekran::Ekran(QWidget *parent)
    : QWidget{parent}
{
    result = QImage(250, 250, QImage::Format_RGB32);
    result.fill(QColor("white"));
    currentLayerIndex = 0;
}

void Ekran::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawImage(0, 0, result);
}

void Ekran::DrawPixel(int x, int y, RGBColor color, QImage &im)
{
    if(x < 0 || y < 0 || x >= im.width() || y >= im.height()) return;
    uchar *pix = im.scanLine(y) + 4*x;
    pix[0] = color.blue;
    pix[1] = color.green;
    pix[2] = color.red;
}

Ekran::RGBColor Ekran::GetColor(int x, int y, QImage &im)
{
    if(x < 0 || y < 0 || x >= im.width() || y >= im.height()) return RGBColor(0, 0, 0);
    uchar *pix = im.scanLine(y) + 4*x;
    return RGBColor(pix[2], pix[1], pix[0]);
}

void Ekran::multiply()
{
    for(int y = 0; y < result.height(); y++)
    {
        for(int x = 0; x < result.width(); x++)
        {
            RGBColor fgColor = GetColor(x, y, img[currentLayerIndex]);
            RGBColor bgColor = GetColor(x, y, layer[currentLayerIndex+1]);
            RGBColor resultC = fgColor * bgColor;

            DrawPixel(x, y, RGBColor::lerp(resultC, bgColor, alpha[currentLayerIndex]->value()/255.0), layer[currentLayerIndex]);
        }
    }

    mergeLayers();
    update();
}

void Ekran::mergeLayers()
{
    result = img[3].copy();
    for(int y = 0; y < result.height(); y++)
    {
        for(int x = 0; x < result.width(); x++)
        {
            for(int i = 1; i >= 0; i--)
            {
                RGBColor fgColor = GetColor(x, y, layer[i]);
                RGBColor bgColor = GetColor(x, y, result);
                DrawPixel(x, y, RGBColor::lerp(fgColor, bgColor, alpha[i]->value()/255.0), result);
            }
        }
    }
}

void Ekran::normal()
{
    for(int y = 0; y < result.height(); y++)
    {
        for(int x = 0; x < result.width(); x++)
        {
            RGBColor fgColor = GetColor(x, y, img[currentLayerIndex]);
            RGBColor bgColor = GetColor(x, y, layer[currentLayerIndex+1]);
            DrawPixel(x, y, RGBColor::lerp(fgColor, bgColor, alpha[currentLayerIndex]->value()/255.0), layer[currentLayerIndex]);
        }
    }
    mergeLayers();
    update();
}

void Ekran::screen() {
    for(int y = 0; y < result.height(); y++)
    {
        for(int x = 0; x < result.width(); x++)
        {
            RGBColor fgColor = GetColor(x, y, img[currentLayerIndex]);
            RGBColor bgColor = GetColor(x, y, layer[currentLayerIndex+1]);
            RGBColor resultC = fgColor.screen(bgColor);

            DrawPixel(x, y, RGBColor::lerp(resultC, bgColor, alpha[currentLayerIndex]->value()/255.0), layer[currentLayerIndex]);
        }
    }

    mergeLayers();
    update();
}
void Ekran::darken() {
    for(int y = 0; y < result.height(); y++)
    {
        for(int x = 0; x < result.width(); x++)
        {
            RGBColor fgColor = GetColor(x, y, img[currentLayerIndex]);
            RGBColor bgColor = GetColor(x, y, layer[currentLayerIndex+1]);
            RGBColor resultC = fgColor.min(bgColor);

            DrawPixel(x, y, RGBColor::lerp(resultC, bgColor, alpha[currentLayerIndex]->value()/255.0), layer[currentLayerIndex]);
        }
    }

    mergeLayers();
    update();
}
void Ekran::lighten() {
    for(int y = 0; y < result.height(); y++)
    {
        for(int x = 0; x < result.width(); x++)
        {
            RGBColor fgColor = GetColor(x, y, img[currentLayerIndex]);
            RGBColor bgColor = GetColor(x, y, layer[currentLayerIndex+1]);
            RGBColor resultC = fgColor.max(bgColor);

            DrawPixel(x, y, RGBColor::lerp(resultC, bgColor, alpha[currentLayerIndex]->value()/255.0), layer[currentLayerIndex]);
        }
    }

    mergeLayers();
    update();
}
void Ekran::average() {
    for(int y = 0; y < result.height(); y++)
    {
        for(int x = 0; x < result.width(); x++)
        {
            RGBColor fgColor = GetColor(x, y, img[currentLayerIndex]);
            RGBColor bgColor = GetColor(x, y, layer[currentLayerIndex+1]);
            RGBColor resultC = fgColor.avg(bgColor);

            DrawPixel(x, y, RGBColor::lerp(resultC, bgColor, alpha[currentLayerIndex]->value()/255.0), layer[currentLayerIndex]);
        }
    }

    mergeLayers();
    update();
}
