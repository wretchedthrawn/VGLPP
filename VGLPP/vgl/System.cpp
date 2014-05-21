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
#include <stdexcept>
#include "System.h"
#include "StateMachine.h"

namespace vgl
{
  static System *currentInstance = nullptr;
  
  System &System::system()
  {
    if(!currentInstance)
    {
      throw std::runtime_error("You must subclass vgl::System, implement the required methods, and declare at least one instance of it in your application.");
    }
    return *currentInstance;
  }
  
  System::System()
  {
    currentInstance = this;
  }
  
  void System::makeGLContextCurrent()
  {
    currentInstance = this;
  }
 
  int System::backingWidth()
  {
    return dimensionsForBackingStore().x;
  }
  
  int System::backingHeight()
  {
    return dimensionsForBackingStore().y;
  }
  
  shared_ptr<StateMachine> System::currentStateMachine()
  {
    if(!stateMachine)
    {
      stateMachine = shared_ptr<StateMachine>(new StateMachine);
    }
        
    return stateMachine;
  }
  
  void System::scheduleTask(Task t)
  {
    scheduledTasks.push_back(t);
  }
  
  void System::runScheduledTasks()
  {
    for(auto t : scheduledTasks)
      t();
    scheduledTasks.clear();
  }
}