//
//  Quat.h

//This is old code that I slosh around that'll probably never die....

#import "VecTypes.h"

#ifdef __cplusplus
extern "C" {
#endif
  
union _QUAT
{
  struct { float x, y, z, w; };
  float q[4];
} __attribute__((aligned(16)));
  
typedef union _QUAT QUAT;

extern QUAT identity_quat;
  
static inline QUAT make_quat(float x, float y, float z, float w)
{
  QUAT ret = { x, y, z, w };
  return ret;
}

void get_vector_rotation_quat(QUAT *q, float ang, float x, float y, float z);
void quat_mul(QUAT *p, QUAT *q, QUAT *out);
void quat_to_matrix(QUAT *q, float m[16]);
void quat_inverse(QUAT *q, QUAT *out);
void apply_quat(QUAT *q, float3 *in, float3 *out);
void apply_quat_2d(QUAT *q, float2 *in, float2 *out);
  
#ifdef __cplusplus
}
#endif

