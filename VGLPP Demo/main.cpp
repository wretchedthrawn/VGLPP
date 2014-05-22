//
//  VGLPP Demo
//
//  This code comes at absolutely NO WARRANTY.
//  The author accepts no liability whatsoever for any effects,
//  adverse or otherwise, that may arise as a result of using this software.
//  By using this code, or any derivitive works thereof, you
//  are stating that you have read and accept these terms, and that you are
//  using this software at YOUR OWN RISK.
//

#include <iostream>
#include <memory>
#include "SDL.h"
#include "VGLPPObjLoaderDemo.h"
#ifndef GL3_PROTOTYPES
#define GL3_PROTOTYPES 1
#endif
#include <OpenGL/gl3.h>

using namespace std;

int main(int argc, const char *argv[])
{
  auto demoApp = std::unique_ptr<VGLPPObjLoaderDemo>(new VGLPPObjLoaderDemo);
  
  demoApp->run();
  demoApp.reset();
  
  return 0;
}

