#include "ekran.h"

QVector3D cubeVertices[8] ={QVector3D(-100,100,-100),
                             QVector3D(100,100,-100),
                             QVector3D(100,-100,-100),
                             QVector3D(-100,-100,-100),
                             QVector3D(-100,-100,100),
                             QVector3D(100,-100,100),
                             QVector3D(100,100,100),
                             QVector3D(-100,100,100)};

QVector3D triangles[12][3] = {  {cubeVertices[1], cubeVertices[0], cubeVertices[3]},
                                {cubeVertices[3], cubeVertices[2], cubeVertices[1]},
                                {cubeVertices[7], cubeVertices[6], cubeVertices[5]},
                                {cubeVertices[5], cubeVertices[4], cubeVertices[7]},
                                {cubeVertices[6], cubeVertices[1], cubeVertices[2]},
                                {cubeVertices[2], cubeVertices[5], cubeVertices[6]},
                                {cubeVertices[0], cubeVertices[7], cubeVertices[4]},
                                {cubeVertices[4], cubeVertices[3], cubeVertices[0]},
                                {cubeVertices[0], cubeVertices[1], cubeVertices[6]},
                                {cubeVertices[6], cubeVertices[7], cubeVertices[0]},
                                {cubeVertices[4], cubeVertices[5], cubeVertices[2]},
                                {cubeVertices[2], cubeVertices[3], cubeVertices[4]},
                             };

QVector3D normals[12];

Ekran::Ekran(QWidget *parent)
    : QWidget{parent}
{
    texture.load("C:\\Studia\\PGK\\Projects\\Tranformacje3D\\img3.png");
    im = QImage(500, 500, QImage::Format_RGB32);
    im.fill(0);
    lightSource = QVector3D(500, 1000, 500);
    result = QMatrix4x4();
    translation = QMatrix4x4();
    rotationMX = QMatrix4x4();
    rotationMY = QMatrix4x4();
    rotationMZ = QMatrix4x4();
    scale = QMatrix4x4();
    translation.translate(im.width()/2, im.height()/2, 10);
    paintCube();
}

void Ekran::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawImage(0, 0, im);
}

void Ekran::DrawPixel(int x, int y, QColor color)
{
    if(x < 0 || y < 0 || x >= im.width() || y >= im.height()) return;
    uchar *pix = im.scanLine(y) + 4*x;
    pix[0] = color.blue();
    pix[1] = color.green();
    pix[2] = color.red();
}

void Ekran::DrawLine(int x1, int y1, int x2, int y2, QColor color)
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
            DrawPixel(x, func, color);
        }
    }
    else if(xdelta == 0 && ydelta == 0)
    {
        DrawPixel(x1, y1, color);
    }
    else if(xdelta == 0)
    {
        for(int y = yi; y <= yt; y++)
        {
            DrawPixel(x1, y, color);
        }
    }
    else
    {
        for(int y = yi; y <= yt; y++)
        {
            func = (y + A * x1 - y1) / A;
            DrawPixel(func, y, color);
        }
    }
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

