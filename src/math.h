#ifndef MATH_H
#define MATH_H

typedef struct
{
    float x, y;
} Vector2;

typedef struct
{
    float x, y, z;
} Vector3;

typedef struct
{
    float x, y, z, w;
} Vector4;

typedef struct
{
    float m00, m01, m02, m03;
    float m10, m11, m12, m13;
    float m20, m21, m22, m23;
    float m30, m31, m32, m33;
} Matrix4;

Matrix4 Matrix4fMul(Matrix4 a, Matrix4 b);
Vector4 Matrix4MulVector4(Vector4 v, Matrix4 m);
Vector2 WorldToScreen(Vector3 pos, char *visible, Matrix4 view, Matrix4 projection, int screenWidth, int screenHeight);

#endif