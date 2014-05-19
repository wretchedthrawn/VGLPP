

#ifdef GL_ES
precision mediump float;
#else
#define highp
#define lowp
#define mediump
#endif

#if (defined GL_ES && __VERSION__ < 300)
varying lowp vec4 lightColor;
varying lowp vec4 lightColorOtherSide;
#define texture texture2D
#define fragColor gl_FragColor
#else
in lowp vec4 lightColor;
in lowp vec4 lightColorOtherSide;
out vec4 fragColor;
#endif

void main()
{
  fragColor = (gl_FrontFacing) ? lightColor : lightColorOtherSide;
}

