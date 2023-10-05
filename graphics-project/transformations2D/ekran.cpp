#include "ekran.h"
#include <QColor>
#include <QPainter>
#include <cmath>

#define PI 3.141592653589793238
#define DEG2RAD PI/180

Ekran::Ekran(QWidget *parent)
    : QWidget{parent}
{
    currentImage = QImage(500, 500, QImage::Format_RGB32);
    currentImage.fill(QColor("white"));
    firstImage.load("C:\\Studia\\PGK\\Projects\\Operacje2D\\img.png");
    //translations = translation(Vector2Int(100,100));
    //rotations = rotation(0);
    //scales = scale(QVector2D(1,1));
    invTranslations = invTranslation(Vector2Int(100,100));
    invRotations = invRotation(0);
    invScales = invScale(QVector2D(1,1));
    invShearingsX = invShearingX(0);
    invShearingsY = invShearingY(0);
    countOperationsMatrix();
}

void Ekran::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawImage(0, 0, currentImage);
}

QImage Ekran::paintOperations(Matrix3x3 mat, QImage &img)
{
    QImage result = currentImage;
    result.fill("white");
    for(int y = 0; y < currentImage.height(); y++)
    {
        for(int x = 0; x < currentImage.width(); x++)
        {
            QVector3D v(x, y, 1);
            QVector2D newV = invOperations*v;
            drawPixel(x, y, getColorBilinear(newV.x(), newV.y(), img), result);
        }
    }
    return result;
}

Ekran::Matrix3x3 Ekran::translation(Vector2Int t)
{
    Matrix3x3 result;
    for(int i = 0; i<3; i++)
    {
        result.value[i][i] = 1;
    }
    result.value[0][2] = t.x;
    result.value[1][2] = t.y;
    return result;
}

Ekran::Matrix3x3 Ekran::invTranslation(Vector2Int t)
{
    Matrix3x3 result;
    for(int i = 0; i<3; i++)
    {
        result.value[i][i] = 1;
    }
    result.value[0][2] = -t.x;
    result.value[1][2] = -t.y;
    return result;
}

Ekran::Matrix3x3 Ekran::rotation(float angle)
{
    Matrix3x3 result;
    float cosAngle = cos(angle * DEG2RAD);
    float sinAngle = sin(angle * DEG2RAD);
    result.value[2][2] = 1;
    result.value[0][0] = cosAngle;
    result.value[1][1] = cosAngle;
    result.value[0][1] = -sinAngle;
    result.value[1][0] = sinAngle;
    result.value[0][2] = (firstImage.width()/2)*(1 - cosAngle) + (firstImage.height()/2)*sinAngle;
    result.value[1][2] = (firstImage.height()/2)*(1 - cosAngle) - (firstImage.width()/2)*sinAngle;
    return result;
}

Ekran::Matrix3x3 Ekran::invRotation(float angle)
{
    Matrix3x3 result;
    float cosAngle = cos(angle * DEG2RAD);
    float sinAngle = sin(angle * DEG2RAD);
    float A = (firstImage.width()/2)*(1 - cosAngle) + (firstImage.height()/2)*sinAngle;
    float B = (firstImage.height()/2)*(1 - cosAngle) - (firstImage.width()/2)*sinAngle;
    result.value[2][2] = 1;
    result.value[0][0] = cosAngle;
    result.value[1][1] = cosAngle;
    result.value[0][1] = sinAngle;
    result.value[1][0] = -sinAngle;
    result.value[0][2] = -sinAngle*B -cosAngle*A;
    result.value[1][2] = -cosAngle*B + sinAngle*A;
    return result;
}

Ekran::Matrix3x3 Ekran::scale(QVector2D s)
{
    QVector2D translation(firstImage.width()/2, firstImage.height()/2);
    Matrix3x3 result;
    result.value[0][0] = s.x();
    result.value[1][1] = s.y();
    result.value[0][2] = translation.x() - (s.x()*translation.x());
    result.value[1][2] = translation.y() - (s.y()*translation.y());
    result.value[2][2] = 1;
    return result;
}

Ekran::Matrix3x3 Ekran::invScale(QVector2D s)
{
    QVector2D translation(firstImage.width()/2, firstImage.height()/2);
    Matrix3x3 result;
    result.value[0][0] = 1/s.x();
    result.value[1][1] = 1/s.y();
    result.value[0][2] = translation.x() - (translation.x()/s.x());
    result.value[1][2] = translation.y() - (translation.y()/s.y());
    result.value[2][2] = 1;
    return result;
}

Ekran::Matrix3x3 Ekran::invShearingX(float x)
{
    Matrix3x3 result;
    result.value[0][0] = 1;
    result.value[1][1] = 1;
    result.value[2][2] = 1;
    result.value[0][1] = -x;
    return result;
}

Ekran::Matrix3x3 Ekran::invShearingY(float y)
{
    Matrix3x3 result;
    result.value[0][0] = 1;
    result.value[1][1] = 1;
    result.value[2][2] = 1;
    result.value[1][0] = -y;
    return result;
}

void Ekran::drawPixel(int x, int y, QColor color, QImage &img)
{
    if(x < 0 || y < 0 || x >= img.width() || y >= img.height()) return;
    uchar *pix = img.scanLine(y) + 4*x;
    pix[0] = color.blue();
    pix[1] = color.green();
    pix[2] = color.red();
}

QColor Ekran::getColor(int x, int y, QImage &img)
{
    if(x < 0 || y < 0 || x >= img.width() || y >= img.height()) return QColor(255,255,255);
    uchar *pix = img.scanLine(y) + 4*x;
    return QColor(pix[2], pix[1], pix[0]);
}

QColor Ekran::getColorBilinear(float x, float y, QImage &img)
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

void Ekran::translationX(int x)
{
    //translations = translation(Vector2Int(x, translations.value[1][2]));
    invTranslations = invTranslation(Vector2Int(x, -invTranslations.value[1][2]));
    countOperationsMatrix();
}

void Ekran::translationY(int y)
{
    //translations = translation(Vector2Int(translations.value[0][2], y));
    invTranslations = invTranslation(Vector2Int(-invTranslations.value[0][2], y));
    countOperationsMatrix();
}

void Ekran::scaleX(int x)
{
    float xTenth = x*0.1;
    //scales = scale(QVector2D(xTenth, scales.value[1][1]));
    invScales = invScale(QVector2D(xTenth, 1/invScales.value[1][1]));
    countOperationsMatrix();
}

void Ekran::scaleY(int y)
{
    float yTenth = y*0.1;
    //scales = scale(QVector2D(scales.value[0][0], yTenth));
    invScales = invScale(QVector2D(1/invScales.value[0][0], yTenth));
    countOperationsMatrix();
}

void Ekran::rotationA(int angle)
{
    //rotations = rotation(angle);
    invRotations = invRotation(angle);
    countOperationsMatrix();
}

void Ekran::shearingX(int x)
{
    invShearingsX = invShearingX((float)x*0.01);
    countOperationsMatrix();
}

void Ekran::shearingY(int y)
{
    invShearingsY = invShearingY((float)y*0.01);
    countOperationsMatrix();
}
