#include "matrix4x4.h"

Matrix4x4::Matrix4x4()
{
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            value[i][j] = 0;
        }
        value[i][i] = 1;
    }
}

Matrix4x4 Matrix4x4::zero()
{
    Matrix4x4 result;
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            result.value[i][j] = 0;
        }
    }
    return result;
}

Matrix4x4 Matrix4x4::identity()
{
    Matrix4x4 result;
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            result.value[i][j] = 0;
        }
        result.value[i][i] = 1;
    }
    return result;
}

Matrix4x4 Matrix4x4::operator*(Matrix4x4 rmat)
{
    Matrix4x4 result;
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            result.value[i][j] = value[i][0]*rmat.value[0][j] + value[i][1]*rmat.value[1][j] + value[i][2]*rmat.value[2][j] + value[i][3]*rmat.value[3][j];
        }
    }
    return result;
}
