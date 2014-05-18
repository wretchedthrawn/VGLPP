
//
//  VGLPP
//
//  This code comes at absolutely NO WARRANTY.
//  The author accepts no liability whatsoever for any effects,
//  adverse or otherwise, that may arise as a result of using this software.
//  By using this code, or any derivitive works thereof, you
//  are stating that you have read and accept these terms, and that you are
//  using this software at YOUR OWN RISK.
//

/*
 Fast inline matrix routines needed to avoid using glm in the verto core
 */

#ifndef __VGL_MATTYPES_H
#define __VGL_MATTYPES_H

#include <math.h>
#import "Quat.h"
#import "VecTypes.h"

/*
 m30, m31, and m32 correspond to the translation values tx, ty, and tz, respectively.
 m[12], m[13], and m[14] correspond to the translation values tx, ty, and tz, respectively.
 */
union _mat4
{
  struct
  {
    float m00, m01, m02, m03;
    float m10, m11, m12, m13;
    float m20, m21, m22, m23;
    float m30, m31, m32, m33;
  };
  float m[16];
} __attribute__((aligned(16)));
typedef union _mat4 mat4;


union _mat3
{
  struct
  {
    float m00, m01, m02;
    float m10, m11, m12;
    float m20, m21, m22;
  };
  float m[9];
};
typedef union _mat3 mat3;

extern const mat4 mat4Identity;
extern const mat3 mat3Identity;

#define MAT_TYPES_INLINE	static inline

MAT_TYPES_INLINE mat4 mat4Multiply(mat4 matrixLeft, mat4 matrixRight);

