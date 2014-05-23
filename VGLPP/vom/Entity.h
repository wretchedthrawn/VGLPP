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

#ifndef __VGLPP_Demo__Entity__
#define __VGLPP_Demo__Entity__

namespace vom
{
  /**
   Generic abstract base class for all visual entities in the system
   */
  
  class Entity
  {
  public:
    Entity();
    virtual ~Entity();
    
    virtual void render()=0;
  };
}

#endif /* defined(__VGLPP_Demo__Entity__) */
