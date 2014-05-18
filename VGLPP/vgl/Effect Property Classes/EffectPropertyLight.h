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

#ifndef __VGLPP__EffectPropertyLight__
#define __VGLPP__EffectPropertyLight__

#include <memory>
#include "vgl.h"
#include "VecTypes.h"

namespace vgl
{
  class EffectPropertyTransform;
  
  class EffectPropertyLight
  {
  public:
    inline void setEnabled(GLboolean b) { enabled = b; }
    inline void setPosition(float4 pos) { position = pos; }
    inline void setWorldPosition(float4 pos) { worldPosition = pos; }
    inline void setAmbient(float4 amb) { ambientColor = amb; }
    inline void setDiffuse(float4 diff) { diffuseColor = diff; }
    inline void setSpecular(float4 spec) { specularColor = spec; }
    inline void setSpotDirection(float3 spotDir) { spotDirection = spotDir; };
    inline void setWorldSpotDirection(float3 spotDir) { worldSpotDirection = spotDir; };
    inline void setSpotExponent(GLfloat se) { spotExponent = se; }
    inline void setSpotCutoff(GLfloat sc) { spotCutoff = sc; }
    void setAttenuation(GLfloat c, GLfloat l, GLfloat q);
    inline void setTransform(std::shared_ptr<EffectPropertyTransform> t) { transform = t; }
    
    inline GLboolean isEnabled() { return enabled; }
    inline float4 getPosition() { return position; }
    inline float4 getWorldPosition() { return worldPosition; }
    inline float4 getAmbient() { return ambientColor; }
    inline float4 getDiffuse() { return diffuseColor; }
    inline float4 getSpecular() { return specularColor; }
    inline float3 getSpotDirection() { return spotDirection; }
    inline float3 getWorldSpotDirection() { return worldSpotDirection; }
    inline GLfloat getSpotExponent() { return spotExponent; }
    inline GLfloat getSpotCutoff() { return spotCutoff; }
    inline float3 getAttenuation() { return make_float3(constantAttenuation, linearAttenuation, quadraticAttenuation); }
    inline std::shared_ptr<EffectPropertyTransform> getTransform() { return transform; }

    EffectPropertyLight();
    
  private:
    GLboolean enabled;
    float4 position;
    float4 worldPosition;
    float4 ambientColor;
    float4 diffuseColor;
    float4 specularColor;
    float3 spotDirection;
    float3 worldSpotDirection;
    GLfloat spotExponent;
    GLfloat spotCutoff;
    GLfloat constantAttenuation, linearAttenuation, quadraticAttenuation;
    std::shared_ptr<EffectPropertyTransform> transform;
  };
}

#endif /* defined(__VGLPP_Demo__EffectPropertyLight__) */
