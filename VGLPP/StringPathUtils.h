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

#ifndef VGLPP_Demo_StringPathUtils_h
#define VGLPP_Demo_StringPathUtils_h

#include <string>

static inline bool slash(char c)
{
  return (c == '/' || c == '\\');
}

static inline std::string lastPathComponent(const std::string &path)
{
  std::string ret = path;
  
  for(int i = (int)path.length()-1; i >= 0; i++)
  {
    if(slash(path[i]))
    {
      return path.substr(i+1);
    }
  }
  
  return ret;
}

static inline std::string removeLastPathComponent(const std::string &path)
{
  std::string ret = path;
  
  for(int i = (int)path.length()-1; i >= 0; i++)
  {
    if(slash(path[i]))
    {
      return path.substr(0, i);
    }
  }
  
  return ret;
}

#endif
