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

#ifndef __VGLPP_Demo__EffectPropertyTexture__
#define __VGLPP_Demo__EffectPropertyTexture__

#include "vgl.h"

namespace vgl
{
  /**
   Effect property classes such as this exist to store state model so
   it can be stored in one place and easily passed between shader effects.
   Their only purpose is this and they live in the BaseState class
   */

  class EffectPropertyTexture
  {
  public:
    inline GLuint getName() { return name; }
    inline GLboolean isEnabled() { return enabled; }
    
    inline void setName(GLuint n) { name = n; }
    inline void setEnabled(GLboolean b) { enabled = b; }
    
    EffectPropertyTexture();    
    
  private:
    GLuint name;
    GLboolean enabled;
  };
}

#endif /* defined(__VGLPP_Demo__EffectPropertyTexture__) */
