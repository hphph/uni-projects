#ifndef EKRAN_H
#define EKRAN_H


#include <QWidget>
#include <QSlider>
#include <QImage>
#include <QMouseEvent>
#include <QString>


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
    };

    struct Vector2Int
    {
        int x, y;
        Vector2Int(int x, int y): x(x), y(y) {};
        Vector2Int(): x(0), y(0) {};
        bool operator==(Vector2Int v)
        {
            if(x != v.x || y != v.y) return false;
            return true;
        }
        bool operator!=(Vector2Int v)
        {
            return !(*(this) == v);
        }
        operator QVector2D()
        {
            return QVector2D(x, y);
        }
    };

    struct Triangle
    {
        Vector2Int a;
        Vector2Int b;
        Vector2Int c;
        Triangle() {};
        Triangle(Vector2Int a, Vector2Int b, Vector2Int c): a(a), b(b), c(c) {};
        Triangle(int ax, int ay, int bx, int by, int cx, int cy)
        {
            a = Vector2Int(ax, ay);
            b = Vector2Int(bx, by);
            c = Vector2Int(cx, cy);
        }
        Vector2Int MinVector()
        {
            Vector2Int minVec = a;
            if(b.x < minVec.x) minVec.x = b.x;
            if(c.x < minVec.x) minVec.x = c.x;
            if(b.y < minVec.y) minVec.y = b.y;
            if(c.y < minVec.y) minVec.y = c.y;
            return minVec;
        }

        Vector2Int MaxVector()
        {
            Vector2Int maxVec = a;
            if(b.x > maxVec.x) maxVec.x = b.x;
            if(c.x > maxVec.x) maxVec.x = c.x;
            if(b.y > maxVec.y) maxVec.y = b.y;
            if(c.y > maxVec.y) maxVec.y = c.y;
            return maxVec;
        }
    };

    struct BPoint
    {
        float x, y, z;
        BPoint(Vector2Int point, Triangle triangle)
        {
            int W = (triangle.a.x-triangle.c.x)*(triangle.b.y-triangle.c.y) - (triangle.a.y-triangle.c.y)*(triangle.b.x-triangle.c.x);
            int Wx = (point.x-triangle.c.x)*(triangle.b.y-triangle.c.y) - (point.y-triangle.c.y)*(triangle.b.x-triangle.c.x);
            int Wy = (triangle.a.x-triangle.c.x)*(point.y-triangle.c.y) - (triangle.a.y-triangle.c.y)*(point.x-triangle.c.x);
            x = Wx/(float)W;
            y = Wy/(float)W;
            z = 1 - x - y;
        }

        QVector2D ToPoint(Triangle triangle)
        {
            return x*(QVector2D)triangle.a + y*(QVector2D)triangle.b + z*(QVector2D)triangle.c;
        }
    };

    Triangle *textureTrianglePoints;
    QImage *secIm;
    QImage im, prevIm;
    bool *isTexturePointsLoaded;
    bool isTriangleLoaded;
    bool isSecImgLoaded;
    Ekran *output;

    void DrawPixel(int x, int y, uchar r, uchar g, uchar b);
    void DrawPixel(Vector2Int pos, RGBColor color);
    void DrawLine(int x1, int y1, int x2, int y2, uchar r, uchar g, uchar b);
    void DrawLine(Vector2Int a, Vector2Int b, RGBColor color);
    void DrawTriangle(Vector2Int a, Vector2Int b, Vector2Int c);
    void DrawTriangle(Triangle pos, Triangle uvPos, QImage img);

    RGBColor GetColor(int x, int y, QImage &im);
    RGBColor GetColor(QVector2D pos, QImage &im);
    bool LoadImage(QString path);
    void LoadSecImage(QImage *img);
    void TextureUpdate(Triangle *tri);
signals:

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *e);
private:
    Triangle currentTriangle;
    Vector2Int trianglePoints[3];
    int trianglePointsPressed;
};

#endif // EKRAN_H
