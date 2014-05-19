

#ifdef GL_ES
precision mediump float;
#else
#define highp
#define lowp
#define mediump
#endif

#if (defined GL_ES && __VERSION__ < 300)
#ifdef TEXTURE
varying mediump vec2 va_texcoord;
#endif
varying mediump vec3 va_normal;
varying mediump vec4 ec_pos;
#define texture texture2D
#define fragColor gl_FragColor
#else
#ifdef TEXTURE
in mediump vec2 va_texcoord;
#endif
in mediump vec3 va_normal;
in mediump vec4 ec_pos;
out vec4 fragColor;
#endif

//Prototypes
vec4 computeLight(in mediump vec3 normal, in mediump vec4 ecPosition, in lowp float alphaFade, out lowp vec4 otherSideColor, out lowp vec4 secondaryHighlight);

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

lowp vec4 lightColor;
lowp vec4 lightColorOtherSide;

#ifdef TEXTURE
uniform lowp sampler2D texture0;
#endif

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
  vec4 color = texture(texture0, va_texcoord);
#else
  vec4 color = vec4(1.0);
#endif
  
  vec4 lightColor = computeLight(normalize(va_normal), ec_pos, 1.0, lightColorOtherSide, highlight);
  if(!gl_FrontFacing)
    lightColor = lightColorOtherSide;
  
  color *= lightColor;
  color.a = material.alpha;
  color += highlight;
  
  //color = clamp(color, 0.0, 1.0);
  
	fragColor = color;
}

