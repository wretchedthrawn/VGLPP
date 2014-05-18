
#include "MatTypes.h"

const mat4 mat4Identity = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
const mat3 mat3Identity = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };

mat4 mat4Invert(mat4 matrix, bool *isInvertible)
{
  mat4 ret;
  float *a = matrix.m;
  float *out = ret.m;

  if(isInvertible)
    *isInvertible = true;

  float a00 = a[0], a01 = a[1], a02 = a[2], a03 = a[3],
  a10 = a[4], a11 = a[5], a12 = a[6], a13 = a[7],
  a20 = a[8], a21 = a[9], a22 = a[10], a23 = a[11],
  a30 = a[12], a31 = a[13], a32 = a[14], a33 = a[15],
  
  b00 = a00 * a11 - a01 * a10,
  b01 = a00 * a12 - a02 * a10,
  b02 = a00 * a13 - a03 * a10,
  b03 = a01 * a12 - a02 * a11,
  b04 = a01 * a13 - a03 * a11,
  b05 = a02 * a13 - a03 * a12,
  b06 = a20 * a31 - a21 * a30,
  b07 = a20 * a32 - a22 * a30,
  b08 = a20 * a33 - a23 * a30,
  b09 = a21 * a32 - a22 * a31,
  b10 = a21 * a33 - a23 * a31,
  b11 = a22 * a33 - a23 * a32,
  
  // Calculate the determinant
  det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
  
  if(!det)
  {
    if(isInvertible)
      *isInvertible = false;
    return ret;
  }
  det = 1.0f / det;
  
  out[0] = (a11 * b11 - a12 * b10 + a13 * b09) * det;
  out[1] = (a02 * b10 - a01 * b11 - a03 * b09) * det;
  out[2] = (a31 * b05 - a32 * b04 + a33 * b03) * det;
  out[3] = (a22 * b04 - a21 * b05 - a23 * b03) * det;
  out[4] = (a12 * b08 - a10 * b11 - a13 * b07) * det;
  out[5] = (a00 * b11 - a02 * b08 + a03 * b07) * det;
  out[6] = (a32 * b02 - a30 * b05 - a33 * b01) * det;
  out[7] = (a20 * b05 - a22 * b02 + a23 * b01) * det;
  out[8] = (a10 * b10 - a11 * b08 + a13 * b06) * det;
  out[9] = (a01 * b08 - a00 * b10 - a03 * b06) * det;
  out[10] = (a30 * b04 - a31 * b02 + a33 * b00) * det;
  out[11] = (a21 * b02 - a20 * b04 - a23 * b00) * det;
  out[12] = (a11 * b07 - a10 * b09 - a12 * b06) * det;
  out[13] = (a00 * b09 - a01 * b07 + a02 * b06) * det;
  out[14] = (a31 * b01 - a30 * b03 - a32 * b00) * det;
  out[15] = (a20 * b03 - a21 * b01 + a22 * b00) * det;
  
  return ret;
}

mat3 mat3Invert(mat3 matrix, bool *isInvertible)
{
  mat3 ret;
  float *a = matrix.m;
  float *out = ret.m;
 
  if(isInvertible)
    *isInvertible = true;
  
  float a00 = a[0], a01 = a[1], a02 = a[2],
  a10 = a[3], a11 = a[4], a12 = a[5],
  a20 = a[6], a21 = a[7], a22 = a[8],
  
  b01 = a22 * a11 - a12 * a21,
  b11 = -a22 * a10 + a12 * a20,
  b21 = a21 * a10 - a11 * a20,
  
  // Calculate the determinant
  det = a00 * b01 + a01 * b11 + a02 * b21;
  
  if(!det)
  {
    if(isInvertible)
      *isInvertible = false;
    return ret;
  }
  
  det = 1.0f / det;
  
  out[0] = b01 * det;
  out[1] = (-a22 * a01 + a02 * a21) * det;
  out[2] = (a12 * a01 - a02 * a11) * det;
  out[3] = b11 * det;
  out[4] = (a22 * a00 - a02 * a20) * det;
  out[5] = (-a12 * a00 + a02 * a10) * det;
  out[6] = b21 * det;
  out[7] = (-a21 * a00 + a01 * a20) * det;
  out[8] = (a11 * a00 - a01 * a10) * det;
  return ret;
}

MAT_TYPES_INLINE mat3 mat3Transpose(mat3 matrix)
{
  mat3 m =
  {
    matrix.m[0], matrix.m[3], matrix.m[6],
    matrix.m[1], matrix.m[4], matrix.m[7],
    matrix.m[2], matrix.m[5], matrix.m[8]
  };
  return m;
}

mat3 mat3InvertAndTranspose(mat3 matrix, bool *isInvertible)
{
  //invert, then transpose
  mat3 inverted = mat3Invert(matrix, isInvertible);
  
  return mat3Transpose(inverted);
}