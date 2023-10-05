#ifndef EKRAN_H
#define EKRAN_H

#include <QWidget>
#include <QImage>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QColor>
#include <QSlider>

class Ekran : public QWidget
{
    Q_OBJECT
public:
    explicit Ekran(QWidget *parent = nullptr);

    struct RGBColor
    {
        uchar red;
        uchar green;
        uchar blue;
        RGBColor(uchar r, uchar g, uchar b) : red(r), green(g), blue(b) {};

        RGBColor operator*(float n)
        {
            return RGBColor(n*red, n*green, n*blue);
        }
        RGBColor operator*(RGBColor color)
        {
            RGBColor result(255 * ((red/255.0) * (color.red/255.0)), 255 * ((green/255.0) * (color.green/255.0)), 255 * ((blue/255.0) * (color.blue/255.0)));
            return result;
        }
        RGBColor screen(RGBColor color)
        {
            RGBColor result(255, 255, 255);
            RGBColor x(255 - red, 255 - green, 255 - blue);
            RGBColor y(255 - color.red, 255 - color.green, 255 - color.blue);
            x = x*y;
            result.red = 255 - x.red;
            result.blue = 255 - x.blue;
            result.green = 255 - x.green;
            return result;
        }
        RGBColor min(RGBColor color)
        {
            RGBColor result(std::min(red, color.red), std::min(green, color.green), std::min(blue, color.blue));
            return result;
        }
        RGBColor max(RGBColor color)
        {
            RGBColor result(std::max(red, color.red), std::max(green, color.green), std::max(blue, color.blue));
            return result;
        }
        RGBColor avg(RGBColor color)
        {
            RGBColor result(((int)color.red + (int)red)/2, ((int)color.green + (int)green)/2, ((int)color.blue + (int)blue)/2);
            return result;
        }
        RGBColor operator-(RGBColor color)
        {
            return RGBColor(abs(red - color.red), abs(green - color.green), abs(blue - color.blue));
        }
        RGBColor operator+(RGBColor n)
        {
            return RGBColor(n.red+red, n.green+green, n.blue+blue);
        }
        bool operator==(RGBColor compare)
        {
            if(red != compare.red || blue != compare.blue || green != compare.green) return false;
            return true;
        }
        bool operator!=(RGBColor compare)
        {
            return !(*(this) == compare);
        }
        static RGBColor lerp(RGBColor fgColor, RGBColor bgColor, float t)
        {
            RGBColor result(0, 0, 0);
            result = fgColor * t + bgColor * (1-t);
            return result;
        }
    };
    RGBColor GetColor(int x, int y, QImage &im);
    void DrawPixel(int x, int y, RGBColor color, QImage &im);

    QImage img[3];
    QImage layer[3];
    void mergeLayers();
    QSlider *alpha[3];
protected:
    void paintEvent(QPaintEvent *);
private:
    QImage result;
    int currentLayerIndex;
signals:
public slots:
    void multiply();
    void screen();
    void darken();
    void lighten();
    void average();
    void normal();

    void changeToFirstLayer()
    {
        currentLayerIndex = 0;
    }
    void changeToSecondLayer()
    {
        currentLayerIndex = 1;
    }
};

#endif // EKRAN_H
