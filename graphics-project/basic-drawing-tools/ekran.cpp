#include "ekran.h"
#include <QPainter>
#include <cmath>
#include <QVector2D>
#include <QColorDialog>
#include <stack>

Ekran::Ekran(QWidget *parent)
    : QWidget{parent}
{
    im = QImage(500, 500, QImage::Format_RGB32);
    lastImage = QImage(500, 500, QImage::Format_RGB32);
    im.fill(0);
    mousePressedTimes = 0;
    currentState = line;
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


Ekran::RGBColor Ekran::GetColor(int x, int y)
{
    if(x < 0 || y < 0 || x >= im.width() || y >= im.height()) return RGBColor(0, 0, 0);
    uchar *pix = im.scanLine(y) + 4*x;
    return RGBColor(pix[2], pix[1], pix[0]);
}

void Ekran::DrawLine(int x1, int y1, int x2, int y2, uchar r, uchar g, uchar b)
{
    float func = 0;
    float xdelta = x2 - x1;
    float ydelta = y2 - y1;
    float A = ydelta/xdelta;
    int xi, yi;
    int xt, yt;

    xi = x1 < x2 ? x1 : x2;
    yi = y1 < y2 ? y1 : y2;

    xt = x1 > x2 ? x1 : x2;
    yt = y1 > y2 ? y1 : y2;
    if(fabs(xdelta) >= fabs(ydelta))
    {
        for(int x = xi; x <= xt; x++)
        {
            func = A * (x - x1) + y1;
            DrawPixel(x, func, r, g, b);
        }
    }
    else if(xdelta == 0 && ydelta == 0)
    {
        DrawPixel(x1, y1, r, g, b);
    }
    else if(xdelta == 0)
    {
        for(int y = yi; y <= yt; y++)
        {
            DrawPixel(x1, y, r, g, b);
        }
    }
    else
    {
        for(int y = yi; y <= yt; y++)
        {
            func = (y + A * x1 - y1) / A;
            DrawPixel(func, y, r, g , b);
        }
    }
}

void Ekran::mousePressEvent(QMouseEvent *e)
{
    pressedMousePosX = e->pos().x();
    pressedMousePosY = e->pos().y();
    if(currentState == bezierPlus)
    {
        modifiedBezierPoint = nullptr;
        for(int i = 0; i < (int)bezierPoints.size(); i++)
        {
            if(bezierPoints[i].distanceToPoint(QVector2D(e->pos())) <= 10)
            {
                modifiedBezierPoint = &bezierPoints[i];
                break;
            }
        }
        if(modifiedBezierPoint == nullptr)
        {
            bezierPoints.push_back(QVector2D(pressedMousePosX, pressedMousePosY));
        }
        im = lastImage;
    }
    else if(currentState == bezierMinus)
    {
        for(int i = 0; i < (int)bezierPoints.size(); i++)
        {
            if(bezierPoints[i].distanceToPoint(QVector2D(e->pos())) <= 10)
            {
                bezierPoints.erase(bezierPoints.begin() + i);
            }
        }
        im = lastImage;
    }
    else if(currentState == bSplinePlus)
    {
        modifiedBSplinePoint = nullptr;
        for(int i = 0; i < (int)bSplinePoints.size(); i++)
        {
            if(bSplinePoints[i].distanceToPoint(QVector2D(e->pos())) <= 10)
            {
                modifiedBSplinePoint = &bSplinePoints[i];
                break;
            }
        }
        if(modifiedBSplinePoint == nullptr)
        {
            bSplinePoints.push_back(QVector2D(pressedMousePosX, pressedMousePosY));
        }

        im = lastImage;
    }
    else if(currentState == bSplineMinus)
    {
        for(int i = 0; i < (int)bSplinePoints.size(); i++)
        {
            if(bSplinePoints[i].distanceToPoint(QVector2D(e->pos())) <= 10)
            {
                bSplinePoints.erase(bSplinePoints.begin() + i);
            }
        }
        im = lastImage;
    }
    else if(currentState == floodFill)
    {
        FloodFill(pressedMousePosX, pressedMousePosY, currentColor);
        lastImage = im;
    }
    else if(currentState == scanLinePlus)
    {
        if(scanLinePoints.size() > 0)
            if(QVector2D(pressedMousePosX, pressedMousePosY).distanceToPoint(scanLinePoints[0]) <= 10)
            {
                currentState = scanLineDraw;
            }
        if(currentState == scanLinePlus)
        scanLinePoints.push_back(QVector2D(pressedMousePosX, pressedMousePosY));
        im = lastImage;
    }
    else if(currentState == scanLineDraw)
    {
        for(int i = 0; i < (int)scanLinePoints.size()-1; i++)
        {
            DrawLine(scanLinePoints[i].x(), scanLinePoints[i].y(), scanLinePoints[i+1].x(), scanLinePoints[i+1].y(), 255, 64, 128);
        }
        DrawLine(scanLinePoints[scanLinePoints.size()-1].x(), scanLinePoints[scanLinePoints.size()-1].y(), scanLinePoints[0].x(), scanLinePoints[0].y(), 255, 64, 128);
        ScanLine(scanLinePoints, RGBColor(0, 255, 0));
        lastImage = im;
    }
    else
    {
        lastImage = im;
    }
    for(int i = 0; i < (int)bezierPoints.size(); i++)
    {
        DrawCircle(bezierPoints[i].x(), bezierPoints[i].y(), 5, 255, 64, 0);
        if(i % 3 == 0 && i != 0)
        {
            DrawBezier(slider->value(), bezierPoints[i-3], bezierPoints[i-2], bezierPoints[i-1], bezierPoints[i], 0, 255, 128);
        }
    }
    for(int i = 0; i < (int)bSplinePoints.size(); i++)
    {
        DrawCircle(bSplinePoints[i].x(), bSplinePoints[i].y(), 5, 128, 64, 255);
        if(i > 2)
        {
            DrawBSpline(slider->value(), bSplinePoints[i-3], bSplinePoints[i-2], bSplinePoints[i-1], bSplinePoints[i], 64, 128, 128);
        }
    }
    for(int i = 0; i < (int)scanLinePoints.size(); i++)
    {
        DrawCircle(scanLinePoints[i].x(), scanLinePoints[i].y(), 5, 255, 64, 128);
    }
    update();
}

void Ekran::mouseMoveEvent(QMouseEvent *e)
{
    im = lastImage;
    switch(currentState)
    {
        case line:
            DrawLine(pressedMousePosX, pressedMousePosY, e->pos().x(), e->pos().y(), 0, 128, 255);
            break;
        case circle:
        {
            int mouseRadius = ceil(sqrt(pow(abs(e->pos().x() - pressedMousePosX),2) + pow(abs(e->pos().y() - pressedMousePosY),2)));
            DrawCircle(pressedMousePosX, pressedMousePosY, mouseRadius, 255, 0, 0);
            break;
        }
        case lineCircle:
        {
            int mouseRadius = ceil(sqrt(pow(abs(e->pos().x() - pressedMousePosX),2) + pow(abs(e->pos().y() - pressedMousePosY),2)));
            DrawLineCircle(slider->value(), pressedMousePosX, pressedMousePosY, mouseRadius, 128, 0, 255);
            break;
        }
        case elipse:
            DrawElipse(slider->value(), pressedMousePosX, pressedMousePosY, e->pos().x(), e->pos().y(), 128, 0, 255);
            break;
        case bSplinePlus:
            if(modifiedBSplinePoint != nullptr)
            {
                modifiedBSplinePoint->setX(e->pos().x());
                modifiedBSplinePoint->setY(e->pos().y());
            }
            break;
        case bezierPlus:
            if(modifiedBezierPoint != nullptr)
            {
                modifiedBezierPoint->setX(e->pos().x());
                modifiedBezierPoint->setY(e->pos().y());
            }
            break;
        default:
            break;
    }

    for(int i = 0; i < (int)bezierPoints.size(); i++)
    {
        DrawCircle(bezierPoints[i].x(), bezierPoints[i].y(), 5, 255, 64, 0);
        if(i % 3 == 0 && i != 0)
        {
            DrawBezier(slider->value(), bezierPoints[i-3], bezierPoints[i-2], bezierPoints[i-1], bezierPoints[i], 0, 255, 128);
        }
    }
    for(int i = 0; i < (int)bSplinePoints.size(); i++)
    {
        DrawCircle(bSplinePoints[i].x(), bSplinePoints[i].y(), 5, 128, 64, 255);
        if(i > 2)
        {
            DrawBSpline(slider->value(), bSplinePoints[i-3], bSplinePoints[i-2], bSplinePoints[i-1], bSplinePoints[i], 64, 128, 128);
        }
    }
    for(int i = 0; i < (int)scanLinePoints.size(); i++)
    {
        DrawCircle(scanLinePoints[i].x(), scanLinePoints[i].y(), 5, 255, 64, 128);
    }
    update();
}

void Ekran::DrawCircle(int x1, int y1, int radius, uchar r, uchar g, uchar b)
{
    float func;
    int rdivsqrt2 = ceil(radius/sqrt(2));

    for(int i = -rdivsqrt2; i <= rdivsqrt2; i++)
    {
        func = sqrt((radius * radius) - (i * i));
        DrawPixel(i+x1, func+y1, r, g, b);
        DrawPixel(i+x1, (-func)+y1, r, g, b);
    }
    for(int i = -rdivsqrt2; i <= rdivsqrt2; i++)
    {
        func = sqrt((radius * radius) - (i * i));
        DrawPixel(x1+func, i+y1, r, g, b);
        DrawPixel((-func)+x1, i+y1, r, g, b);
    }
}

void Ekran::DrawLineCircle(int N, int x1, int y1, int radius, uchar r, uchar g, uchar b)
{
    int xold = x1 + radius;
    int yold = y1;
    for(int i = 0; i <= N; i++)
    {
        float alpha = 2*M_PI*i/N;
        int x = radius * cos(alpha) + x1;
        int y = radius * sin(alpha) + y1;
        DrawLine(x, y, xold, yold, r, g, b);
        xold = x;
        yold = y;
    }
}

void Ekran::DrawElipse(int N, int x0, int y0, int x1, int y1, uchar r, uchar g, uchar b)
{
    int rx = (x1 - x0)/2;
    int ry = (y1 - y0)/2;
    int xold = x0 + 2*rx;
    int yold = y0 + ry;
    for(int i = 0; i <= N; i++)
    {
        float alpha = 2*M_PI*i/N;
        int x = rx * cos(alpha) + x0 + rx;
        int y = ry * sin(alpha) + y0 + ry;
        DrawLine(x, y, xold, yold, r, g, b);
        xold = x;
        yold = y;
    }
}

QVector2D Ekran::Bezier(float t, QVector2D p0, QVector2D p1, QVector2D p2, QVector2D p3)
{
    return pow(1 - t, 3) * p0 + 3 * pow(1 - t, 2)*t*p1 + 3*(1 - t)*t*t*p2 + t*t*t*p3;
}

QVector2D Ekran::BSpline(float t, QVector2D p0, QVector2D p1, QVector2D p2, QVector2D p3)
{
    return (((-pow(t, 3)) + (3*t*t) - (3*t) + 1)/6)*p0 + (((3*pow(t,3)) - (6*t*t) + 4)/6)*p1 + (((-3*pow(t,3)) + (3*t*t) + (3*t) + 1)/6)*p2 + (pow(t, 3)/6) * p3;
}

void Ekran::DrawBezier(int N, QVector2D p0, QVector2D p1, QVector2D p2, QVector2D p3, uchar r, uchar g, uchar b)
{
    QVector2D lastPoint = p0;
    QVector2D currentPoint;
    for(int i = 0; i <= N; i++)
    {
        float t = (float)i/N;
        currentPoint = Bezier(t, p0, p1, p2, p3);
        DrawLine(lastPoint.x(), lastPoint.y(), currentPoint.x(), currentPoint.y(), r, g, b);
        lastPoint = currentPoint;
    }
}

void Ekran::DrawBSpline(int N, QVector2D p0, QVector2D p1, QVector2D p2, QVector2D p3, uchar r, uchar g, uchar b)
{
    QVector2D lastPoint = BSpline(0, p0, p1, p2, p3);
    QVector2D currentPoint;
    for(int i = 1; i<= N; i++)
    {
        float t = (float)i/N;
        currentPoint = BSpline(t, p0, p1, p2, p3);
        DrawLine(lastPoint.x(), lastPoint.y(), currentPoint.x(), currentPoint.y(), r, g, b);
        lastPoint = currentPoint;
    }
}

void Ekran::ScanLine(std::vector<QVector2D> points, RGBColor color)
{
    QVector2D minVec(2048, 2048);
    QVector2D maxVec(-1, -1);
    for(int i = 0; i <= (int)points.size(); i++)
    {
        if(points[i].x() > maxVec.x()) maxVec.setX(points[i].x());
        if(points[i].x() < minVec.x()) minVec.setX(points[i].x());
        if(points[i].y() > maxVec.y()) maxVec.setY(points[i].y());
        if(points[i].y() < minVec.y()) minVec.setY(points[i].y());
    }

    for(int y = minVec.y(); y <= maxVec.y(); y++)
    {
        std::vector<int> crossingPoints;
        for(int i = 0; i < (int)points.size(); i++)
        {
            int modiplus = (i+1)%(int)points.size();
            int modiminus = i == 0 ? (int)points.size()-1 : i-1;
            if((points[i].y() < y && points[modiplus].y() > y) || (points[i].y() > y && points[modiplus].y() < y) || points[i].y() == y)
            {
                int x1 = points[i].x();
                int x2 = points[modiplus].x();
                int y1 = points[i].y();
                int y2 = points[modiplus].y();
                float xdelta = x2 - x1;
                float ydelta = y2 - y1;
                float A = ydelta/xdelta;
                if(points[i].y() == y && ((points[modiplus].y() < y && points[modiminus].y() < y) || (points[modiplus].y() > y && points[modiminus].y() > y)))
                {
                    crossingPoints.push_back((y-y1)/A + x1);
                }
                crossingPoints.push_back((y-y1)/A + x1);
            }
        }
        std::sort(crossingPoints.begin(), crossingPoints.end());
        for(int i = 0; i < (int)crossingPoints.size(); i++)
        {
            if(i%2 == 1)
            {
                for(int x = crossingPoints[i-1]; x<= crossingPoints[i]; x++)
                {
                    DrawPixel(x, y, color.red, color.green, color.blue);
                }
            }
        }
    }
}

void Ekran::FloodFill(int x, int y, RGBColor newColor)
{
    if(x < 0 || y < 0 || x >= im.width() || y >= im.height()) return;
    RGBColor oldColor = GetColor(x, y);
    if(oldColor == newColor) return;
    std::stack<QVector2D> toVisit;
    toVisit.push(QVector2D(x, y));
    while(!toVisit.empty())
    {
        QVector2D currentPixel = toVisit.top();
        toVisit.pop();
        if(currentPixel.x() < 0 || currentPixel.y() < 0 || currentPixel.x() >= im.width() || currentPixel.y() >= im.height()) continue;
        if(GetColor(currentPixel.x(), currentPixel.y()) == oldColor)
        {
            DrawPixel(currentPixel , newColor);
            toVisit.push(QVector2D(currentPixel.x() - 1, currentPixel.y()));
            toVisit.push(QVector2D(currentPixel.x() + 1, currentPixel.y()));
            toVisit.push(QVector2D(currentPixel.x(), currentPixel.y() - 1));
            toVisit.push(QVector2D(currentPixel.x(), currentPixel.y() + 1));
        }
    }
}

void Ekran::onButtonBezierPlusPressed()
{
    im = lastImage;
    if(currentState != bezierMinus && currentState != bezierPlus) bezierPoints.clear();
    if(currentState == bSplinePlus || currentState == bSplineMinus)
    {
        for(int i = 0; i < (int)bSplinePoints.size(); i++)
        {
            if(i > 2)
            {
                DrawBSpline(slider->value(), bSplinePoints[i-3], bSplinePoints[i-2], bSplinePoints[i-1], bSplinePoints[i], 64, 128, 128);
            }
        }
        update();
    }
    lastImage = im;
    bSplinePoints.clear();
    scanLinePoints.clear();
    currentState = bezierPlus;
}

void Ekran::onButtonBezierMinusPressed()
{
    im = lastImage;
    if(currentState != bezierMinus && currentState != bezierPlus) bezierPoints.clear();
    if(currentState == bSplinePlus || currentState == bSplineMinus)
    {
        for(int i = 0; i < (int)bSplinePoints.size(); i++)
        {
            if(i > 2)
            {
                DrawBSpline(slider->value(), bSplinePoints[i-3], bSplinePoints[i-2], bSplinePoints[i-1], bSplinePoints[i], 64, 128, 128);
            }
        }
        update();
    }
    lastImage = im;
    bSplinePoints.clear();
    scanLinePoints.clear();
    currentState = bezierMinus;
}

void Ekran::onButtonBSplinePlusPressed()
{
    im = lastImage;
    if(currentState != bSplineMinus && currentState != bSplinePlus) bSplinePoints.clear();
    if(currentState == bezierPlus || currentState == bezierMinus)
    {
        for(int i = 0; i < (int)bezierPoints.size(); i++)
        {
            if(i % 3 == 0 && i != 0)
            {
                DrawBezier(slider->value(), bezierPoints[i-3], bezierPoints[i-2], bezierPoints[i-1], bezierPoints[i], 0, 255, 128);
            }
        }
        update();
    }
    lastImage = im;
    bezierPoints.clear();
    scanLinePoints.clear();
    currentState = bSplinePlus;
}

void Ekran::onButtonBSplineMinusPressed()
{
    im = lastImage;
    if(currentState != bSplineMinus && currentState != bSplinePlus) bSplinePoints.clear();
    if(currentState == bezierPlus || currentState == bezierMinus)
    {
        for(int i = 0; i < (int)bezierPoints.size(); i++)
        {
            if(i % 3 == 0 && i != 0)
            {
                DrawBezier(slider->value(), bezierPoints[i-3], bezierPoints[i-2], bezierPoints[i-1], bezierPoints[i], 0, 255, 128);
            }
        }
        update();
    }
    lastImage = im;
    scanLinePoints.clear();
    bezierPoints.clear();
    currentState = bSplineMinus;
}

void Ekran::onButtonFloodFillPressed()
{
    im = lastImage;
    if(currentState == bezierPlus || currentState == bezierMinus)
    {
        for(int i = 0; i < (int)bezierPoints.size(); i++)
        {
            if(i % 3 == 0 && i != 0)
            {
                DrawBezier(slider->value(), bezierPoints[i-3], bezierPoints[i-2], bezierPoints[i-1], bezierPoints[i], 0, 255, 128);
            }
        }
        update();
    }
    if(currentState == bSplinePlus || currentState == bSplineMinus)
    {
        for(int i = 0; i < (int)bSplinePoints.size(); i++)
        {
            if(i > 2)
            {
                DrawBSpline(slider->value(), bSplinePoints[i-3], bSplinePoints[i-2], bSplinePoints[i-1], bSplinePoints[i], 64, 128, 128);
            }
        }
        update();
    }
    lastImage = im;
    bezierPoints.clear();
    bSplinePoints.clear();
    scanLinePoints.clear();
    currentColor = RGBColor(QColorDialog::getColor());
    currentState = floodFill;
}

void Ekran::onButtonScanLinePlusPressed()
{
    im = lastImage;
    if(currentState != scanLinePlus) scanLinePoints.clear();
    if(currentState == bezierPlus || currentState == bezierMinus)
    {
        for(int i = 0; i < (int)bezierPoints.size(); i++)
        {
            if(i % 3 == 0 && i != 0)
            {
                DrawBezier(slider->value(), bezierPoints[i-3], bezierPoints[i-2], bezierPoints[i-1], bezierPoints[i], 0, 255, 128);
            }
        }
        update();
    }
    if(currentState == bSplinePlus || currentState == bSplineMinus)
    {
        for(int i = 0; i < (int)bSplinePoints.size(); i++)
        {
            if(i > 2)
            {
                DrawBSpline(slider->value(), bSplinePoints[i-3], bSplinePoints[i-2], bSplinePoints[i-1], bSplinePoints[i], 64, 128, 128);
            }
        }
        update();
    }
    lastImage = im;
    bezierPoints.clear();
    bSplinePoints.clear();
    currentState = scanLinePlus;
}

void Ekran::LateUpdate()
{
    connect(buttonBezierMinus, &QAbstractButton::pressed, this, &Ekran::onButtonBezierMinusPressed);
    connect(buttonBezierPlus, &QAbstractButton::pressed, this, &Ekran::onButtonBezierPlusPressed);
    connect(buttonBSplineMinus, &QAbstractButton::pressed, this, &Ekran::onButtonBSplineMinusPressed);
    connect(buttonBSplinePlus, &QAbstractButton::pressed, this, &Ekran::onButtonBSplinePlusPressed);
    connect(buttonFloodFill, &QAbstractButton::pressed, this, &Ekran::onButtonFloodFillPressed);
    connect(buttonScanLinePlus, &QAbstractButton::pressed, this, &Ekran::onButtonScanLinePlusPressed);
}
