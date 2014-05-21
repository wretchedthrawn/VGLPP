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

#ifndef __VGLPP__System__
#define __VGLPP__System__

#include <functional>
#include <memory>
#include <list>
#include "VecTypes.h"

namespace vgl
{
  typedef std::function<void()> Task;
  
  class StateMachine;
  
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

    ///returns access to the current state machine
    virtual std::shared_ptr<StateMachine> currentStateMachine();

    //Other methods----------------------------------------------------
    
    ///Should be called whenever the screen resizes
    void backingResized();

    int backingWidth();
    int backingHeight();
    
    //This task stuff was put together to get me out of a bind, it's far from perfect
    
    ///Schedule a task to occur on the main thread
    ///currently there is no backbone mechanism to drain these
    void scheduleTask(Task t);
    
    ///Runs and drains the scheduled tasks queue for the main thread
    void runScheduledTasks();
    
  private:
    std::shared_ptr<StateMachine> stateMachine;
    
    std::list<Task> scheduledTasks;
    
  };
}
#endif /* defined(__VGLPP_Demo__System__) */
