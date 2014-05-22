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

#include <stdexcept>
#include "Texture2D.h"
#include "TextureLoader.h"

using namespace std;

namespace vom
{
  std::shared_ptr<TextureLoader> Texture2D::textureLoader;

  Texture2D::Texture2D(const string &filename) : filename(filename)
  {
    imageSize = make_uint2(0, 0);
    size = make_uint2(0, 0);
    glGenTextures(1, &name);
    
    if(!textureLoader)
      throw std::runtime_error("A texture loader must be set before you can load textures via Texture2D class!");
    textureLoader->loadTexture(*this, filename);
  }
  
  void Texture2D::setTextureLoader(std::shared_ptr<TextureLoader> tl)
  {
    textureLoader = tl;
  }
  
  Texture2D::~Texture2D()
  {
    glDeleteTextures(1, &name);
  }
}