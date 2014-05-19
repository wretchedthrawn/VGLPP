
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
varying mediump vec3 va_normal;
varying mediump vec4 ec_pos;
#else
in highp vec4 position;
in mediump vec3 normal;
in mediump vec2 texcoord0;
#ifdef TEXTURE
out mediump vec2 va_texcoord;
#endif
out mediump vec3 va_normal;
out mediump vec4 ec_pos;
#endif

uniform mat4 modelViewProjectionMatrix;
uniform mediump mat4 modelViewMatrix;
uniform mediump vec2 textureScale;
uniform mediump mat3 normalMatrix;


void main()
{
#ifdef TEXTURE
  //Pass transformed texcoord.
  va_texcoord = texcoord0*textureScale;
#endif
  
  //Compute transformed normal
  vec3 eyeNormal = normalize(normalMatrix * normal);
  va_normal = eyeNormal;
  
  //Compute vert position in eye coordinates
  ec_pos = (modelViewMatrix * position);
  
  //Pass GL-trasnformed to vertex down pipeline
  gl_Position = modelViewProjectionMatrix * position;
}

