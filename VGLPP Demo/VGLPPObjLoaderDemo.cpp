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

#include <iostream>
#include "VGLPPObjLoaderDemo.h"
#include "VecTypes.h"
#define GL3_PROTOTYPES 1
#include <OpenGL/gl3.h>

using namespace std;

VGLPPObjLoaderDemo::VGLPPObjLoaderDemo()
{
  context = NULL;
  int mode = 0;
  int flags[] = { 0, SDL_WINDOW_FULLSCREEN, SDL_WINDOW_FULLSCREEN_DESKTOP };
  
  SDL_Init(SDL_INIT_VIDEO);
  
  srandom((int)time(NULL));
  
  //OpenGL init params, we want a 3.2 core profile here
  //NOTE that only the absolute latest version of SDL2 will actually grant this 3.2 profile on mac
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_STEREO, 0);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  
  //Create an application window with the following settings:
  window = SDL_CreateWindow("VGL++ OBJ Loading Demo",          // window title
                            SDL_WINDOWPOS_UNDEFINED,           // initial x position
                            SDL_WINDOWPOS_UNDEFINED,           // initial y position
                            640,                               // width, in pixels
                            480,                               // height, in pixels
                            flags[mode] | SDL_WINDOW_OPENGL);                      // flags
  
  /* Create OpenGL context */
  context = SDL_GL_CreateContext(window);
  if(!context)
  {
    cerr <<  "Couldn't create OpenGL context: " << SDL_GetError() << endl;
    exit(2);
  }
  
  cout << "Created OpenGL " << glGetString(GL_VERSION) << " context" << endl;
  
  SDL_GetWindowSize(window, &screenW, &screenH);
  SDL_GL_SetSwapInterval(1);
    
  //create and retrieve the vgl state machine
  vgl = currentStateMachine();
}

void VGLPPObjLoaderDemo::run()
{
  bool done = false;
  
  //Very simple event loop
  while(!done)
  {
    //Check for events
    done = processEvents();
    
    //Render display
    SDL_GL_MakeCurrent(window, context);
    doRender();
    SDL_GL_SwapWindow(window);
    
    //Drain out any scheduled vgl system tasks (super class method)
    runScheduledTasks();
  }
}

bool VGLPPObjLoaderDemo::processEvents()
{
  SDL_Event event;
  bool done = false;
  
  while(SDL_PollEvent(&event))
  {
    switch(event.type)
    {
      case SDL_WINDOWEVENT_CLOSE:
      {
        if(window)
        {
          SDL_DestroyWindow(window);
          window = NULL;
          done = true;
        }
      }
      break;
      case SDL_KEYDOWN:
        //non-game keys
        switch(event.key.keysym.sym)
        {
          case SDLK_ESCAPE:
            done = true;
          break;
        }
      break;
      case SDL_QUIT:
        //quit out of the program
        done = true;
      break;
    }
  }
  
  return done;
}

void VGLPPObjLoaderDemo::doRender()
{
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

uint2 VGLPPObjLoaderDemo::dimensionsForBackingStore()
{
  return make_uint2(screenW, screenH);
}

bool VGLPPObjLoaderDemo::isGLContextReady()
{
  return true;
}

void VGLPPObjLoaderDemo::bindDefaultFramebuffer()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


VGLPPObjLoaderDemo::~VGLPPObjLoaderDemo()
{
  vgl.reset();
  
  //close and destroy the window
  SDL_DestroyWindow(window);
  
  //clean up
  SDL_Quit();
}

