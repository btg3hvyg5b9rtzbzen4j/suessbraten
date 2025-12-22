#include "math.h"

Matrix4 Matrix4fMul(Matrix4 a, Matrix4 b) {
    Matrix4 r;

    r.m00 = a.m00 * b.m00 + a.m01 * b.m10 + a.m02 * b.m20 + a.m03 * b.m30;
    r.m01 = a.m00 * b.m01 + a.m01 * b.m11 + a.m02 * b.m21 + a.m03 * b.m31;
    r.m02 = a.m00 * b.m02 + a.m01 * b.m12 + a.m02 * b.m22 + a.m03 * b.m32;
    r.m03 = a.m00 * b.m03 + a.m01 * b.m13 + a.m02 * b.m23 + a.m03 * b.m33;

    r.m10 = a.m10 * b.m00 + a.m11 * b.m10 + a.m12 * b.m20 + a.m13 * b.m30;
    r.m11 = a.m10 * b.m01 + a.m11 * b.m11 + a.m12 * b.m21 + a.m13 * b.m31;
    r.m12 = a.m10 * b.m02 + a.m11 * b.m12 + a.m12 * b.m22 + a.m13 * b.m32;
    r.m13 = a.m10 * b.m03 + a.m11 * b.m13 + a.m12 * b.m23 + a.m13 * b.m33;

    r.m20 = a.m20 * b.m00 + a.m21 * b.m10 + a.m22 * b.m20 + a.m23 * b.m30;
    r.m21 = a.m20 * b.m01 + a.m21 * b.m11 + a.m22 * b.m21 + a.m23 * b.m31;
    r.m22 = a.m20 * b.m02 + a.m21 * b.m12 + a.m22 * b.m22 + a.m23 * b.m32;
    r.m23 = a.m20 * b.m03 + a.m21 * b.m13 + a.m22 * b.m23 + a.m23 * b.m33;

    r.m30 = a.m30 * b.m00 + a.m31 * b.m10 + a.m32 * b.m20 + a.m33 * b.m30;
    r.m31 = a.m30 * b.m01 + a.m31 * b.m11 + a.m32 * b.m21 + a.m33 * b.m31;
    r.m32 = a.m30 * b.m02 + a.m31 * b.m12 + a.m32 * b.m22 + a.m33 * b.m32;
    r.m33 = a.m30 * b.m03 + a.m31 * b.m13 + a.m32 * b.m23 + a.m33 * b.m33;

    return r;
}

Vector4 Matrix4MulVector4(Vector4 v, Matrix4 m) {
    Vector4 r;

    r.x = v.x * m.m00 + v.y * m.m10 + v.z * m.m20 + v.w * m.m30;
    r.y = v.x * m.m01 + v.y * m.m11 + v.z * m.m21 + v.w * m.m31;
    r.z = v.x * m.m02 + v.y * m.m12 + v.z * m.m22 + v.w * m.m32;
    r.w = v.x * m.m03 + v.y * m.m13 + v.z * m.m23 + v.w * m.m33;

    return r;
}

Vector2 WorldToScreen(Vector3 pos, char *visible, Matrix4 view,
                      Matrix4 projection, int screenWidth, int screenHeight) {
    // combine proj + view matrix
    Matrix4 pvMatrix = Matrix4fMul(view, projection);

    // convert to clip space
    Vector4 csp =
        Matrix4MulVector4((Vector4){pos.x, pos.y, pos.z, 1.f}, pvMatrix);

    // check if its behind the camera
    if (csp.w < 0.0f) {
        *visible = 0;
        return (Vector2){};
    }

    // normalize to device coordinates
    Vector3 ndc = {csp.x / csp.w, csp.y / csp.w, csp.z / csp.w};

    float screenX = (ndc.x + 1.0f) * screenWidth / 2.0f;
    float screenY = (1.0f - ndc.y) * screenHeight / 2.0f;

    *visible = 1;

    return (Vector2){screenX, screenY};
}