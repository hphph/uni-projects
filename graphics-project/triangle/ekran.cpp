#include "ekran.h"
#include <QPainter>
#include <cmath>
#include <QVector2D>

Ekran::Ekran(QWidget *parent) : QWidget{parent}
{
    im = QImage(350, 350, QImage::Format_RGB32);
    prevIm = QImage(350, 350, QImage::Format_RGB32);
    currentTriangle = Triangle(Vector2Int(0,0), Vector2Int(0,0), Vector2Int(0,0));
    textureTrianglePoints = nullptr;
    isTexturePointsLoaded = nullptr;
    isTriangleLoaded = false;
    isSecImgLoaded = false;
    prevIm.fill(0);
    im.fill(0);
    trianglePointsPressed = 0;
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

void Ekran::mousePressEvent(QMouseEvent *e)
{
    if(trianglePointsPressed <= 3)
    {
        trianglePoints[trianglePointsPressed] = Vector2Int(e->pos().x(), e->pos().y());
        trianglePointsPressed++;
    }
    if(trianglePointsPressed >= 3)
    {
        im = prevIm;
        DrawTriangle(trianglePoints[0], trianglePoints[1], trianglePoints[2]);
        if(isSecImgLoaded)
        {
            textureTrianglePoints->a = trianglePoints[0];
            textureTrianglePoints->b = trianglePoints[1];
            textureTrianglePoints->c = trianglePoints[2];
            *isTexturePointsLoaded = true;
        }
        else
        {
            isTriangleLoaded = true;
        }
        update();
        trianglePointsPressed = 0;
    }
}

void Ekran::DrawPixel(Vector2Int pos, RGBColor color)
{
    DrawPixel(pos.x, pos.y, color.red, color.green, color.blue);
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

void Ekran::DrawLine(Vector2Int a, Vector2Int b, RGBColor color)
{
    DrawLine(a.x, a.y, b.x, b.y, color.red, color.green, color.blue);
}

void Ekran::DrawTriangle(Vector2Int a, Vector2Int b, Vector2Int c)
{
    DrawLine(a, b, RGBColor(255, 255, 255));
    DrawLine(b, c, RGBColor(255, 255, 255));
    DrawLine(c, a, RGBColor(255, 255, 255));
    if(!isSecImgLoaded && *isTexturePointsLoaded) DrawTriangle(Triangle(a,b,c), *textureTrianglePoints, *secIm);
    else if(*isTexturePointsLoaded)
    {
        output->DrawTriangle(Triangle(output->trianglePoints[0], output->trianglePoints[1], output->trianglePoints[2]), *textureTrianglePoints, *secIm);
        output->update();
    }
    update();
}

void Ekran::DrawTriangle(Triangle pos, Triangle uvPos, QImage img)
{
    Vector2Int minVec = pos.MinVector();
    Vector2Int maxVec = pos.MaxVector();
    Vector2Int points[3] = {pos.a, pos.b, pos.c};

    for(int y = minVec.y; y <= maxVec.y; y++)
    {
        std::vector<int> crossingPoints;
        for(int i = 0; i < 3; i++)
        {
            int modiplus = (i+1)%3;
            int modiminus = i == 0 ? 2 : i-1;
            if((points[i].y < y && points[modiplus].y > y) || (points[i].y > y && points[modiplus].y < y) || points[i].y == y)
            {
                int x1 = points[i].x;
                int x2 = points[modiplus].x;
                int y1 = points[i].y;
                int y2 = points[modiplus].y;
                float xdelta = x2 - x1;
                float ydelta = y2 - y1;
                float A = 1;
                if(xdelta != 0) A = ydelta/xdelta;
                if(points[i].y == y && ((points[modiplus].y < y && points[modiminus].y < y) || (points[modiplus].y > y && points[modiminus].y > y)))
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
                    BPoint pixelPos(Vector2Int(x,y), pos);
                    QVector2D uvPixelPos = pixelPos.ToPoint(uvPos);
                    //RGBColor colorToDraw(0,0,0);
                    //colorToDraw = ((GetColor(floorx, floory, img) * (1-a) + GetColor(floorx+1, floory, img)*a)*b)
                    //         +((GetColor(floorx, floory+1, img)*(1-a) + GetColor(floorx+1, floory+1, img) * a) * (1-b));


                    //DrawPixel(Vector2Int(x,y), colorToDraw);
                    DrawPixel(Vector2Int(x,y), GetColor(uvPixelPos, img));
                }
            }
        }
    }
}


Ekran::RGBColor Ekran::GetColor(int x, int y, QImage &im)
{
    if(x < 0 || y < 0 || x >= im.width() || y >= im.height()) return RGBColor(0, 0, 0);
    uchar *pix = im.scanLine(y) + 4*x;
    return RGBColor(pix[2], pix[1], pix[0]);
}

Ekran::RGBColor Ekran::GetColor(QVector2D pos, QImage &im)
{
    if(pos.x() < 0 || pos.y() < 0 || pos.x() >= im.width() || pos.y() >= im.height()) return RGBColor(0, 0, 0);
    uchar *pix = im.scanLine((int)pos.y()) + 4*(int)pos.x();
    return RGBColor(pix[2], pix[1], pix[0]);
}

bool Ekran::LoadImage(QString path)
{
    isSecImgLoaded = secIm->load(path);
    if(secIm->width() > 350)
        *secIm = secIm->scaled(350, 350, Qt::KeepAspectRatio);
    update();
    im.load(path);
    prevIm.load(path);
    return isSecImgLoaded;
}

void Ekran::LoadSecImage(QImage *img)
{
    secIm = img;
}


