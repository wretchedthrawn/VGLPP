//
//  VGLPP Demo
//
//  This code comes at absolutely NO WARRANTY.
//  The author accepts no liability whatsoever for any effects,
//  adverse or otherwise, that may arise as a result of using this software.
//  By using this code, or any derivitive works thereof, you
//  are stating that you have read and accept these terms, and that you are
//  using this software at YOUR OWN RISK.
//

#ifndef __VGLPP_VecTypes__
#define __VGLPP_VecTypes__

typedef struct float2
{
  float x, y;  
} float2;

typedef struct float3
{
  float x, y, z;
} float3;

typedef struct float4
{
  union
  {
    struct
    {
      float x, y, z, w;
    };
    struct
    {
      float r, g, b, a;
    };
  };
  
} float4;

typedef struct float8
{
  union
  {
    struct
    {
      float x, y, z, w, a, b, c, d;
    };
    struct
    {
      float4 hi, lo;
    };
  };
  
} float8;

typedef struct float16
{
	float a1, a2, a3, a4;
	float b1, b2, b3, b4;
	float c1, c2, c3, c4;
	float d1, d2, d3, d4;
} float16;

typedef struct ushort3
{
  unsigned short x, y, z; 
} ushort3;

typedef struct int2
{
  int x, y;
} int2;

typedef struct int3
{
  int x, y, z;
} int3;

typedef struct int4
{
  int x, y, z, w;
} int4;

typedef struct uint2
{
  unsigned int x, y;
} uint2;

typedef struct uint3
{
  unsigned int x, y, z;
} uint3;

typedef struct uint4
{
  unsigned int x, y, z, w;
} uint4;

static inline float4 make_float4(float x, float y, float z, float w)
{
  float4 ret = { x, y, z, w };
  return ret;
}

static inline float4 make_float4v(float3 v, float w)
{
  float4 ret = { v.x, v.y, v.z, w };
  return ret;
}


static inline float3 make_float3(float x, float y, float z)
{
  float3 ret = { x, y, z };
  return ret;
}

static inline float2 make_float2(float x, float y)
{
  float2 ret = { x, y };
  return ret;
}

static inline ushort3 make_ushort3(unsigned short x, unsigned short y, unsigned short z)
{
  ushort3 ret = { x, y, z };
  return ret;
}

static inline ushort3 make_ushort3_reverse(unsigned short x, unsigned short y, unsigned short z)
{
  ushort3 ret = { z, y, x };
  return ret;
}

static inline void reverse_ushort3_order(ushort3 *tri)
{
  unsigned short tmp = tri->x;
  tri->x = tri->z;
  tri->z = tmp;
}

static inline uint2 make_uint2(unsigned int x, unsigned int y)
{
  uint2 ret = { x, y };
  return ret;
}

static inline uint3 make_uint3(unsigned int x, unsigned int y, unsigned int z)
{
  uint3 ret = { x, y, z };
  return ret;
}

static inline uint3 make_uint3_reverse(unsigned int x, unsigned int y, unsigned int z)
{
  uint3 ret = { z, y, x };
  return ret;
}

static inline void reverse_uint3_order(uint3 *tri)
{
  unsigned int tmp = tri->x;
  tri->x = tri->z;
  tri->z = tmp;
}

static inline uint4 make_uint4(unsigned int x, unsigned int y, unsigned int z, unsigned int w)
{
  uint4 ret = { x, y, z, w };
  return ret;
}

//Generates the quad in the proper order for GL_TRIANGLE_STRIP quad emulation
static inline uint4 make_uint4_tri_strip(unsigned int x, unsigned int y, unsigned int z, unsigned int w)
{
  uint4 ret = { x, y, w, z };
  return ret;
}

static inline uint4 make_uint4_reverse(unsigned int x, unsigned int y, unsigned int z, unsigned int w)
{
  uint4 ret = { w, z, y, x };
  return ret;
}

static inline void reverse_uint4_order(uint4 *quad)
{
  uint4 reversed = make_uint4_reverse(quad->x, quad->y, quad->z, quad->w);
  *quad = reversed;
}

static inline int2 make_int2(int x, int y)
{
  int2 ret = { x, y };
  return ret;
}

static inline int3 make_int3(int x, int y, int z)
{
  int3 ret = { x, y, z };
  return ret;
}

static inline int4 make_int4(int x, int y, int z, int w)
{
  int4 ret = { x, y, z, w };
  return ret;
}

#define FLOAT4_EQUAL(vec1, vec2)   (vec1.x == vec2.x && vec1.y == vec2.y && vec1.z == vec2.z && vec1.w == vec2.w)
#define FLOAT3_EQUAL(vec1, vec2)   (vec1.x == vec2.x && vec1.y == vec2.y && vec1.z == vec2.z)

#endif