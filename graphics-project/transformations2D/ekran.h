#ifndef EKRAN_H
#define EKRAN_H

#include <QWidget>
#include <QImage>
#include <QPaintEvent>
#include <QColor>
#include <QVector2D>
#include <QVector3D>
#include <QTimer>

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

    struct Vector2Int
    {
        int x, y;
        Vector2Int(int x, int y): x(x), y(y) {}
        Vector2Int(): x(0), y(0) {}
        Vector2Int operator+(Vector2Int b) { return Vector2Int(x + b.x, y + b.y); }
        Vector2Int operator-(Vector2Int b) { return Vector2Int(x - b.x, y - b.y); }
    };
    struct Matrix3x3
    {
        float value[3][3] = {{0.0, 0.0, 0.0},
                             {0.0, 0.0, 0.0},
                             {0.0, 0.0, 0.0}};

        Matrix3x3 operator*(Matrix3x3 m)
        {
            Matrix3x3 result;
            for(int i = 0; i < 3; i++)
            {
                for(int j = 0; j < 3; j++)
                {
                    result.value[i][j] = value[i][0]*m.value[0][j] + value[i][1]*m.value[1][j] + value[i][2]*m.value[2][j];
                }
            }
            return result;
        }

        QVector2D operator*(QVector3D v)
        {
            QVector2D result;
            result.setX(value[0][0]*v.x() + value[0][1]*v.y() + value[0][2]*v.z());
            result.setY(value[1][0]*v.x() + value[1][1]*v.y() + value[1][2]*v.z());
            return result;
        }

        float det()
        {
            return value[0][0]*value[1][1]*value[2][2] + value[1][0]*value[2][1]*value[0][2] + value[2][0]*value[0][1]*value[1][2]
                   - value[1][0]*value[0][1]*value[2][2] - value[0][0]*value[2][1]*value[1][2] - value[2][0]*value[1][1]*value[0][2];
        }

        float det2x2(float a11, float a12,float a21,float a22) { return a11*a22 - a12*a21; }
    };
    void drawPixel(int x, int y, QColor color, QImage &img);
    void drawPixel(Vector2Int v, QColor color, QImage &img) { drawPixel(v.x, v.y, color, img); }
    QImage paintOperations(Matrix3x3 mat, QImage &img);
    QColor getColor(int x, int y, QImage &img);
    QColor getColorBilinear(float x, float y, QImage &img);
    QColor getColor(Vector2Int v, QImage &img) { return getColor(v.x, v.y, img); }

    Matrix3x3 translation(Vector2Int t);
    Matrix3x3 rotation(float angle);
    Matrix3x3 scale(QVector2D s);
    Matrix3x3 invTranslation(Vector2Int t);
    Matrix3x3 invRotation(float angle);
    Matrix3x3 invScale(QVector2D s);
    Matrix3x3 invShearingX(float x);
    Matrix3x3 invShearingY(float y);
protected:
    void paintEvent(QPaintEvent *);
private:
    QImage currentImage;
    QImage firstImage;
    Matrix3x3 operations;
    Matrix3x3 translations;
    Matrix3x3 rotations;
    Matrix3x3 scales;
    Matrix3x3 invOperations;
    Matrix3x3 invTranslations;
    Matrix3x3 invRotations;
    Matrix3x3 invScales;
    Matrix3x3 invShearingsX;
    Matrix3x3 invShearingsY;
    void countOperationsMatrix()
    {
        //operations = translations*rotations*scales;
        invOperations = invShearingsX*invShearingsY*invScales*invRotations*invTranslations;
        currentImage = paintOperations(operations, firstImage);
        update();
    }
public slots:
    void translationX(int x);
    void translationY(int y);
    void rotationA(int angle);
    void scaleX(int x);
    void scaleY(int y);
    void shearingX(int x);
    void shearingY(int y);
signals:
};

#endif // EKRAN_H
