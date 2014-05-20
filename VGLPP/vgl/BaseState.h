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

#ifndef __VGLPP__BaseState__
#define __VGLPP__BaseState__

#include <memory>
#include <vector>
#include "vgl.h"
#include "VecTypes.h"
#include "EffectPropertyTransform.h"
#include "EffectPropertyTexture.h"
#include "EffectPropertyMaterial.h"
#include "EffectPropertyLight.h"

/**
 This class serves as the end-all catch-all for the current
 state.  It is set by the state machine and referenced by all
 GL effect objects to query the state.
 */

namespace vgl
{
  class BaseState
  {
  public:
    //quick accessors
    inline std::shared_ptr<EffectPropertyTransform> getTransform() { return transform; }
    inline std::shared_ptr<EffectPropertyMaterial> getMaterial() { return material; }
    inline std::shared_ptr<EffectPropertyTexture> getTexture0() { return texture0; }
    
    inline mat4 getTextureTransformMatrix() { return textureTransformMatrix; }
    inline void setTextureTransformMatrix(mat4 m) { textureTransformMatrix = m; }

    inline float4 getLightModelAmbientColor() { return lightModelAmbientColor; }
    inline void setLightModelAmbientColor(float4 color) { lightModelAmbientColor = color; }
    
    inline float4 getConstantColor() { return constantColor; }
    inline void setConstantColor(float4 color) { constantColor = color; }

    inline void setLightingEnabled(GLboolean b) { lightingEnabled = b; }
    inline GLboolean isLightingEnabled() { return lightingEnabled; }
        
    std::shared_ptr<EffectPropertyLight> lightAtIndex(unsigned int index);
    inline const std::vector<std::shared_ptr<EffectPropertyLight> > &getLights() { return lights; }

    BaseState();

  private:
    GLboolean lightingEnabled;
    
    mat4 textureTransformMatrix;
    
    //Transform state
    std::shared_ptr<EffectPropertyTransform> transform;
    
    //Lights
    std::vector<std::shared_ptr<EffectPropertyLight> > lights;
    
    //Material
    std::shared_ptr<EffectPropertyMaterial> material;
    
    //Texture
    std::shared_ptr<EffectPropertyTexture> texture0;
    
    float4 constantColor;
    float4 lightModelAmbientColor;
  };
}

#endif /* defined(__VGLPP_Demo__BaseState__) */
