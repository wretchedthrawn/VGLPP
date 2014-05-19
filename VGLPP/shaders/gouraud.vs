
//base effect fixed functionality emulation
//Reference:  http://code.google.com/p/gles2-bc/source/browse/trunk/trunk/Sources/OpenGLES/OpenGLES20/shaders/main.vert?r=2

#ifdef GL_ES
//precision mediump float;
#else
#define highp 
#define lowp
#define mediump
#endif

#if (defined GL_ES && __VERSION__ < 300)
attribute highp vec4 position;
attribute mediump vec3 normal;
attribute mediump vec2 texcoord0;
#ifdef TEXTURE
varying mediump vec2 va_texcoord;
#endif
varying lowp vec4 lightColor;
varying lowp vec4 lightColorOtherSide;
#else
in highp vec4 position;
in mediump vec3 normal;
in mediump vec2 texcoord0;

#ifdef TEXTURE
out mediump vec2 va_texcoord;
#endif
out lowp vec4 lightColor;
out lowp vec4 lightColorOtherSide;
#endif

//Prototypes
vec4 computeLight(in mediump vec3 normal, in mediump vec4 ecPosition, in lowp float alphaFade, out lowp vec4 otherSideColor);

//Material colors
struct Material
{
  //lowp vec4 ambient, diffuse, specular, emissive;
  lowp float alpha;
  mediump float shininess;
};
uniform Material material;

//Lights
struct Light
{
  mediump vec4 position;
  lowp vec4 ambient, diffuse, specular;
  mediump vec3 spotDirection;
  float spotExponent, spotCosCutoff;
  float constantAttenuation, linearAttenuation, quadraticAttenuation;
  
  //bool doAtten;
  //bool doSpec;
  //mat4 transform;
};
uniform Light lights[8];
uniform vec4 lightModelProductSceneColor;
//uniform bool lightingEnabled;

lowp vec4 ambient;
lowp vec4 diffuse, diffuseBack = vec4(0.0);
lowp vec4 specular, specularBack = vec4(0.0);

uniform vec4 solidColor;
uniform mat4 modelViewProjectionMatrix;
uniform mediump mat4 modelViewMatrix;
//uniform mat4 textureMatrix;
uniform mediump vec2 textureScale;
uniform mediump mat3 normalMatrix;

uniform bool doNormalize;

/*void pointLight0(in mediump vec3 normal, in mediump vec3 eye, in mediump vec3 ecPosition3)
{
  mediump float nDotVP;       // normal . light direction
  mediump float nDotHV;       // normal . light half vector
  //mediump float pf = 0.0;           // power factor
  mediump float attenuation = 1.0;  // computed attenuation factor
  mediump float d;            // distance from surface to light source
  mediump vec3  VP;           // direction from surface to light position
  mediump vec3  halfVector;   // direction of maximum highlights
  
  // Compute vector from surface to light position
  VP = vec3(lights[0].position) - ecPosition3;
  
#ifdef ATTENUATION
  // Compute distance between surface and light position
  d = length(VP);
#endif
  
  // Normalize the vector from surface to light position
  VP = normalize(VP);
  
  // Compute attenuation
#ifdef ATTENUATION
  {
    attenuation = 1.0 / (lights[0].constantAttenuation +
                         lights[0].linearAttenuation * d +
                         lights[0].quadraticAttenuation * d * d);
  }
#endif
  
  nDotVP = dot(normal, VP);
  mediump vec2 frontAndBack = vec2(nDotVP, -nDotVP);
  frontAndBack = max(vec2(0.0), frontAndBack);
  
  //  if(nDotVP > 0.0)
  {
    diffuse  += lights[0].diffuse * frontAndBack.x * attenuation;
    diffuseBack += lights[0].diffuse * frontAndBack.y * attenuation;
  }
  
  //if(lights[0].doSpec)
  {
    mediump vec2 cutOff = step(frontAndBack, vec2(0.0));
    
    halfVector = normalize(VP + eye);
    nDotHV = dot(normal, halfVector);
    frontAndBack = vec2(nDotHV, -nDotHV);
    frontAndBack = max(vec2(0.0), frontAndBack);
    
    lowp vec2 pf = pow(frontAndBack, vec2(material.shininess, material.shininess));
    specular += lights[0].specular * pf.x * attenuation * cutOff.y;
    specularBack += lights[0].specular * pf.y * attenuation * cutOff.x;
  }
  
  ambient += lights[0].ambient * attenuation;
}

void directionalLight0(in mediump vec3 normal, in mediump vec3 eye)
{
  mediump float nDotVP;         // normal . light direction
  mediump float nDotHV;         // normal . light half vector
  mediump vec3 lightDir = normalize(vec3(lights[0].position));
  
  nDotVP = dot(normal, lightDir);
  mediump vec2 frontAndBack = vec2(nDotVP, -nDotVP);
  frontAndBack = max(vec2(0.0), frontAndBack);

  vec3 halfVector = normalize(lightDir + eye);
  nDotHV = dot(normal, halfVector);
  
  diffuse  += lights[0].diffuse * frontAndBack.x;
  diffuseBack += lights[0].diffuse * frontAndBack.y;

  mediump vec2 cutOff = step(frontAndBack, vec2(0.0));
  
  frontAndBack = vec2(nDotHV, -nDotHV);
  frontAndBack = max(vec2(0.0), frontAndBack);
  
  lowp vec2 pf = pow(frontAndBack, vec2(material.shininess, material.shininess));
  specular += lights[0].specular * pf.x * cutOff.y;
  specularBack += lights[0].specular * pf.y * cutOff.x;
  
  ambient += lights[0].ambient;
}

 */

