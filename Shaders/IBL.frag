//Spherical Harmonics values

uniform vec3 L00;
uniform vec3 L1m1;
uniform vec3 L10;
uniform vec3 L11;
uniform vec3 L2m2;
uniform vec3 L2m1;
uniform vec3 L20;
uniform vec3 L21;
uniform vec3 L22;

//Spherical Harmonics constants
const float C1 = 0.429043;
const float C2 = 0.511664;
const float C3 = 0.743125;
const float C4 = 0.886227;
const float C5 = 0.247708;

uniform int mode;
uniform float scaleFactor;
varying vec3 N;
varying vec3 v;


vec4 phong() 
{
	
   vec3 L = normalize(gl_LightSource[0].position.xyz - v);   
   vec3 E = normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0)  
   vec3 R = normalize(-reflect(L,N));  
 
   //calculate Ambient Term:  
   vec4 Iamb = gl_FrontLightProduct[0].ambient;    

   //calculate Diffuse Term:  
   vec4 Idiff = gl_FrontLightProduct[0].diffuse * max(dot(N,L), 0.0);    
   
   // calculate Specular Term:
   vec4 Ispec = gl_FrontLightProduct[0].specular 
                * pow(max(dot(R,E),0.0),0.3*gl_FrontMaterial.shininess);

   // write Total Color:  
   return gl_FrontLightModelProduct.sceneColor + Iamb + Idiff + Ispec;  

}

vec4 ibl()
{


   vec3 diffuseColor = C1 * L22 * (N.x * N.x - N.y * N.y) +
		C3 * L20 * N.z * N.z +
		C4 * L00 -
		C5 * L20 +
		2.0 * C1 * L2m2 * N.x * N.y +
		2.0 * C1 * L21 * N.x * N.z +
		2.0 * C1 * L2m1 * N.y * N.z +
		2.0 * C2 * L11 * N.x +
		2.0 * C2 * L1m1 * N.y +
		2.0 * C2 * L10 * N.z;

   if(diffuseColor.r < 0) diffuseColor.r *= -1;
   if(diffuseColor.g < 0) diffuseColor.g *= -1;
   if(diffuseColor.b < 0) diffuseColor.b *= -1;

   //calculate Diffuse Term:  
   vec4 Idiff = gl_FrontMaterial.diffuse * scaleFactor * vec4(diffuseColor, 1.0);

   // write Total Color:  
   return Idiff;  

}

void main (void)  
{  
   
   vec4 color;
   if(mode == 0)
      color = phong();
   else
      color = ibl();

   gl_FragColor = color;

}