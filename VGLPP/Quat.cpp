//
//  Quat.cpp

//This is old code that I slosh around that'll probably never die....

#include <math.h>
#import "Quat.h"

static float RAD(float n)  { return (n * (M_PI/180.0)); }

QUAT identity_quat = { 0.0, 0.0, 0.0, 1.0 };

void get_vector_rotation_quat(QUAT *q, float ang, float x, float y, float z)
{
  float l;
  float s;
    
  l = sqrtf(x*x + y*y + z*z);
  
  x /= l;
  y /= l;
  z /= l;
  
  float a = -RAD(ang);  
  q->w = cosf(a/2);
  s = sinf(a/2);
  
  q->x = s * x;
  q->y = s * y;
  q->z = s * z;
}

void quat_mul(QUAT *p, QUAT *q, QUAT *out)
{
  QUAT temp;
  /* qp = ww' - v.v' + vxv' + wv' + w'v */
  
  if(p == out) {
    temp = *p;
    p = &temp;
  }
  else if (q == out) {
    temp = *q;
    q = &temp;
  }
  
  /* w" = ww' - xx' - yy' - zz' */
  out->w = (p->w * q->w) -
  (p->x * q->x) -
  (p->y * q->y) -
  (p->z * q->z);
  
  /* x" = wx' + xw' + yz' - zy' */
  out->x = (p->w * q->x) +
  (p->x * q->w) +
  (p->y * q->z) -
  (p->z * q->y);
  
  /* y" = wy' + yw' + zx' - xz' */
  out->y = (p->w * q->y) +
  (p->y * q->w) +
  (p->z * q->x) -
  (p->x * q->z);
  
  /* z" = wz' + zw' + xy' - yx' */
  out->z = (p->w * q->z) +
  (p->z * q->w) +
  (p->x * q->y) -
  (p->y * q->x);
}

void quat_to_matrix(QUAT *q, float m[16])
{
  float ww;
  float xx;
  float yy;
  float zz;
  float wx;
  float wy;
  float wz;
  float xy;
  float xz;
  float yz;
  
  /* This is the layout for converting the values in a quaternion to a
   * matrix.
   *
   *  | ww + xx - yy - zz       2xy + 2wz             2xz - 2wy     |
   *  |     2xy - 2wz       ww - xx + yy - zz         2yz - 2wx     |
   *  |     2xz + 2wy           2yz - 2wx         ww + xx - yy - zz |
   */
  
  ww = q->w * q->w;
  xx = q->x * q->x;
  yy = q->y * q->y;
  zz = q->z * q->z;
  wx = q->w * q->x * 2;
  wy = q->w * q->y * 2;
  wz = q->w * q->z * 2;
  xy = q->x * q->y * 2;
  xz = q->x * q->z * 2;
  yz = q->y * q->z * 2;
  
  m[0] = ww + xx - yy - zz;
  m[1] = xy - wz;
  m[2] = xz + wy;
  m[3] = 0;
  
  m[4] = xy + wz;
  m[5] = ww - xx + yy - zz;
  m[6] = yz - wx;
  m[7] = 0;
  
  m[8] = xz - wy;
  m[9] = yz + wx;
  m[10] = ww - xx - yy + zz;
  m[11] = 0;
  
  m[12] = m[13] = m[14] = 0;
  m[15] = 1;  
}

static void quat_conjugate(QUAT *q, QUAT *out)
{
  /* q^* = w - x - y - z */
  out->w =  (q->w);
  out->x = -(q->x);
  out->y = -(q->y);
  out->z = -(q->z);
}

static float quat_normal(QUAT *q)
{  
  /* N(q) = ww + xx + yy + zz */
  return ((q->w * q->w) + (q->x * q->x) + (q->y * q->y) + (q->z * q->z));
}

void quat_inverse(QUAT *q, QUAT *out)
{
  QUAT  con;
  float norm;

  /* q^-1 = q^* / N(q) */
  
  quat_conjugate(q, &con);
  norm = quat_normal(q);
    
  out->w = con.w / norm;
  out->x = con.x / norm;
  out->y = con.y / norm;
  out->z = con.z / norm;
}

void apply_quat(QUAT *q, float3 *in, float3 *out)
{
  QUAT v;
  QUAT i;
  QUAT t;
  
  /* v' = q * v * q^-1 */
  
  v.w = 0;
  v.x = in->x;
  v.y = in->y;
  v.z = in->z;
    
  quat_inverse(q, &i);
  quat_mul(&i, &v, &t);
  quat_mul(&t,  q, &v);
  
  out->x = v.x;
  out->y = v.y;
  out->z = v.z;
}

void apply_quat_2d(QUAT *q, float2 *in, float2 *out)
{
  QUAT v;
  QUAT i;
  QUAT t;
  
  /* v' = q * v * q^-1 */
  
  v.w = 0;
  v.x = in->x;
  v.y = in->y;
  v.z = 0;
  
  quat_inverse(q, &i);
  quat_mul(&i, &v, &t);
  quat_mul(&t,  q, &v);
  
  out->x = v.x;
  out->y = v.y;  
}

