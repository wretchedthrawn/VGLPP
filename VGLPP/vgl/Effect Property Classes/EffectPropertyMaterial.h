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

#ifndef __VGLPP__EffectPropertyMaterial__
#define __VGLPP__EffectPropertyMaterial__

#include "vgl.h"
#include "VecTypes.h"

namespace vgl
{
  /**
   Effect property classes such as this exist to store state model so
   it can be stored in one place and easily passed between shader effects.
   Their only purpose is this and they live in the BaseState class
   */

  class EffectPropertyMaterial
  {
  public:
    inline float4 getAmbient() { return ambientColor; }
    inline float4 getDiffuse() { return diffuseColor; }
    inline float4 getSpecular() { return specularColor; }
    inline float4 getEmissive() { return emissiveColor; }
    inline GLfloat getShininess() { return shininess; }
    
    inline void setAmbient(float4 amb) { ambientColor = amb; }
    inline void setDiffuse(float4 diff) { diffuseColor = diff; }
    inline void setSpecular(float4 spec) { specularColor = spec; }
    inline void setEmissive(float4 em) { emissiveColor = em; }
    inline void setShininess(GLfloat s) { shininess = s; }
    
    void setMaterial(float4 amb, float4 diff, float4 spec, float4 emissive, GLfloat shininess);
    
    EffectPropertyMaterial();
    
  private:
    float4 ambientColor;
    float4 diffuseColor;
    float4 specularColor;
    float4 emissiveColor;
    GLfloat shininess;
  };
}

#endif /* defined(__VGLPP_Demo__EffectPropertyMaterial__) */
