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
#include <fstream>
#include <memory>
#include <stdexcept>
#include "StringPathUtils.h"
#include "MaterialManager.h"
#include "Material.h"
#include "TextureManager.h"
#include "Texture2D.h"

using namespace std;

namespace vom
{
  MaterialManager::MaterialManager()
  {
    
  }
  
  MaterialManager::~MaterialManager()
  {
    //nothing to clean up, gotta love c++11 smart pointers
  }
  
  void MaterialManager::setMaterial(const string &matName, shared_ptr<Material> mat)
  {
    materials[matName] = mat;
  }
  
  shared_ptr<Material> MaterialManager::getMaterial(const string &matName)
  {
    shared_ptr<Material> ret = nullptr;
    auto iter = materials.find(matName);
    
    if(iter != materials.end())
      ret = iter->second;
      
    return ret;
  }
 
  void MaterialManager::removeMaterial(const string &matName)
  {
    materials.erase(matName);
  }

  void MaterialManager::readMtl(const string &mtlFilename, shared_ptr<TextureManager> textureManager)
  {
    ifstream in(mtlFilename);
    
    if(!in.is_open())
    {
      cerr << "No material file exists for " << mtlFilename << endl;
      return;
    }

    string baseName = removeLastPathComponent(mtlFilename);
    float4 diff = make_float4(0.8f, 0.8f, 0.8f, 1.0f), amb, spec = make_float4(0, 0, 0, 0);
    float shininess = 64;
    shared_ptr<Texture2D> tex;
    string name, token;
    
    while(in.good())
    {
      token = "";
      in >> token;
      
      if(token == "newmtl")
      {
        if(!tex && !name.empty())
        {
          //last material had no texture
          shared_ptr<Material> m = shared_ptr<Material>(new Material);
          m->setDiffuse(diff);
          m->setAmbient(amb);
          m->setSpecular(spec);
          m->setShininess(shininess);
          m->setName(name);
          setMaterial(name, m);
        }
        
        //free texture to avoid issues with badly formatted mtl files
        tex = nullptr;
        in >> name;
        
        //object already exists, skip to next or EOF
        if(materials.find(name) != materials.end())
        {
          do
          {
            in >> token;
          } while(in.good() && token != "newmtl");
          if(in.good())
          {
            cerr << in.peek();
            
            //this should never happen
            if(in.peek() != 'n')
              throw std::runtime_error("Internal error in readMtl()");
          }
          else
          {
            in.close();
            return;
          }
        }
      }
      else if(token == "Kd")  //diffuse color
      {
        in >> diff.x >> diff.y >> diff.z;
        
        //not sure why but .obj Kd colors from blender seem to be premultipled by 0.8
        diff.x /= 0.8;
        diff.y /= 0.8;
        diff.z /= 0.8;
      }
      else if(token == "Ka")  //ambient color
      {
        in >> amb.x >> amb.y >> amb.z;
        
        //not sure why but .obj Kd colors from blender seem to be premultipled by 0.8
        amb.x += 0.2;
        amb.y += 0.2;
        amb.z += 0.2;
        amb.w = 0.0;
      }
      else if(token == "Ks") //specular color
      {
        in >> spec.x >> spec.y >> spec.z;
      }
      else if(token == "Ns")
      {
        in >> shininess;
        shininess = shininess/1000.0f*128.0f;
      }
      else if(token == "d")  //opacity
      {
        in >> diff.w;
        
        //0 is probably a bug in the exporter and will confuse the user, ignore it
        if(diff.w == 0.0f)
          diff.w = 1.0f;
      }
      else if(token == "map_Kd")  //texture map
      {
        string tex_fn;
        while(in.good() && isspace(in.peek()))
          in.get();
        while(in.good() && in.peek() != '\r' && in.peek() != '\n')
          tex_fn += in.get();
        
        tex = shared_ptr<Texture2D>(new Texture2D(baseName + "/" + tex_fn));
        textureManager->setTexture(lastPathComponent(tex_fn), tex);
        
        shared_ptr<Material> m = shared_ptr<Material>(new Material);
        m->setDiffuse(diff);
        m->setAmbient(amb);
        m->setSpecular(spec);
        m->setShininess(shininess);
        m->setName(name);
        m->setTexture(tex);
        setMaterial(name, m);
        
        name = "";
        tex = nullptr;
      }
      else
      {
        while(in.good() && in.get() != '\n') ;
      }
    }
    
    if(!tex && !name.empty())
    {
      //last material had no texture
      shared_ptr<Material> m = shared_ptr<Material>(new Material);
      m->setDiffuse(diff);
      m->setAmbient(amb);
      m->setSpecular(spec);
      m->setShininess(shininess);
      m->setName(name);
      setMaterial(name, m);
    }
    
    in.close();
  }
}

