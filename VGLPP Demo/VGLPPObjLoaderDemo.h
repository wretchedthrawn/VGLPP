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

#ifndef __VGLPP_Demo__VGLPPObjLoaderDemo__
#define __VGLPP_Demo__VGLPPObjLoaderDemo__

#include <memory>
#include "System.h"
#include "SDL.h"

namespace vgl
{
  class StateMachine;
}

class VGLPPObjLoaderDemo : private vgl::System
{
public:
  VGLPPObjLoaderDemo();
  virtual ~VGLPPObjLoaderDemo();
  
  void run();
  
  //necessary vgl system stuff
  virtual uint2 dimensionsForBackingStore();
  virtual bool isGLContextReady();
  virtual void bindDefaultFramebuffer();

private:
  bool processEvents();
  void doRender();
  
  SDL_Window *window;
  SDL_GLContext context;
  int screenW = 0, screenH = 0;
  
  //convenient pointer to the vgl state machine
  std::shared_ptr<vgl::StateMachine> vgl;
};

#endif /* defined(__VGLPP_Demo__VGLPPObjLoaderDemo__) */
