#ifndef EKRAN_H
#define EKRAN_H

#include "qpushbutton.h"
#include <QSlider>
#include <QWidget>
#include <QMouseEvent>

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
        RGBColor(QColor col) : red(col.red()), green(col.green()), blue(col.blue()) {};
        bool operator==(RGBColor compare)
        {
            if(red != compare.red || blue != compare.blue || green != compare.green) return false;
            return true;
        }
        bool operator!=(RGBColor compare)
        {
            if(red != compare.red || blue != compare.blue || green != compare.green) return true;
            return false;
        }
    };

    void DrawPixel(int x, int y, uchar r, uchar g, uchar b);
    void DrawPixel(QVector2D pos, RGBColor color);
    void DrawLine(int x1, int y1, int x2, int y2, uchar r, uchar g, uchar b);
    void DrawCircle(int x1, int y1, int radius, uchar r, uchar g, uchar b);
    void DrawLineCircle(int N, int x1, int y1, int radius, uchar r, uchar g, uchar b);
    void DrawElipse(int N, int x0, int y0, int rx, int ry, uchar r, uchar g, uchar b);
    QVector2D Bezier(float t, QVector2D p0, QVector2D p1, QVector2D p2, QVector2D p3);
    QVector2D BSpline(float t, QVector2D p0, QVector2D p1, QVector2D p2, QVector2D p3);
    void DrawBezier(int N, QVector2D p0, QVector2D p1, QVector2D p2, QVector2D p3, uchar r, uchar g, uchar b);
    void DrawBSpline(int N, QVector2D p0, QVector2D p1, QVector2D p2, QVector2D p3, uchar r, uchar g, uchar b);
    RGBColor GetColor(int x, int y);
    void FloodFill(int x, int y, RGBColor newColor);
    void ScanLine(std::vector<QVector2D> points, RGBColor color);
    void LateUpdate();
    QSlider *slider = nullptr;
    QPushButton *buttonBezierPlus = nullptr;
    QPushButton *buttonBezierMinus = nullptr;
    QPushButton *buttonBSplinePlus = nullptr;
    QPushButton *buttonBSplineMinus = nullptr;
    QPushButton *buttonFloodFill = nullptr;
    QPushButton *buttonScanLinePlus = nullptr;
    QPushButton *buttonScanLineMinus = nullptr;
protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
private:
    QImage im, lastImage;
    int pressedMousePosX, pressedMousePosY;
    int mousePressedTimes;
    QVector2D *modifiedBezierPoint = nullptr;
    QVector2D *modifiedBSplinePoint = nullptr;
    RGBColor currentColor = RGBColor(255, 255, 255);
    enum State
    {
        line,
        circle,
        lineCircle,
        elipse,
        bezierPlus,
        bezierMinus,
        bSplinePlus,
        bSplineMinus,
        floodFill,
        scanLinePlus,
        scanLineDraw
    };
    State currentState;
    std::vector<QVector2D> bezierPoints;
    std::vector<QVector2D> bSplinePoints;
    std::vector<QVector2D> scanLinePoints;
private slots:
    void onButtonBezierPlusPressed();
    void onButtonBezierMinusPressed();
    void onButtonBSplinePlusPressed();
    void onButtonBSplineMinusPressed();
    void onButtonFloodFillPressed();
    void onButtonScanLinePlusPressed();
};

#endif // EKRAN_H