void Ekran::DrawTriangle(QVector3D verts[3], QVector2D uvPos[3], QImage &img, QVector3D normal)
{
    QVector3D L = lightSource - verts[0];
    L.normalize();
    float cosb = QVector3D::dotProduct(L, normal);
    QVector3D R = (2*cosb*normal) - L;
    R.normalize();
    float cosa = QVector3D::dotProduct(normal, L);
    cosa = std::max((float)0, cosa);

    QVector2D tVerts[3];
    for(int i = 0; i < 3; i++)
    {
        QVector3D v = result.map(verts[i]);
        tVerts[i] = QVector2D(v.x()*d/(v.z()+d), v.y()*d/(v.z()+d));
    }

    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            if(tVerts[i].x() == tVerts[j].x() && i != j)
            {
                tVerts[i].setX(tVerts[i].x()+1);
            }
            if(tVerts[i].y() == tVerts[j].y() && i != j)
            {
                tVerts[i].setY(tVerts[i].y()+1);
            }
        }
    }

    QVector2D minVec = MinVector(tVerts);
    QVector2D maxVec = MaxVector(tVerts);
    for(int y = minVec.y(); y <= maxVec.y(); y++)
    {
        std::vector<int> crossingPoints;
        for(int i = 0; i < 3; i++)
        {
            int modiplus = (i+1)%3;
            int modiminus = i == 0 ? 2 : i-1;
            if((tVerts[i].y() < y && tVerts[modiplus].y() > y) || (tVerts[i].y() > y && tVerts[modiplus].y() < y) || tVerts[i].y() == y)
            {
                float x1 = tVerts[i].x();
                float x2 = tVerts[modiplus].x();
                float y1 = tVerts[i].y();
                float y2 = tVerts[modiplus].y();
                float xdelta = x2 - x1;
                float ydelta = y2 - y1;
                float A = 1;
                if(xdelta != 0) A = ydelta/xdelta;
                if(tVerts[i].y() == y && ((tVerts[modiplus].y() < y && tVerts[modiminus].y() < y) || (tVerts[modiplus].y() > y && tVerts[modiminus].y() > y)))
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
                    BPoint pixelPos(QVector2D(x,y), tVerts);
                    QVector2D uvPixelPos = pixelPos.ToPoint(uvPos);
                    QColor textureColor = getColorBilinear(uvPixelPos.x(), uvPixelPos.y(), img);
                    QVector3D cVec = QVector3D(textureColor.red()/255.0, textureColor.green()/255.0, textureColor.blue()/255.0);

                        cVec = cVec * cosa;
                        textureColor = QColor((int)(cVec.x() * 255.0), (int)(cVec.y() * 255.0), (int)(cVec.z() * 255.0));

                    DrawPixel(x, y, textureColor);
                }
            }
        }
    }
}

void Ekran::CountOperations()
{
    QMatrix4x4 rotation = rotationMZ * rotationMY * rotationMX;
    result = translation * rotation * scale;
}

void Ekran::paintCube()
{
    CountOperations();
    im.fill(0);
    QVector2D uvPos1[3] = {QVector2D(0,0), QVector2D(300,0), QVector2D(300, 300)};
    QVector2D uvPos2[3] = {QVector2D(300, 300), QVector2D(0,300), QVector2D(0, 0)};
    std::vector<std::pair<int, float>> sortedTrianglesPos;
    for(int i = 0; i<12; i++)
    {
        float avg = 0;
        QVector3D tVec[3];
        for(int j = 0; j < 3; j++)
        {
            tVec[j] = result.map(triangles[i][j]);
            avg += tVec[j].z();
        }
        avg /= 3;

        QVector3D A = tVec[1] - tVec[0];
        QVector3D B = tVec[2] - tVec[0];
        QVector3D normal = QVector3D::crossProduct(A, B);
        normal.normalize();
        normals[i] = normal;

        sortedTrianglesPos.push_back(std::make_pair(i, avg));
    }
    std::sort(sortedTrianglesPos.begin(), sortedTrianglesPos.end(), [](std::pair<int, float> a, std::pair<int, float> b)
                                  {
                                      return a.second > b.second;
                                  });


    for(std::pair<int, float> p : sortedTrianglesPos)
    {
        QVector3D vecT = result.map(triangles[p.first][0]);
        if(p.first%2 == 0)
        {
            if(QVector3D::dotProduct(vecT - QVector3D(0,0, -d), normals[p.first]) >= 0)
                DrawTriangle(triangles[p.first], uvPos2, texture, normals[p.first]);
        }
        else
        {
            if(QVector3D::dotProduct(vecT - QVector3D(0,0, -d), normals[p.first]) >= 0)
                DrawTriangle(triangles[p.first], uvPos1, texture, normals[p.first]);
        }
    }
    update();
}


