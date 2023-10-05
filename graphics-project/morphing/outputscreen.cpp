#include "outputscreen.h"
#include <windows.h>
#include <QTime>
#include <QCoreApplication>
#include <QEventLoop>

OutputScreen::OutputScreen(QWidget *parent)
    : QWidget{parent}
{
    outputImg = QImage(300, 300, QImage::Format_RGB32);
    outputImg.fill(0);
    update();
}

void OutputScreen::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawImage(0, 0, outputImg);
}

QColor OutputScreen::getColor(int x, int y, QImage &img)
{
    if(x < 0 || y < 0 || x >= img.width() || y >= img.height()) return QColor(255,255,255);
    uchar *pix = img.scanLine(y) + 4*x;
    return QColor(pix[2], pix[1], pix[0]);
}

void OutputScreen::DrawPixel(int x, int y, QColor color)
{
    if(x < 0 || y < 0 || x >= outputImg.width() || y >= outputImg.height()) return;
    uchar *pix = outputImg.scanLine(y) + 4*x;
    pix[0] = color.blue();
    pix[1] = color.green();
    pix[2] = color.red();
}

void OutputScreen::DrawPixel(int x, int y, QColor color, QImage &img)
{
    if(x < 0 || y < 0 || x >= img.width() || y >= img.height()) return;
    uchar *pix = img.scanLine(y) + 4*x;
    pix[0] = color.blue();
    pix[1] = color.green();
    pix[2] = color.red();
}

QColor OutputScreen::getColorBilinear(float x, float y, QImage &img)
{
    RGBColor colorToDraw(0,0,0);
    int floorx = floor(x);
    int floory = floor(y);
    float a = x - floorx;
    float b = floory + 1 - y;
    colorToDraw = ((RGBColor(getColor(floorx, floory, img)) * (1-a) + RGBColor(getColor(floorx+1, floory, img))*a)*b) +
                  ((RGBColor(getColor(floorx, floory+1, img))*(1-a) + RGBColor(getColor(floorx+1, floory+1, img)) * a) * (1-b));
    return colorToDraw.toQColor();
}

void OutputScreen::DrawTriangle(QVector2D pos[3], QVector2D uvPos[3], QImage &img)
{
    QVector2D minVec = MinVector(pos);
    QVector2D maxVec = MaxVector(pos);

    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            if(pos[i].x() == pos[j].x() && i != j)
            {
                pos[i].setX(pos[i].x()+1);
            }
            if(pos[i].y() == pos[j].y() && i != j)
            {
                pos[i].setY(pos[i].y()+1);
            }
        }
    }

    for(int y = minVec.y(); y <= maxVec.y(); y++)
    {
        std::vector<int> crossingPoints;
        for(int i = 0; i < 3; i++)
        {
            int modiplus = (i+1)%3;
            int modiminus = i == 0 ? 2 : i-1;
            if((pos[i].y() < y && pos[modiplus].y() > y) || (pos[i].y() > y && pos[modiplus].y() < y) || pos[i].y() == y)
            {
                float x1 = pos[i].x();
                float x2 = pos[modiplus].x();
                float y1 = pos[i].y();
                float y2 = pos[modiplus].y();
                float xdelta = x2 - x1;
                float ydelta = y2 - y1;
                float A = 1;
                if(xdelta != 0) A = ydelta/xdelta;
                if(pos[i].y() == y && ((pos[modiplus].y() < y && pos[modiminus].y() < y) || (pos[modiplus].y() > y && pos[modiminus].y() > y)))
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
                    BPoint pixelPos(QVector2D(x,y), pos);
                    QVector2D uvPixelPos = pixelPos.ToPoint(uvPos);
                    DrawPixel(x, y, getColorBilinear(uvPixelPos.x(), uvPixelPos.y(), img));
                }
            }
        }
    }
}

QImage OutputScreen::lerpImg(QImage a, QImage b, float t)
{
    QImage img = QImage(a.width(), a.height(), QImage::Format_RGB32);
    b = b.scaled(a.width(), a.height());
    for(int y = 0; y < a.height(); y++)
    {
        for(int x = 0; x < a.width(); x++)
        {
            QColor colorA = getColor(x,y, a);
            QColor colorB = getColor(x,y, b);
            QColor lerped;
            lerped.setRed((1 - t) * colorA.red() + t * colorB.red());
            lerped.setGreen((1 - t) * colorA.green() + t * colorB.green());
            lerped.setBlue((1 - t) * colorA.blue() + t * colorB.blue());
            DrawPixel(x,y, lerped, img);
        }
    }
    return img;
}

void OutputScreen::Morph(QVector2D firstTriangle[3], QImage firstImg, QVector2D finishTriangle[3], QImage finishImg)
{
    for(int i = 0; i <= 100; i++)
    {
        outputImg.fill(0);
        QVector2D lerpedTriangle[3] = {lerp(firstTriangle[0], finishTriangle[0], i/100.), lerp(firstTriangle[1], finishTriangle[1], i/100.), lerp(firstTriangle[2], finishTriangle[2], i/100.)};
        QImage lerpedImg = lerpImg(firstImg, finishImg, i/100.);
        DrawTriangle(lerpedTriangle, firstTriangle, lerpedImg);

        update();
        delay();
    }
}

void OutputScreen::delay()
{
    QTime dieTime= QTime::currentTime().addMSecs(100);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
