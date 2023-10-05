#ifndef EKRAN_H
#define EKRAN_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QMouseEvent>
#include <QPushButton>

class Ekran : public QWidget
{
    Q_OBJECT
public:
    struct Vector2D
    {
        int x;
        int y;
        Vector2D(int x, int y) : x(x), y(y) {};
        Vector2D() {};
        bool operator==(Vector2D v)
        {
            if(x != v.x || y != v.y) return false;
            return true;
        }
    };
    enum DrawingState
    {
        DRAW,
        EROSION,
        DILATION
    };

    explicit Ekran(QWidget *parent = nullptr);
    void DrawPixel(int x, int y, bool isBlack, QImage &img);
    void DrawPixel(int x, int y, bool isBlack, int scale, QImage &img);
    void DrawPixelInGrid(int x, int y, bool isBlack, int scale, QImage &img);
    void DrawGrid(int scale);
    bool GetColor(int x, int y, QImage &img);
    int gridPos(int n, int scale) {return (n - n/scale)/scale; };
    void Dilation(int x, int y, int scale, QImage &img);
    void Erosion(int x, int y, int scale, QImage &img);

    void SetButton(QPushButton *button, DrawingState newState);
protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
private:
    int globalScale;
    Vector2D lastPosition;
    DrawingState state;
    QImage currentImage;
    QImage lastImage;
public slots:
    void DilationOnButtonPress();
    void ErosionOnButtonPress();
    void OpeningOnButtonPress();
    void ClosingOnButtonPress();

signals:

};

#endif // EKRAN_H
