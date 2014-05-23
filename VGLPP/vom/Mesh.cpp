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

#include "Mesh.h"

namespace vom
{
  Mesh::Mesh()
  {
    
  }
  
  Mesh::~Mesh()
  {
    if(ind_vbo)
      glDeleteBuffers(1, &ind_vbo);
    if(ind_vbo_quad)
      glDeleteBuffers(1, &ind_vbo_quad);
  }
  
  void Mesh::setMaterial(std::shared_ptr<Material> mat)
  {
    material = mat;
  }
}