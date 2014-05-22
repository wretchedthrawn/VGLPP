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
#include "SDL_image.h"
#include "SDLTextureLoader.h"
#include "Texture2D.h"

using namespace std;

//This is pretty limited currently, SDL texture loading has always been a giant pain

namespace vom
{
  static int nextP2(int val)
  {
    val--;
    val = (val >> 1) | val;
    val = (val >> 2) | val;
    val = (val >> 4) | val;
    val = (val >> 8) | val;
    val = (val >> 16) | val;
    val++;
    
    return val;
  }
  
  static void flip(SDL_Surface *surf, unsigned char *pixels, int pSz)
  {
    const int w = surf->w, h = surf->h;
    for(int i = 0; i < h; i++)
    {
      memcpy(pixels+(h-1-i)*w*pSz, ((unsigned char *)surf->pixels)+(i*w*pSz), w*pSz);
    }
  }

  void SDLTextureLoader::loadTexture(Texture2D &texture, const std::string &filename)
  {
    SDL_Surface *surf = IMG_Load(filename.c_str());
    if(!surf)
    {
      cerr << "unable to load texture: " << filename << endl;
      return;
    }
    
    texture.setImageSize(make_uint2(surf->w, surf->h));
    
    bool alpha = false, mipmapping = false, nonp2 = true;
    GLenum format = GL_RGBA;
    int nOfColors = surf->format->BytesPerPixel;
    if(nOfColors == 4)     // contains an alpha channel
    {
      if(surf->format->Rmask == 0x000000ff)
        format = GL_RGBA;
      else
        format = GL_BGRA;
      alpha = true;
    }
    else if(nOfColors == 3)     // no alpha channel
    {
      if(surf->format->Rmask == 0x000000ff)
        format = GL_RGB;
      else
        format = GL_BGR;
      alpha = false;
    }
    else
    {
      cerr << "Bad image format: " << filename << endl;
      return;
    }
    
    glBindTexture(GL_TEXTURE_2D, texture.getName());
    
    int width = surf->w, height = surf->h;
    int texWidth = surf->w, texHeight = surf->h;
    if(!nonp2)
    {
      texWidth = nextP2(texWidth);
      texHeight = nextP2(texWidth);
    }
    texture.setSize(make_uint2(texWidth, texHeight));
    
    //flip manually
    if(SDL_MUSTLOCK(surf))
      SDL_LockSurface(surf);
    void *pixels = malloc(nOfColors*width*height);
    flip(surf, (unsigned char *)pixels, nOfColors);
    memcpy(surf->pixels, pixels, nOfColors*width*height);
    //free(surf->pixels);
    //surf->pixels = pixels;
    if(SDL_MUSTLOCK(surf))
      SDL_UnlockSurface(surf);
    
    if(texWidth != width || texHeight != height)
    {
      //Textures should be powers of two, no choice but to waste here
      SDL_Surface *texSurf = SDL_CreateRGBSurface(SDL_SWSURFACE, texWidth, texHeight, nOfColors*8,
                                                  surf->format->Rmask, surf->format->Gmask, surf->format->Bmask, surf->format->Amask);
      SDL_SetSurfaceBlendMode(surf, SDL_BLENDMODE_NONE);
      SDL_SetSurfaceBlendMode(texSurf, SDL_BLENDMODE_NONE);
      
      SDL_FillRect(texSurf, NULL, 0);
      SDL_Rect rect = { 0, 0, width, height };
      SDL_BlitSurface(surf, NULL, texSurf, &rect);
      glTexImage2D(GL_TEXTURE_2D, 0, nOfColors, texWidth, texHeight, 0,
                   format, GL_UNSIGNED_BYTE, texSurf->pixels);
      SDL_FreeSurface(texSurf);
    }
    else
    {
      glTexImage2D(GL_TEXTURE_2D, 0, nOfColors, texWidth, texHeight, 0,
                   format, GL_UNSIGNED_BYTE, surf->pixels);
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    if(!mipmapping)
    {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
      glGenerateMipmap(GL_TEXTURE_2D);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    
    SDL_FreeSurface(surf);    
  }
}