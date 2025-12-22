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

/** Multiplies two 4x4 matrices. */
Matrix4 Matrix4fMul(Matrix4 a, Matrix4 b);

/** Multiplies a 4D vector by a 4x4 matrix. */
Vector4 Matrix4MulVector4(Vector4 v, Matrix4 m);

/** Converts world coordinates to screen coordinates.
 * @param pos The 3D world position.
 * @param screenPos Pointer to store the resulting 2D screen position.
 * @param view The view matrix.
 * @param projection The projection matrix.
 * @param screenWidth The width of the screen.
 * @param screenHeight The height of the screen.
 * @returns 1 if the position is on screen, 0 if it's behind the camera
 */
int WorldToScreen(Vector3 pos, Vector2 *screenPos, Matrix4 view, Matrix4 projection, int screenWidth, int screenHeight);

#endif