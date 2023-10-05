
#ifndef EKRAN_H
#define EKRAN_H


#include <QWidget>
#include <QSlider>
#include <QImage>


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
        bool operator==(RGBColor compare)
        {
            if(red != compare.red || blue != compare.blue || green != compare.green) return false;
            return true;
        }
        bool operator!=(RGBColor compare)
        {
            return !(*(this) == compare);
        }
        uchar max()
        {
            uchar max = 0;
            if(red > max) max = red;
            if(blue > max) max = blue;
            if(green > max) max = green;
            return max;
        }
        uchar min()
        {
            uchar min = 255;
            if(red < min) min = red;
            if(blue < min) min = blue;
            if(green < min) min = green;
            return min;
        }
    };
    struct HSVColor
    {
        short hue;
        short saturation;
        short value;
        HSVColor(short hue, short saturation, short value) : hue(hue), saturation(saturation), value(value) {};
        bool operator==(HSVColor compare)
        {
            if(hue != compare.hue || saturation != compare.saturation || value != compare.value) return false;
            return true;
        }
        bool operator!=(HSVColor compare)
        {
            return !(*(this) == compare);
        }
    };

    QSlider* rgbSliders[3];
    QSlider* hsvSliders[3];

    void DrawPixel(int x, int y, uchar r, uchar g, uchar b);
    void DrawPixel(QVector2D pos, RGBColor color);
    void DrawPixel(QVector2D pos, HSVColor color);
    HSVColor RGBtoHSV(RGBColor color);
    RGBColor HSVtoRGB(HSVColor color);
    void LateUpdate();
signals:

protected:
    void paintEvent(QPaintEvent *);
private:
    QImage im;
    enum SliderState {rSlider, gSlider, bSlider, hSlider, sSlider, vSlider};
    SliderState sliderState;
    void OnSlidersUpdate();
    void OnRSliderUpdate();
    void OnGSliderUpdate();
    void OnBSliderUpdate();
    void OnHSliderUpdate();
    void OnSSliderUpdate();
    void OnVSliderUpdate();

};

#endif // EKRAN_H
