#include "ekran.h"

Ekran::Ekran(QWidget *parent)
    : QWidget{parent}
{
    currentImage = QImage(500, 500, QImage::Format_RGB32);
    lastImage = QImage(500, 500, QImage::Format_RGB32);
    globalScale = 16;
    currentImage.fill(QColor("white"));
    DrawGrid(globalScale);
    state = DRAW;
    lastPosition = Vector2D(0,0);
}

void Ekran::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawImage(0, 0, currentImage);
}

void Ekran::DrawPixel(int x, int y, bool isBlack, QImage &img)
{
    if(x < 0 || y < 0 || x >= img.width() || y >= img.height()) return;
    uchar *pix = img.scanLine(y) + 4*x;
    if(isBlack)
    {
        pix[0] = 0;
        pix[1] = 0;
        pix[2] = 0;
    }
    else
    {
        pix[0] = 255;
        pix[1] = 255;
        pix[2] = 255;
    }
}

void Ekran::DrawPixel(int x, int y, bool isBlack, int scale, QImage &img)
{
    for(int i = y; i < y+scale; i++)
    {
        for(int j = x; j < x+scale; j++)
        {
            DrawPixel(j, i, isBlack, img);
        }
    }
}

void Ekran::DrawPixelInGrid(int x, int y, bool isBlack, int scale, QImage &img)
{
    DrawPixel(scale*x + x, scale*y + y, isBlack, scale, img);
}

bool Ekran::GetColor(int x, int y, QImage &img)
{
    if(x < 0 || y < 0 || x >= img.width() || y >= img.height()) return false;
    uchar *pix = img.scanLine(y) + 4*x;
    if(pix[0] == 0) return true;
    return false;
}

void Ekran::DrawGrid(int scale)
{
    for(int i = scale; i < currentImage.height(); i += scale+1)
    {
        for(int j = 0; j < currentImage.width(); j++)
        {
            DrawPixel(i, j, true, currentImage);
            DrawPixel(j, i, true, currentImage);
        }
    }
    update();
}

void Ekran::Dilation(int x, int y, int scale, QImage &img)
{
    if(x < 0 || y < 0 || x >= img.width() || y >= img.height()) return;
    for(int i = y-1; i<=y+1; i++)
    {
        for(int j = x-1; j<=x+1; j++)
        {
            if(i != y || j != x)
            {
                if(GetColor(scale*j + j, scale*i + i, img))
                {
                    DrawPixelInGrid(x, y, true, scale, currentImage);
                    return;
                }
            }
        }
    }
}

void Ekran::DilationOnButtonPress()
{
    lastImage = currentImage;
    for(int i = 0; i < 500; i++)
    {
        for(int j = 0; j < 500; j++)
        {
            Dilation(j, i, globalScale, lastImage);
        }
    }
    update();
}

void Ekran::ErosionOnButtonPress()
{
    lastImage = currentImage;
    for(int i = 0; i < 500; i++)
    {
        for(int j = 0; j < 500; j++)
        {
            Erosion(j, i, globalScale, lastImage);
        }
    }
    update();
}

void Ekran::OpeningOnButtonPress()
{
    ErosionOnButtonPress();
    DilationOnButtonPress();
}

void Ekran::ClosingOnButtonPress()
{
    DilationOnButtonPress();
    ErosionOnButtonPress();
}

void Ekran::Erosion(int x, int y, int scale, QImage &img)
{
    if(x < 0 || y < 0 || x >= img.width() || y >= img.height()) return;
    for(int i = y-1; i<=y+1; i++)
    {
        for(int j = x-1; j<=x+1; j++)
        {
            if(i != y || j != x)
            {
                if(!GetColor(scale*j + j, scale*i + i, img))
                {
                    DrawPixelInGrid(x, y, false, scale, currentImage);
                    return;
                }
            }
        }
    }
}

void Ekran::mousePressEvent(QMouseEvent *e)
{
    lastImage = currentImage;
    int xScaled = gridPos(e->pos().x(), globalScale);
    int yScaled = gridPos(e->pos().y(), globalScale);

    switch(state)
    {
    case DRAW:
        if(!(lastPosition == Vector2D(xScaled, yScaled)))
            DrawPixelInGrid(xScaled, yScaled, true, globalScale, currentImage);
        update();
        break;
    case EROSION:
        Erosion(xScaled, yScaled, globalScale, currentImage);
        update();
        break;
    case DILATION:
        Dilation(xScaled, yScaled, globalScale, currentImage);
        update();
        break;
    default:
        state = DRAW;
        break;
    }
    lastPosition = Vector2D(xScaled , yScaled);
}

void Ekran::mouseMoveEvent(QMouseEvent *e)
{
    int xScaled = gridPos(e->pos().x(), globalScale);
    int yScaled = gridPos(e->pos().y(), globalScale);

    switch(state)
    {
    case DRAW:
        if(!(lastPosition == Vector2D(xScaled, yScaled)))
            DrawPixelInGrid(xScaled, yScaled, true, globalScale, currentImage);
        update();
        break;
    case EROSION:
        Erosion(xScaled, yScaled, globalScale, currentImage);
        update();
        break;
    case DILATION:
        Dilation(xScaled, yScaled, globalScale, currentImage);
        update();
        break;
    default:
        state = DRAW;
        break;
    }
    lastPosition = Vector2D(xScaled , yScaled);
}

void Ekran::SetButton(QPushButton *button, DrawingState newState)
{
    connect(button, &QAbstractButton::pressed, this, [=](){ state = newState; });
}
