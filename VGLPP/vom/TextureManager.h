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

#ifndef __VGLPP_Demo__TextureManager__
#define __VGLPP_Demo__TextureManager__

#include <memory>
#include <string>
#include <map>

namespace vom
{
  class Texture2D;
  
  class TextureManager
  {
  public:
    TextureManager();
    ~TextureManager();
    
    ///Manages a new texture
    void setTexture(const std::string &texName, std::shared_ptr<Texture2D> tex);
    
    ///Retrieves a previously managed texture by its name
    std::shared_ptr<Texture2D> getTexture(const std::string &texName);

    ///Removes a previously managed texture by its name
    void removeTexture(const std::string &texName);

  private:
    std::map<std::string, std::shared_ptr<Texture2D>> textures;
  };
}

#endif /* defined(__VGLPP_Demo__TextureManager__) */
