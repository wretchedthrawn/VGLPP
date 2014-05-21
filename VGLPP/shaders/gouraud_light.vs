
//Material colors
//struct Material
//{
//  vec4 ambient, diffuse, specular, emissive;
//  float shininess;
//};
//uniform Material material;
//uniform vec4 lightModelProductSceneColor;
//
//vec4 ambient;
//vec4 diffuse;
//vec4 specular;
//
////Prototypes
//void pointLight(in int i, in vec3 normal, in vec3 eye, in vec3 ecPosition3);
//void directionalLight(in int i, in vec3 normal, in vec3 ec_pos);
//void infiniteSpotLight(in int i, in vec3 normal, in vec3 ec_pos);
//void spotLight(in int i, in vec3 normal, in vec3 eye, in vec3 ecPosition3);

vec4 computeLight(in mediump vec3 normal, in mediump vec4 ecPosition, in lowp float alphaFade, out lowp vec4 otherSideColor)
{
  lowp vec4 color, ret;
  mediump vec3 eye;
  mediump vec3 ecPosition3 = (vec3(ecPosition)) / ecPosition.w;
  
  //note:  may need to pass in camera pos at some point (only if local viewer, so not really)
  eye = vec3(0.0, 0.0, 1.0);
  
  // Clear the light intensity accumulators
  ambient  = vec4(0.0);
  diffuse  = vec4(0.0);
  specular = vec4(0.0);
  
  //directionalLight(0, normal);
  //pointLight(0, normal, eye, ecPosition3);
  
  //final surface color computation
  color = (lightModelProductSceneColor) + ambient + specular + diffuse;
  otherSideColor = (lightModelProductSceneColor) + ambient + specularBack + diffuseBack;
  
  color = clamp(color, 0.0, 1.0);
  ret = color;
  
  //ret.a *= alphaFade;
  //secondaryHighlight.a *= alphaFade;
  return ret;
}

