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

#include "TextureManager.h"
#include "Texture2D.h"

using namespace std;

namespace vom
{
  TextureManager::TextureManager()
  {
    
  }
  
  TextureManager::~TextureManager()
  {
    //nothing to clean up, gotta love c++11 smart pointers
  }
  
  void TextureManager::setTexture(const string &texName, shared_ptr<Texture2D> tex)
  {
    textures[texName] = tex;
  }
  
  shared_ptr<Texture2D> TextureManager::getTexture(const string &texName)
  {
    shared_ptr<Texture2D> ret = nullptr;
    auto iter = textures.find(texName);
    
    if(iter != textures.end())
      ret = iter->second;
      
    return ret;
  }
 
  void TextureManager::removeTexture(const string &texName)
  {
    textures.erase(texName);
  }
  
}

