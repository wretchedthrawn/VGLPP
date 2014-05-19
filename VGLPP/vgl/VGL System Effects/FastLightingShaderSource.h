//
//  VGLPP
//
//  This code comes at absolutely NO WARRANTY.
//  The author accepts no liability whatsoever for any effects,
//  adverse or otherwise, that may arise as a result of using this software.
//  By using this code, or any derivitive works thereof, you
//  are stating that you have read and accept these terms, and that you are
//  using this software at YOUR OWN RISK.
//

#ifndef VGL_VertoGLFastLightingShader_h
#define VGL_VertoGLFastLightingShader_h

//oh god please no (no way to get "constant-mode" access) to uniform arrays short of
//actually templatizing these functions
static const char *pointLight = "void pointLight0(in mediump vec3 normal, in mediump vec3 eye, in mediump vec3 ecPosition3)\n"
"{\n"
"  mediump float nDotVP;       // normal . light direction\n"
"  mediump float nDotHV;       // normal . light half vector\n"
"  //mediump float pf = 0.0;           // power factor\n"
"  mediump float attenuation = 1.0;  // computed attenuation factor\n"
"  mediump float d;            // distance from surface to light source\n"
"  mediump vec3  VP;           // direction from surface to light position\n"
"  mediump vec3  halfVector;   // direction of maximum highlights\n"
"  \n"
"  // Compute vector from surface to light position\n"
"  VP = vec3(lights[0].position) - ecPosition3;\n"
"  \n"
"#ifdef ATTENUATION\n"
"  // Compute distance between surface and light position\n"
"  d = length(VP);\n"
"#endif\n"
"  \n"
"  // Normalize the vector from surface to light position\n"
"  VP = normalize(VP);\n"
"  \n"
"  // Compute attenuation\n"
"#ifdef ATTENUATION\n"
"  {\n"
"    attenuation = 1.0 / (lights[0].constantAttenuation +\n"
"                         lights[0].linearAttenuation * d +\n"
"                         lights[0].quadraticAttenuation * d * d);\n"
"  }\n"
"#endif\n"
"  \n"
"  nDotVP = dot(normal, VP);\n"
"  mediump vec2 frontAndBack = vec2(nDotVP, -nDotVP);\n"
"  frontAndBack = max(vec2(0.0), frontAndBack);\n"
"  \n"
"  //  if(nDotVP > 0.0)\n"
"  {\n"
"    diffuse  += lights[0].diffuse * frontAndBack.x * attenuation;\n"
"    diffuseBack += lights[0].diffuse * frontAndBack.y * attenuation;\n"
"  }\n"
"  \n"
"  //if(lights[0].doSpec)\n"
"  {\n"
"    mediump vec2 cutOff = step(frontAndBack, vec2(0.0));\n"
"    \n"
"    halfVector = normalize(VP + eye);\n"
"    nDotHV = dot(normal, halfVector);\n"
"    frontAndBack = vec2(nDotHV, -nDotHV);\n"
"    frontAndBack = max(vec2(0.0), frontAndBack);\n"
"    \n"
"    lowp vec2 pf = pow(frontAndBack, vec2(material.shininess, material.shininess));\n"
"    specular += lights[0].specular * pf.x * attenuation * cutOff.y;\n"
"    specularBack += lights[0].specular * pf.y * attenuation * cutOff.x;\n"
"  }\n"
"  \n"
"  ambient += lights[0].ambient * attenuation;\n"
"}\n\n";

static const char *directionalLight = "void directionalLight0(in mediump vec3 normal, in mediump vec3 eye)\n"
"{\n"
"  mediump float nDotVP;         // normal . light direction\n"
"  mediump float nDotHV;         // normal . light half vector\n"
"  mediump vec3 lightDir = normalize(vec3(lights[0].position));\n"
"  \n"
"  nDotVP = dot(normal, lightDir);\n"
"  mediump vec2 frontAndBack = vec2(nDotVP, -nDotVP);\n"
"  frontAndBack = max(vec2(0.0), frontAndBack);\n"
"\n"
"  vec3 halfVector = normalize(lightDir + eye);\n"
"  nDotHV = dot(normal, halfVector);\n"
"  \n"
"  diffuse  += lights[0].diffuse * frontAndBack.x;\n"
"  diffuseBack += lights[0].diffuse * frontAndBack.y;\n"
"\n"
"  mediump vec2 cutOff = step(frontAndBack, vec2(0.0));\n"
"  \n"
"  frontAndBack = vec2(nDotHV, -nDotHV);\n"
"  frontAndBack = max(vec2(0.0), frontAndBack);\n"
"  \n"
"  lowp vec2 pf = pow(frontAndBack, vec2(material.shininess, material.shininess));\n"
"  specular += lights[0].specular * pf.x * cutOff.y;\n"
"  specularBack += lights[0].specular * pf.y * cutOff.x;\n"
"  \n"
"  ambient += lights[0].ambient;\n"
"}\n\n";

#endif
