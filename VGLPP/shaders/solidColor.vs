
#ifdef GL_ES
//precision mediump float;
#else
#define highp
#define lowp
#define mediump
#endif

#if (defined GL_ES && __VERSION__ < 300)
attribute vec4 position;
//attribute vec4 solidColor;
varying vec4 varColor;
#else
in vec4 position;
//in vec4 solidColor;
flat out vec4 varColor;
#endif

uniform mat4 modelViewProjectionMatrix;
uniform float pointSize;
uniform vec4 solidColor;

void main()
{ 
  varColor = solidColor;
  gl_PointSize = pointSize;
  gl_Position = modelViewProjectionMatrix * position;
}

