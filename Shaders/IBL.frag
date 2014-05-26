uniform sampler2D image;
uniform int mode;
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

   float x = 1.0 + atan(N.x, -N.z)/3.14;
   float y = acos(N.y)/3.14;
   x = x * 0.5;
   if(x < 0.001 || x > 0.999) x = 0.999;

   vec4 diffuseColor = texture2D(image, vec2(x, y));
   
   //calculate Diffuse Term:  
   vec4 Idiff = gl_FrontMaterial.diffuse * diffuseColor;

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