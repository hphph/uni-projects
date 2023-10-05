#ifndef MATRIX4X4_H
#define MATRIX4X4_H


class Matrix4x4
{
public:
    Matrix4x4();
    static Matrix4x4 zero();
    static Matrix4x4 identity();
    Matrix4x4 operator*(Matrix4x4 rmat);
private:
    float value[4][4];
};

#endif // MATRIX4X4_H
