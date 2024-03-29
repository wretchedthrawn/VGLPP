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

#ifndef VGLPP_Demo_vgl_h
#define VGLPP_Demo_vgl_h

#ifdef WIN32
#ifndef GL3_PROTOTYPES
#define GL3_PROTOTYPES 1
#endif
#include "GL/gl_core_3_2.h"
#else
//this'll need to be changed on windows and ios
#include <OpenGL/gl3.h>
#endif

#endif
