#ifndef OUTPUTSCREEN_H
#define OUTPUTSCREEN_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QVector2D>
#include <QPaintEvent>
#include <QPainter>
#include <QVector3D>
#include <QMatrix4x4>
#include <QColor>
#include <algorithm>
#include <cmath>
#include <iostream>

class OutputScreen : public QWidget
{
    Q_OBJECT
public:
    explicit OutputScreen(QWidget *parent = nullptr);
    QVector2D MinVector(QVector2D verts[3])
    {
        QVector2D minVec = verts[0];
        if(verts[1].x() < minVec.x()) minVec.setX(verts[1].x());
        if(verts[2].x() < minVec.x()) minVec.setX(verts[2].x());
        if(verts[1].y() < minVec.y()) minVec.setY(verts[1].y());
        if(verts[2].y() < minVec.y()) minVec.setY(verts[2].y());
        return minVec;
    }

    QVector2D MaxVector(QVector2D verts[3])
    {
        QVector2D maxVec = verts[0];
        if(verts[1].x() > maxVec.x()) maxVec.setX(verts[1].x());
        if(verts[2].x() > maxVec.x()) maxVec.setX(verts[2].x());
        if(verts[1].y() > maxVec.y()) maxVec.setY(verts[1].y());
        if(verts[2].y() > maxVec.y()) maxVec.setY(verts[2].y());
        return maxVec;
    }

    struct RGBColor
    {
        uchar red;
        uchar green;
        uchar blue;
        RGBColor(uchar r, uchar g, uchar b) : red(r), green(g), blue(b) {};
        RGBColor(QColor c) : red(c.red()), green(c.green()), blue(c.blue()) {};
        RGBColor operator*(float n)
        {
            return RGBColor(n*red, n*green, n*blue);
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

        QColor toQColor()
        {
            return QColor(red, green ,blue);
        }
    };

    struct BPoint
    {
        float x, y, z;
        BPoint(QVector2D point, QVector2D triangle[3])
        {
            float W = (triangle[0].x()-triangle[2].x())*(triangle[1].y()-triangle[2].y()) - (triangle[0].y()-triangle[2].y())*(triangle[1].x()-triangle[2].x());
            float Wx = (point.x()-triangle[2].x())*(triangle[1].y()-triangle[2].y()) - (point.y()-triangle[2].y())*(triangle[1].x()-triangle[2].x());
            float Wy = (triangle[0].x()-triangle[2].x())*(point.y()-triangle[2].y()) - (triangle[0].y()-triangle[2].y())*(point.x()-triangle[2].x());
            x = Wx/(float)W;
            y = Wy/(float)W;
            z = 1 - x - y;
        }

        QVector2D ToPoint(QVector2D triangle[3])
        {
            return x*triangle[0] + y*triangle[1] + z*triangle[2];
        }
    };

    void DrawTriangle(QVector2D pos[3], QVector2D uvPos[3], QImage &img);
    QVector2D lerp(QVector2D a, QVector2D b, float t) { return t*b + (1 - t)*a; }
    QImage lerpImg(QImage a, QImage b, float t);

    void Morph(QVector2D firstTriangle[3], QImage firstImg, QVector2D finishTriangle[3], QImage finishImg);
protected:
    void paintEvent(QPaintEvent *);
private:
    void DrawPixel(int x, int y, QColor color);
    void DrawPixel(int x, int y, QColor color, QImage &img);
    QColor getColor(int x, int y, QImage &img);
    QColor getColorBilinear(float x, float y, QImage &img);
    QImage outputImg;
    void delay();

signals:

};

#endif // OUTPUTSCREEN_H
