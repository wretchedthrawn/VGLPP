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

#include "EffectPropertyLight.h"
#include "EffectPropertyTransform.h"

namespace vgl
{
  EffectPropertyLight::EffectPropertyLight()
  {
    enabled = true;
    position = make_float4(0, 0, 0, 1);
    ambientColor = make_float4(0, 0, 0, 1);
    diffuseColor = make_float4(1, 1, 1, 1);
    specularColor = make_float4(1, 1, 1, 1);
    spotDirection = make_float3(0, 0, -1);
    spotExponent = 0;
    spotCutoff = 180;
    constantAttenuation = 1;
    linearAttenuation = 0;
    quadraticAttenuation = 0;
    
    transform = std::shared_ptr<EffectPropertyTransform>(new EffectPropertyTransform);
  }
  
  void EffectPropertyLight::setAttenuation(GLfloat c, GLfloat l, GLfloat q)
  {
    constantAttenuation = c;
    linearAttenuation = l;
    quadraticAttenuation = q;
  }
}