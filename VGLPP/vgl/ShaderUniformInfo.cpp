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

#include <stdlib.h>
#include "ShaderUniformInfo.h"

namespace vgl
{
  ShaderUniformInfo::ShaderUniformInfo() : name(""), type(0), format(0), count(0), uiType(0), isMatrixFlag(false), setByMaterialFlag(false), valueData(nullptr)
  {
    
  }
  
  ShaderUniformInfo::~ShaderUniformInfo()
  {
    free(valueData);
  }

  void ShaderUniformInfo::set(GLenum t, GLenum f, int c, bool isMat)
  {
    type = t;
    format = f;
    count = c;
    isMatrixFlag = isMat;
  }

  template <typename T>
  void ShaderUniformInfo::set(GLenum t, GLenum f, int c, bool isMat, const T &value)
  {
    type = t;
    format = f;
    count = c;
    isMatrixFlag = isMat;
    setValue<T>(value);
  }
  
  template <typename T>
  void ShaderUniformInfo::setValue(const T &value)
  {
    if(valueData)
      free(valueData);
    valueData = malloc(sizeof(T));
    memcpy(valueData, value, sizeof(T));
  }
  
  template <typename T>
  void ShaderUniformInfo::clearValue()
  {
    if(valueData)
      free(valueData);
    valueData = nullptr;
  }
  
  int ShaderUniformInfo::intValue()
  {
    return *(static_cast<int *>(valueData));
  }
  
  int2 ShaderUniformInfo::int2Value()
  {
    return *(static_cast<int2 *>(valueData));
  }
  
  int3 ShaderUniformInfo::int3Value()
  {
    return *(static_cast<int3 *>(valueData));
  }
  
  int4 ShaderUniformInfo::int4Value()
  {
    return *(static_cast<int4 *>(valueData));
  }
  
  float ShaderUniformInfo::floatValue()
  {
    return *(static_cast<float *>(valueData));
  }
  
  float2 ShaderUniformInfo::float2Value()
  {
    return *(static_cast<float2 *>(valueData));
  }
  
  float3 ShaderUniformInfo::float3Value()
  {
    return *(static_cast<float3 *>(valueData));
  }
  
  float4 ShaderUniformInfo::float4Value()
  {
    return *(static_cast<float4 *>(valueData));
  }
  
  float16 ShaderUniformInfo::mat4Value()
  {
    return *(static_cast<float16 *>(valueData));
  }
}