void infiniteSpotLight(const in int i, in vec3 normal, in vec3 eye) //in vec3 ec_pos)
{
  float nDotVP;         // normal . light direction
  float nDotHV;         // normal . light half vector
  float pf;             // power factor
  float spotAttenuation;
  vec3  Ppli;
  vec3  Sdli;
  vec3 lightDir = normalize(vec3(lights[i].position));

  nDotVP = max(0.0, dot(normal, normalize(vec3(lights[i].position))));
  //vec3 halfVector = normalize(lightDir - ec_pos);
  vec3 halfVector = normalize(eye + lightDir);  //FF precomputes this..
  nDotHV = max(0.0, dot(normal, halfVector));
  
  Ppli = -normalize(vec3(lights[i].position));
  Sdli = normalize(vec3(lights[i].spotDirection));
  
  spotAttenuation = pow(dot(Ppli, Sdli), lights[i].spotExponent);
  if(nDotVP == 0.0)
  {
    pf = 0.0;
  }
  else
  {
    pf = pow(nDotHV, material.shininess);
  }
  
  ambient  += lights[i].ambient * spotAttenuation;
  diffuse  += lights[i].diffuse * nDotVP * spotAttenuation;
  specular += lights[i].specular * pf * spotAttenuation;
}

void spotLight(const in int i, in vec3 normal, in vec3 eye, in vec3 ecPosition3)
{
  float nDotVP;            // normal . light direction
  float nDotHV;            // normal . light half vector
  float spotDot;           // cosine of angle between spotlight
  float spotAttenuation;   // spotlight attenuation factor
  float attenuation = 1.0;       // computed attenuation factor
  float d;                 // distance from surface to light source
  vec3  VP;                // direction from surface to light position
  vec3  halfVector;        // direction of maximum highlights
  
  // Compute vector from surface to light position
  VP = vec3(lights[i].position) - ecPosition3;
  
  // Compute distance between surface and light position
  d = length(VP);
  
  // Normalize the vector from surface to light position
  VP = normalize(VP);
  
  // Compute attenuation
#ifdef ATTENUATION
  attenuation = 1.0 / (lights[i].constantAttenuation +
                       lights[i].linearAttenuation * d +
                       lights[i].quadraticAttenuation * d * d);
#endif
  
  // See if point on surface is inside cone of illumination
  spotDot = dot(-VP, normalize(lights[i].spotDirection));
  
  if(spotDot < lights[i].spotCosCutoff)
  {
    spotAttenuation = 0.0; // light adds no contribution
  }
  else
  {
    spotAttenuation = pow(spotDot, lights[i].spotExponent);
  }
  
  // Combine the spotlight and distance attenuation.
  attenuation *= spotAttenuation;
  
  nDotVP = dot(normal, VP);
  mediump vec2 frontAndBack = vec2(nDotVP, -nDotVP);
  frontAndBack = max(vec2(0.0), frontAndBack);
  
  diffuse  += lights[i].diffuse * frontAndBack.x * attenuation;
  diffuseBack += lights[i].diffuse * frontAndBack.y * attenuation;

  mediump vec2 cutOff = step(frontAndBack, vec2(0.0));
  
  halfVector = normalize(VP + eye);
  nDotHV = dot(normal, halfVector);
  frontAndBack = vec2(nDotHV, -nDotHV);
  frontAndBack = max(vec2(0.0), frontAndBack);
  
  lowp vec2 pf = pow(frontAndBack, vec2(material.shininess, material.shininess));
  specular += lights[i].specular * pf.x * attenuation * cutOff.y;
  specularBack += lights[i].specular * pf.y * attenuation * cutOff.x;

  ambient += lights[i].ambient * attenuation;
}

void main()
{
  vec4 highlight = vec4(0.0);
  
#ifdef TEXTURE
  //Pass transformed texcoord.
  va_texcoord = texcoord0*textureScale;
#endif
  
  //Compute transformed normal
  mediump vec3 eyeNormal = normalize(normalMatrix * normal);
  
  //Compute vert position in eye coordinates
  mediump vec4 ec_pos = modelViewMatrix * position;

  vec4 color = computeLight(eyeNormal, ec_pos, 1.0, lightColorOtherSide);
  color.a = material.alpha;
  lightColor = color;
  
  //Pass GL-transformed to vertex down pipeline
  gl_Position = modelViewProjectionMatrix * position;
}

