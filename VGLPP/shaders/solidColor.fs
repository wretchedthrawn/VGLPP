
#ifdef GL_ES
precision lowp float;
#else
#define highp
#define lowp
#define mediump
#endif

#if (defined GL_ES && __VERSION__ < 300)
varying vec4 varColor;
#define fragColor gl_FragColor
#else
flat in vec4 varColor;
out vec4 fragColor;
#endif

void main()
{
	fragColor = varColor;
}


