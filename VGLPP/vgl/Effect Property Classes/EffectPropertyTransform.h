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

#ifndef __VGLPP__EffectPropertyTransform__
#define __VGLPP__EffectPropertyTransform__

#include "MatTypes.h"

namespace vgl
{
  class EffectPropertyTransform
  {
  public:
    inline mat4 getModelviewMatrix() { return modelviewMatrix; }
    inline mat4 getProjectionMatrix() { return projectionMatrix; }
    inline mat3 getNormalMatrix() { return normalMatrix; }
    
    void setModelviewMatrix(const mat4 &mv) { modelviewMatrix = mv; }
    void setProjectionMatrix(const mat4 &p) { projectionMatrix = p; }
    void setNormalMatrix(const mat3 &n) { normalMatrix = n; }
    
    EffectPropertyTransform();
    
  private:
    mat4 modelviewMatrix, projectionMatrix;
    mat3 normalMatrix;
  };
}
#endif /* defined(__VGLPP_Demo__EffectPropertyTransform__) */
