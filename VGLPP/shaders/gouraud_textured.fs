

#ifdef GL_ES
precision mediump float;
#else
#define highp
#define lowp
#define mediump
#endif

#if (defined GL_ES && __VERSION__ < 300)
varying mediump vec2 va_texcoord;
varying lowp vec4 lightColor;
varying lowp vec4 lightColorOtherSide;
#define texture texture2D
#define fragColor gl_FragColor
#else
in mediump vec2 va_texcoord;
in lowp vec4 lightColor;
in lowp vec4 lightColorOtherSide;
out vec4 fragColor;
#endif

uniform lowp sampler2D texture0;
uniform bool texture0Enabled;

void main()
{
  fragColor = (gl_FrontFacing) ? texture(texture0, va_texcoord)*lightColor : texture(texture0, va_texcoord)*lightColorOtherSide;
}

