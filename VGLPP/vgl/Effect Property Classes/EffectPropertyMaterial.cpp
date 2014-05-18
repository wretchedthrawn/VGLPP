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

#include "EffectPropertyMaterial.h"

namespace vgl
{
  EffectPropertyMaterial::EffectPropertyMaterial()
  {
    ambientColor = make_float4(0.2, 0.2, 0.2, 1.0);
    diffuseColor = make_float4(0.8, 0.8, 0.8, 1.0);
    specularColor = make_float4(0.0, 0.0, 0.0, 1.0);
    emissiveColor =  make_float4(0.0, 0.0, 0.0, 1.0);
    shininess = 0.0f;
  }
  
  void EffectPropertyMaterial::setMaterial(float4 amb, float4 diff, float4 spec, float4 emissive, GLfloat shininess)
  {
    ambientColor = amb;
    diffuseColor = diff;
    specularColor = spec;
    emissiveColor = emissive;
    this->shininess = shininess;
  }
}