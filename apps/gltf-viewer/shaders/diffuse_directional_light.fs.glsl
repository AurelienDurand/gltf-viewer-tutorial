#version 330

in vec3 vViewSpacePosition;
in vec3 vViewSpaceNormal;
in vec2 vTexCoords;
uniform vec3 uWi; // ligth direction
uniform vec3 uLi; // Ligth intensity
uniform vec4 uBaseColorFactor ;
out vec3 fColor;



void main()
{


   // Need another normalization because interpolation of vertex attributes does not maintain unit length
   vec3 viewSpaceNormal = normalize(vViewSpaceNormal);
   float fr = 1.0/3.14;
   //dot product between ligth direction and normal to obtain cos
   //fColor = vec3(fr)*uLi*dot(viewSpaceNormal,uWi);
   fColor = vec3(uBaseColorFactor)*uLi*dot(viewSpaceNormal,uWi);


}
/*
void main()
{
   // Need another normalization because interpolation of vertex attributes does not maintain unit length
   vec3 viewSpaceNormal = normalize(vViewSpaceNormal);
   fColor = viewSpaceNormal;
}*/
