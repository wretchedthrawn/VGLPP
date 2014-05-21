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

#include "BaseState.h"

using namespace std;

namespace vgl
{
  BaseState::BaseState()
  {
    transform = shared_ptr<EffectPropertyTransform>(new EffectPropertyTransform);
    
    //support up to 8 lights by default
    shared_ptr<EffectPropertyLight> light0, light1;
    for(int i = 0; i < 8; i++)
    {
      lights.push_back(shared_ptr<EffectPropertyLight>(new EffectPropertyLight));
      lights[i]->setEnabled(GL_FALSE);
    }
    light0 = lights.front();
    light0->setEnabled(GL_TRUE);
    
    //setup lighting defaults
    for(auto light : lights)
    {
      light->setPosition(make_float4(0, 0, 0, 0));
      light->setAmbient(make_float4(0, 0, 0, 1));
      
      light->setDiffuse(make_float4(1, 1, 1, 1));
      light->setSpecular(make_float4(1, 1, 1, 1));
      light->setSpotDirection(make_float3(0, 0, -1));
      light->setSpotCutoff(180);
      light->setSpotExponent(0);
      light->setAttenuation(1, 0, 0);
    }
    light0->setPosition(make_float4(0, 0, 1, 0));
    lightModelAmbientColor = make_float4(0.2f, 0.2f, 0.2f, 1.0f);
    
    //setup material and texture state
    material = shared_ptr<EffectPropertyMaterial>(new EffectPropertyMaterial);
    texture0 = shared_ptr<EffectPropertyTexture>(new EffectPropertyTexture);
    
    textureTransformMatrix = mat4Identity;
    
    material->setAmbient(make_float4(0.2f, 0.2f, 0.2f, 1.0f));
    material->setDiffuse(make_float4(0.8f, 0.8f, 0.8f, 1.0f));
    material->setSpecular(make_float4(0.0f, 0.0f, 0.0f, 1.0f));
    material->setEmissive(make_float4(0.0f, 0.0f, 0.0f, 1.0f));
  }
  
  shared_ptr<EffectPropertyLight> BaseState::lightAtIndex(unsigned int index)
  {
    return lights[index];
  }
}

