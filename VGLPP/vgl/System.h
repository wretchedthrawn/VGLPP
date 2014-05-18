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

#ifndef __VGLPP__System__
#define __VGLPP__System__

#include "VecTypes.h"

namespace vgl
{
  class System
  {
  public:
    ///Access to the current vgl system object
    static System &system();
    
    System();
    
    //Required methods for subclassing---------------------------------
    
    virtual uint2 dimensionsForBackingStore()=0;

    ///Return true if the OpenGL context is up and running
    virtual bool isGLContextReady()=0;
    
    ///This method should bind the default framebuffer to the context
    ///on desktop systems this is usually framebuffer 0
    virtual void bindDefaultFramebuffer()=0;
    
    //Optional methods for subclassing---------------------------------
    
    ///Makes the current GL context current, you must call superclass method
    ///when implementing this method.
    virtual void makeGLContextCurrent();
    
    //Other methods----------------------------------------------------
    
    ///Should be called whenever the screen resizes
    void backingResized();

    int backingWidth();
    int backingHeight();

  private:
    
  };
}
#endif /* defined(__VGLPP_Demo__System__) */