MAT_TYPES_INLINE float float3Length(float3 vector)
{
  return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

MAT_TYPES_INLINE float3 float3Normalize(float3 vector)
{
  float scale = 1.0f / float3Length(vector);
  float3 v = { vector.x * scale, vector.y * scale, vector.z * scale };
  return v;
}

MAT_TYPES_INLINE float float3DotProduct(float3 vectorLeft, float3 vectorRight)
{
  return vectorLeft.x * vectorRight.x + vectorLeft.y * vectorRight.y + vectorLeft.z * vectorRight.z;
}

MAT_TYPES_INLINE float4 float4Multiply(float4 lhs, float4 rhs)
{
  return make_float4(lhs.x*rhs.x, lhs.y*rhs.y, lhs.z*rhs.z, lhs.w*rhs.w);
}

MAT_TYPES_INLINE float4 float4Add(float4 lhs, float4 rhs)
{
  return make_float4(lhs.x+rhs.x, lhs.y+rhs.y, lhs.z+rhs.z, lhs.w+rhs.w);
}

MAT_TYPES_INLINE float3 float3Add(float3 lhs, float3 rhs)
{
  return make_float3(lhs.x+rhs.x, lhs.y+rhs.y, lhs.z+rhs.z);
}

MAT_TYPES_INLINE float3 float3Negate(float3 lhs)
{
  return make_float3(-lhs.x, -lhs.y, -lhs.z);
}

MAT_TYPES_INLINE mat4 mat4Make(float m00, float m01, float m02, float m03,
                               float m10, float m11, float m12, float m13,
                               float m20, float m21, float m22, float m23,
                               float m30, float m31, float m32, float m33)
{
  mat4 m =
  {
    m00, m01, m02, m03,
    m10, m11, m12, m13,
    m20, m21, m22, m23,
    m30, m31, m32, m33
  };
  return m;
}

//for syntax consitencies between vectypes and mattypes
#define make_mat4  mat4Make

MAT_TYPES_INLINE mat4 mat4MakeWithArray(float values[16])
{
  mat4 m =
  {
    values[0], values[1], values[2], values[3],
    values[4], values[5], values[6], values[7],
    values[8], values[9], values[10], values[11],
    values[12], values[13], values[14], values[15]
  };
  return m;
}

MAT_TYPES_INLINE mat4 mat4MakeTranslation(float tx, float ty, float tz)
{
  mat4 m = mat4Identity;
  m.m[12] = tx;
  m.m[13] = ty;
  m.m[14] = tz;
  return m;
}

MAT_TYPES_INLINE mat4 mat4MakeScale(float sx, float sy, float sz)
{
  mat4 m = mat4Identity;
  m.m[0] = sx;
  m.m[5] = sy;
  m.m[10] = sz;
  return m;
}

MAT_TYPES_INLINE mat4 mat4MakeRotation(float radians, float x, float y, float z)
{
  float3 v = float3Normalize(make_float3(x, y, z));
  float cos = cosf(radians);
  float cosp = 1.0f - cos;
  float sin = sinf(radians);
  
  mat4 m =
  {
    cos + cosp * v.x * v.x,
    cosp * v.x * v.y + v.z * sin,
    cosp * v.x * v.z - v.y * sin,
    0.0f,
    cosp * v.x * v.y - v.z * sin,
    cos + cosp * v.y * v.y,
    cosp * v.y * v.z + v.x * sin,
    0.0f,
    cosp * v.x * v.z + v.y * sin,
    cosp * v.y * v.z - v.x * sin,
    cos + cosp * v.z * v.z,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    1.0f
  };
  
  return m;
}

MAT_TYPES_INLINE mat4 mat4Translate(mat4 matrix, float tx, float ty, float tz)
{
  mat4 m =
  {
    matrix.m[0], matrix.m[1], matrix.m[2], matrix.m[3],
    matrix.m[4], matrix.m[5], matrix.m[6], matrix.m[7],
    matrix.m[8], matrix.m[9], matrix.m[10], matrix.m[11],
    matrix.m[0] * tx + matrix.m[4] * ty + matrix.m[8] * tz + matrix.m[12],
    matrix.m[1] * tx + matrix.m[5] * ty + matrix.m[9] * tz + matrix.m[13],
    matrix.m[2] * tx + matrix.m[6] * ty + matrix.m[10] * tz + matrix.m[14],
    matrix.m[15]
  };
  return m;
}

MAT_TYPES_INLINE mat4 mat4Scale(mat4 matrix, float sx, float sy, float sz)
{
  mat4 m =
  {
    matrix.m[0] * sx, matrix.m[1] * sx, matrix.m[2] * sx, matrix.m[3] * sx,
    matrix.m[4] * sy, matrix.m[5] * sy, matrix.m[6] * sy, matrix.m[7] * sy,
    matrix.m[8] * sz, matrix.m[9] * sz, matrix.m[10] * sz, matrix.m[11] * sz,
    matrix.m[12], matrix.m[13], matrix.m[14], matrix.m[15]
  };
  return m;
}

MAT_TYPES_INLINE mat4 mat4Rotate(mat4 matrix, float radians, float x, float y, float z)
{
  mat4 rm = mat4MakeRotation(radians, x, y, z);
  return mat4Multiply(matrix, rm);
}

MAT_TYPES_INLINE mat4 mat4RotateWithVector3(mat4 matrix, float radians, float3 axisVector)
{
  mat4 rm = mat4MakeRotation(radians, axisVector.x, axisVector.y, axisVector.z);
  return mat4Multiply(matrix, rm);
}

MAT_TYPES_INLINE mat4 mat4Multiply(mat4 matrixLeft, mat4 matrixRight)
{
  mat4 m;
  
  m.m[0]  = matrixLeft.m[0] * matrixRight.m[0]  + matrixLeft.m[4] * matrixRight.m[1]  + matrixLeft.m[8] * matrixRight.m[2]   + matrixLeft.m[12] * matrixRight.m[3];
	m.m[4]  = matrixLeft.m[0] * matrixRight.m[4]  + matrixLeft.m[4] * matrixRight.m[5]  + matrixLeft.m[8] * matrixRight.m[6]   + matrixLeft.m[12] * matrixRight.m[7];
	m.m[8]  = matrixLeft.m[0] * matrixRight.m[8]  + matrixLeft.m[4] * matrixRight.m[9]  + matrixLeft.m[8] * matrixRight.m[10]  + matrixLeft.m[12] * matrixRight.m[11];
	m.m[12] = matrixLeft.m[0] * matrixRight.m[12] + matrixLeft.m[4] * matrixRight.m[13] + matrixLeft.m[8] * matrixRight.m[14]  + matrixLeft.m[12] * matrixRight.m[15];
  
	m.m[1]  = matrixLeft.m[1] * matrixRight.m[0]  + matrixLeft.m[5] * matrixRight.m[1]  + matrixLeft.m[9] * matrixRight.m[2]   + matrixLeft.m[13] * matrixRight.m[3];
	m.m[5]  = matrixLeft.m[1] * matrixRight.m[4]  + matrixLeft.m[5] * matrixRight.m[5]  + matrixLeft.m[9] * matrixRight.m[6]   + matrixLeft.m[13] * matrixRight.m[7];
	m.m[9]  = matrixLeft.m[1] * matrixRight.m[8]  + matrixLeft.m[5] * matrixRight.m[9]  + matrixLeft.m[9] * matrixRight.m[10]  + matrixLeft.m[13] * matrixRight.m[11];
	m.m[13] = matrixLeft.m[1] * matrixRight.m[12] + matrixLeft.m[5] * matrixRight.m[13] + matrixLeft.m[9] * matrixRight.m[14]  + matrixLeft.m[13] * matrixRight.m[15];
  
	m.m[2]  = matrixLeft.m[2] * matrixRight.m[0]  + matrixLeft.m[6] * matrixRight.m[1]  + matrixLeft.m[10] * matrixRight.m[2]  + matrixLeft.m[14] * matrixRight.m[3];
	m.m[6]  = matrixLeft.m[2] * matrixRight.m[4]  + matrixLeft.m[6] * matrixRight.m[5]  + matrixLeft.m[10] * matrixRight.m[6]  + matrixLeft.m[14] * matrixRight.m[7];
	m.m[10] = matrixLeft.m[2] * matrixRight.m[8]  + matrixLeft.m[6] * matrixRight.m[9]  + matrixLeft.m[10] * matrixRight.m[10] + matrixLeft.m[14] * matrixRight.m[11];
	m.m[14] = matrixLeft.m[2] * matrixRight.m[12] + matrixLeft.m[6] * matrixRight.m[13] + matrixLeft.m[10] * matrixRight.m[14] + matrixLeft.m[14] * matrixRight.m[15];
  
	m.m[3]  = matrixLeft.m[3] * matrixRight.m[0]  + matrixLeft.m[7] * matrixRight.m[1]  + matrixLeft.m[11] * matrixRight.m[2]  + matrixLeft.m[15] * matrixRight.m[3];
	m.m[7]  = matrixLeft.m[3] * matrixRight.m[4]  + matrixLeft.m[7] * matrixRight.m[5]  + matrixLeft.m[11] * matrixRight.m[6]  + matrixLeft.m[15] * matrixRight.m[7];
	m.m[11] = matrixLeft.m[3] * matrixRight.m[8]  + matrixLeft.m[7] * matrixRight.m[9]  + matrixLeft.m[11] * matrixRight.m[10] + matrixLeft.m[15] * matrixRight.m[11];
	m.m[15] = matrixLeft.m[3] * matrixRight.m[12] + matrixLeft.m[7] * matrixRight.m[13] + matrixLeft.m[11] * matrixRight.m[14] + matrixLeft.m[15] * matrixRight.m[15];
  
  return m;
}

MAT_TYPES_INLINE float4 mat4MultiplyFloat4(mat4 matrixLeft, float4 vectorRight)
{
  float4 v =
  {
    matrixLeft.m[0] * vectorRight.x + matrixLeft.m[4] * vectorRight.y + matrixLeft.m[8] * vectorRight.z + matrixLeft.m[12] * vectorRight.w,
    matrixLeft.m[1] * vectorRight.x + matrixLeft.m[5] * vectorRight.y + matrixLeft.m[9] * vectorRight.z + matrixLeft.m[13] * vectorRight.w,
    matrixLeft.m[2] * vectorRight.x + matrixLeft.m[6] * vectorRight.y + matrixLeft.m[10] * vectorRight.z + matrixLeft.m[14] * vectorRight.w,
    matrixLeft.m[3] * vectorRight.x + matrixLeft.m[7] * vectorRight.y + matrixLeft.m[11] * vectorRight.z + matrixLeft.m[15] * vectorRight.w
  };
  return v;
}


MAT_TYPES_INLINE float quatLength(QUAT quaternion)
{
  return sqrt(quaternion.q[0] * quaternion.q[0] +
              quaternion.q[1] * quaternion.q[1] +
              quaternion.q[2] * quaternion.q[2] +
              quaternion.q[3] * quaternion.q[3]);
}

MAT_TYPES_INLINE QUAT quatNormalize(QUAT quaternion)
{
  float scale = 1.0f / quatLength(quaternion);
  QUAT q = { quaternion.q[0] * scale, quaternion.q[1] * scale, quaternion.q[2] * scale, quaternion.q[3] * scale };
  return q;
}

MAT_TYPES_INLINE mat4 mat4MakeWithQuaternion(QUAT quaternion)
{
  quaternion = quatNormalize(quaternion);
  
  float x = quaternion.x;
  float y = quaternion.y;
  float z = quaternion.z;
  float w = quaternion.w;
  
  float _2x = x + x;
  float _2y = y + y;
  float _2z = z + z;
  float _2w = w + w;
  
  mat4 m =
  {
    1.0f - _2y * y - _2z * z,
    _2x * y + _2w * z,
    _2x * z - _2w * y,
    0.0f,
    _2x * y - _2w * z,
    1.0f - _2x * x - _2z * z,
    _2y * z + _2w * x,
    0.0f,
    _2x * z + _2w * y,
    _2y * z - _2w * x,
    1.0f - _2x * x - _2y * y,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    1.0f
  };
  
  return m;
}


MAT_TYPES_INLINE mat3 mat4GetMatrix3(mat4 matrix)
{
  mat3 m =
  {
    matrix.m[0], matrix.m[1], matrix.m[2],
    matrix.m[4], matrix.m[5], matrix.m[6],
    matrix.m[8], matrix.m[9], matrix.m[10]
  };
  return m;
}

#ifdef __cplusplus 
extern "C" {
#endif
  
mat4 mat4Invert(mat4 matrix, bool *isInvertible);

mat3 mat3Invert(mat3 matrix, bool *isInvertible);
mat3 mat3InvertAndTranspose(mat3 matrix, bool *isInvertible);

#ifdef __cplusplus
}
#endif

MAT_TYPES_INLINE float3 float3CrossProduct(float3 vectorLeft, float3 vectorRight)
{
  float3 v = { vectorLeft.y * vectorRight.z - vectorLeft.z * vectorRight.y,
    vectorLeft.z * vectorRight.x - vectorLeft.x * vectorRight.z,
    vectorLeft.x * vectorRight.y - vectorLeft.y * vectorRight.x };
  return v;
}

MAT_TYPES_INLINE mat4 mat4MakeLookAt(float eyeX, float eyeY, float eyeZ,
                                           float centerX, float centerY, float centerZ,
                                           float upX, float upY, float upZ)
{
  float3 ev = { eyeX, eyeY, eyeZ };
  float3 cv = { centerX, centerY, centerZ };
  float3 uv = { upX, upY, upZ };
  float3 n = float3Normalize(float3Add(ev, float3Negate(cv)));
  float3 u = float3Normalize(float3CrossProduct(uv, n));
  float3 v = float3CrossProduct(n, u);
  
  mat4 m =
  {
    u.x, v.x, n.x, 0.0f,
    u.y, v.y, n.y, 0.0f,
    u.z, v.z, n.z, 0.0f,
    float3DotProduct(float3Negate(u), ev),
    float3DotProduct(float3Negate(v), ev),
    float3DotProduct(float3Negate(n), ev),
    1.0f
  };
  
  return m;
}

MAT_TYPES_INLINE mat4 mat4MakePerspective(float fovyRadians, float aspect, float nearZ, float farZ)
{
  float cotan = 1.0f / tanf(fovyRadians / 2.0f);
  
  mat4 m = { cotan / aspect, 0.0f, 0.0f, 0.0f,
    0.0f, cotan, 0.0f, 0.0f,
    0.0f, 0.0f, (farZ + nearZ) / (nearZ - farZ), -1.0f,
    0.0f, 0.0f, (2.0f * farZ * nearZ) / (nearZ - farZ), 0.0f };
  
  return m;
}

MAT_TYPES_INLINE mat4 mat4MakeFrustum(float left, float right,
                                            float bottom, float top,
                                            float nearZ, float farZ)
{
  float ral = right + left;
  float rsl = right - left;
  float tsb = top - bottom;
  float tab = top + bottom;
  float fan = farZ + nearZ;
  float fsn = farZ - nearZ;
  
  mat4 m =
  {
    2.0f * nearZ / rsl, 0.0f, 0.0f, 0.0f,
    0.0f, 2.0f * nearZ / tsb, 0.0f, 0.0f,
    ral / rsl, tab / tsb, -fan / fsn, -1.0f,
    0.0f, 0.0f, (-2.0f * farZ * nearZ) / fsn, 0.0f
  };
  
  return m;
}

MAT_TYPES_INLINE mat4 mat4MakeOrtho(float left, float right,
                                          float bottom, float top,
                                          float nearZ, float farZ)
{
  float ral = right + left;
  float rsl = right - left;
  float tab = top + bottom;
  float tsb = top - bottom;
  float fan = farZ + nearZ;
  float fsn = farZ - nearZ;
  
  mat4 m =
  {
    2.0f / rsl, 0.0f, 0.0f, 0.0f,
    0.0f, 2.0f / tsb, 0.0f, 0.0f,
    0.0f, 0.0f, -2.0f / fsn, 0.0f,
    -ral / rsl, -tab / tsb, -fan / fsn, 1.0f
  };
  
  return m;
}

MAT_TYPES_INLINE mat3 mat3Make(float m00, float m01, float m02,
                               float m10, float m11, float m12,
                               float m20, float m21, float m22)
{
  mat3 m =
  {
    m00, m01, m02,
    m10, m11, m12,
    m20, m21, m22
  };
  return m;
}

//for syntax consistencies between vectypes and mattypes
#define make_mat3  mat3Make

#endif