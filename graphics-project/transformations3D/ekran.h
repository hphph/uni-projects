#ifndef EKRAN_H
#define EKRAN_H

#include <QWidget>
#include <QImage>
#include <QPaintEvent>
#include <QPainter>
#include <QVector3D>
#include <QMatrix4x4>
#include <QColor>
#include <algorithm>
#include <cmath>

class Ekran : public QWidget
{
    Q_OBJECT
public:
    explicit Ekran(QWidget *parent = nullptr);
    void DrawPixel(int x, int y, QColor color);
    void DrawLine(int x1, int y1, int x2, int y2, QColor color);
    void DrawTriangle(QVector3D verts[3], QVector2D uvPos[3], QImage &img, QVector3D normal);
    QColor getColor(int x, int y, QImage &img);
    QColor getColorBilinear(float x, float y, QImage &img);
protected:
    void paintEvent(QPaintEvent *);
private:
    void countAllTransformations();
    void paintCube();
    QMatrix4x4 translation, rotationMX, rotationMY, rotationMZ, scale, result;
    void CountOperations();
    QImage im, texture;
    const float ToRad = M_PI/180;
    const float d = 500;
    QVector3D lightSource;

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
signals:

public slots:
    void transformX(int x)
    {
        translation(0, 3) = x + im.width()/2;
        paintCube();
    }
    void transformY(int y)
    {
        translation(1, 3) = y + im.height()/2;
        paintCube();
    }
    void transformZ(int z)
    {
        translation(2, 3) = z;
        paintCube();
    }
    void rotationX(int x)
    {
        rotationMX.setToIdentity();
        rotationMX.rotate(x, 1, 0, 0);
        paintCube();
    }
    void rotationY(int x)
    {
        rotationMY.setToIdentity();
        rotationMY.rotate(x, 0, 1, 0);
        paintCube();
    }
    void rotationZ(int x)
    {
        rotationMZ.setToIdentity();
        rotationMZ.rotate(x, 0, 0, 1);
        paintCube();
    }
    void scaleX(int x)
    {
        scale(0,0) = x/100.;
        paintCube();
    }
    void scaleY(int y)
    {
        scale(1,1) = y/100.;
        paintCube();
    }
    void scaleZ(int z)
    {
        scale(2,2) = z/100.;
        paintCube();
    }

};

#endif // EKRAN_H
