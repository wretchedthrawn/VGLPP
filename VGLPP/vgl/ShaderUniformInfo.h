//
//  VGLPP
//  A very limited port of the Verto Core GL Layer from Verto Studio to C++
//  http://vertostudio.com
//
//
//  This code comes at absolutely NO WARRANTY.
//  The author accepts no liability whatsoever for any effects,
//  adverse or otherwise, that may arise as a result of using this software.
//  By using this code, or any derivitive works thereof, you
//  are stating that you have read and accept these terms, and that you are
//  using this software at YOUR OWN RISK.
//

#ifndef __VGLPP_Demo__ShaderUniformInfo__
#define __VGLPP_Demo__ShaderUniformInfo__

#include <string>
#include "vgl.h"
#include "VecTypes.h"

namespace vgl
{
  class ShaderUniformInfo
  {
  public:
    void set(GLenum type, GLenum format, int count, bool isMatrix);

    template <typename T>
    void set(GLenum type, GLenum format, int count, bool isMatrix, const T &value);
    
    template <typename T>
    void setValue(const T &value);

    template <typename T>
    void clearValue();

    inline operator bool()
    {
      return (valueData != nullptr);
    }
    
    inline GLenum getType() { return type; }
    inline GLenum getFormat() { return format; }
    inline int getCount() { return count; };
    inline bool isMatrix() { return isMatrixFlag; }
    
    inline const std::string &getName() { return name; }
    inline void setName(const std::string &n) { name = n; }
    
    inline void setByMaterial(bool b=true) { setByMaterialFlag = b; }
    inline bool isSetByMaterial() { return setByMaterialFlag; }
    
    int intValue();
    int2 int2Value();
    int3 int3Value();
    int4 int4Value();
    
    float floatValue();
    float2 float2Value();
    float3 float3Value();
    float4 float4Value();
    
    float16 mat4Value();
    
    inline void *getValueData() { return valueData; }
    
    ShaderUniformInfo();
    ~ShaderUniformInfo();
    
  private:
    std::string name;
    
    GLenum type, format;
    int count;
    int uiType;
    bool isMatrixFlag;
    bool setByMaterialFlag;
    
    void *valueData;
  };
}

#endif /* defined(__VGLPP_Demo__ShaderUniformInfo__) */
