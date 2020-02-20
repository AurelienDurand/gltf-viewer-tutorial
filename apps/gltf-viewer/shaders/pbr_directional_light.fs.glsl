#version 330

in vec3 vViewSpacePosition;
in vec3 vViewSpaceNormal;
in vec2 vTexCoords;

//uniform vec3 uLightDirection;
//uniform vec3 uLightIntensity;

uniform vec3 uWi; // ligth direction
uniform vec3 uLi; // Ligth intensity

uniform sampler2D uBaseColorTexture;
uniform vec4 uBaseColorFactor;

uniform float uMetallicFactor;
uniform float uRougnessFactor;
uniform sampler2D uMetallicRoughnessTextur;
//uniform vec4 uBaseColorFactor ;
//uniform sampler2D uBaseColor ; //uBaseColorTexture

out vec3 fColor;

// Constants
const float GAMMA = 2.2;
const float INV_GAMMA = 1. / GAMMA;
const float M_PI = 3.141592653589793;
const float M_1_PI = 1.0 / M_PI;

// We need some simple tone mapping functions
// Basic gamma = 2.2 implementation
// Stolen here: https://github.com/KhronosGroup/glTF-Sample-Viewer/blob/master/src/shaders/tonemapping.glsl

// linear to sRGB approximation
// see http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
vec3 LINEARtoSRGB(vec3 color)
{
  return pow(color, vec3(INV_GAMMA));
}

// sRGB to linear approximation
// see http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
vec4 SRGBtoLINEAR(vec4 srgbIn)
{
  return vec4(pow(srgbIn.xyz, vec3(GAMMA)), srgbIn.w);
}

float sqr(float x){
    return x*x;
}
void main()
{
    vec3 N = normalize(vViewSpaceNormal); //Normal
    vec3 L = uWi; // Ligth direction

    ///


    vec4 baseColorFromTexture = SRGBtoLINEAR(texture(uBaseColorTexture, vTexCoords));
    vec4 metallicRoughnessTextur = texture(uMetallicRoughnessTextur, vTexCoords);
	vec3 metallic = vec3(uMetallicFactor * metallicRoughnessTextur.b);
    float roughness = uRougnessFactor * metallicRoughnessTextur.g;


    vec4 baseColor = baseColorFromTexture*uBaseColorFactor;
    // clamp — constrain a value to lie between two further values
    float NdotL = clamp(dot(N, L), 0, 1);
   // vec3 diffuse1 = baseColor.rgb * M_1_PI * NdotL;

  /// Diffuse + Glossy material model in GLSL
    float alpha = roughness*roughness;
    vec3 V = normalize(-vViewSpacePosition); // V== viewDirection
    vec3 H = normalize(L + V); // Half vector normalized
    float NdotV = clamp(dot(N, V), 0, 1);
    float alpha2 = sqr(alpha);
    float NdotV2 = sqr(NdotV);
    float NdotL2 = sqr(NdotL);
    float Vis = 0.5 / ((NdotL)*sqrt(NdotV2 * (1 - alpha2) +alpha2)+NdotV*sqrt(NdotL2*(1-alpha2)+alpha2));
    float NdotH = clamp(dot(N, H), 0, 1);
    float D = alpha2/ (M_PI*sqr((sqr(NdotH)*(alpha2-1)+1)));
    const vec3 dielectricSpecular = vec3(0.04, 0.04, 0.04);
    // mix == lerp - interpolation linéaire
    vec3 F0 = mix(dielectricSpecular, baseColor.rgb, metallic);
    float VdotH = clamp(dot(N, V), 0, 1);
    float baseShlickFactor = 1-VdotH;
    // You need to compute baseShlickFactor first
    float shlickFactor = baseShlickFactor * baseShlickFactor; // power 2
    shlickFactor *= shlickFactor; // power 4
    shlickFactor *= baseShlickFactor; // power 5

    vec3 F = F0 +(1-F0)*shlickFactor;
    vec3 f_specular = F*Vis*D;
    const vec3 black = vec3(0, 0, 0);
    vec3 cdiff = mix(baseColor.rgb * (1 - dielectricSpecular.r), black, metallic);
    vec3 diffuse = cdiff / M_PI;

    vec3 f_difuse = (1-F) * diffuse;
    vec3 f = f_specular + f_difuse;
    //


    fColor = LINEARtoSRGB(f*uLi*NdotL);
    //fColor = LINEARtoSRGB(diffuse1*uLi);
}
