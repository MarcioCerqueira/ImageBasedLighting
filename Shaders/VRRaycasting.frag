uniform sampler3D volume;
uniform sampler3D minMaxOctree;
uniform sampler2D transferFunction;
uniform sampler2D frontFrameBuffer;
uniform sampler2D backFrameBuffer;
uniform float stepSize;
uniform float IBLScaleFactor;
uniform int windowWidth;
uniform int windowHeight;
uniform int useTransferFunction;
uniform int useIBL;
varying vec3 v;

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

//Specular SH values
uniform vec3 lightDir;
uniform vec3 lightColor;

//Spherical Harmonics constants
const float C1 = 0.429043;
const float C2 = 0.511664;
const float C3 = 0.743125;
const float C4 = 0.886227;
const float C5 = 0.247708;

//SH aux
uniform float diffuseScaleFactor;
uniform float specularScaleFactor;
uniform float shininess;

vec3 diffuseIBL(vec3 N)
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

   //calculate Diffuse Term:  
   vec4 Idiff = (diffuseScaleFactor/10) * vec4(diffuseColor, 0.0);

   // write Total Color:  
   return vec3(Idiff);  

}

vec4 specularIBL(vec3 N)
{

   vec3 E = normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0)  
   vec3 R = normalize(-reflect(vec3(lightDir.x, lightDir.y, lightDir.z), N));  
   vec4 Ispec = vec4(lightColor, 1.0) * pow(max(dot(R,E),0.0), 0.3 * shininess) * specularScaleFactor;
   return Ispec;

}

vec4 computeIllumination(vec4 scalar, vec3 position) 
{

	if(scalar.a > 0.025) {
	
		float delta = 0.01;
		vec3 sample1, sample2;
		vec3 alpha1 = vec3(0, 0, 0);
		vec3 alpha2 = vec3(0, 0, 0);
		
		sample2.x = texture3D(volume, vec3(position + vec3(delta, 0, 0))).x;
		sample2.y = texture3D(volume, vec3(position + vec3(0, delta, 0))).x;
		sample2.z = texture3D(volume, vec3(position + vec3(0, 0, delta))).x;
		
		sample1.x = texture3D(volume, vec3(position - vec3(delta, 0, 0))).x;
		sample1.y = texture3D(volume, vec3(position - vec3(0, delta, 0))).x;
		sample1.z = texture3D(volume, vec3(position - vec3(0, 0, delta))).x;
		
		//central difference and normalization
		vec3 N = normalize(sample2 - sample1);

		//we consider the volume color our diffuse material
		if(N.x > -1 && N.y > -1 && N.z > -1)
			scalar.rgb += diffuseIBL(N) + specularIBL(N);
	
	}

	return scalar;
}

void main (void)  
{

	vec4 value;
	vec2 scalar = vec2(0, 0);
	vec4 src = vec4(0, 0, 0, 0);
	vec4 rayStart = texture2D(frontFrameBuffer, vec2(gl_FragCoord.x/float(windowWidth ), gl_FragCoord.y/float(windowHeight)));
	vec4 rayEnd = texture2D(backFrameBuffer, vec2(gl_FragCoord.x/float(windowWidth), gl_FragCoord.y/float(windowHeight)));
	if(rayStart == rayEnd)
		discard;
	//Initialize accumulated color and opacity
	vec4 dst = vec4(0, 0, 0, 0);
	//Determine volume entry position
	vec3 position = vec3(rayStart);
	vec3 direction = vec3(rayEnd) - vec3(rayStart);
	float len = length(direction); // the length from front to back is calculated and used to terminate the ray
    direction = normalize(direction);
	float dirLength = length(direction);
	float maxStepSize = 0.04; //2.f/50.f
	float accLength = 0.0;
	vec4 maxOpacity;
	
	//Loop for ray traversal
	for(int i = 0; i < 200; i++) //Some large number
	{
		
		maxOpacity = texture3D(minMaxOctree, position);

		if(maxOpacity.g > 0.0) {
			
			//Data access to scalar value in 3D volume texture
			value = texture3D(volume, position);

			if(useTransferFunction == 1) {	
				
				scalar.y = value.a;
				//Lookup in pre-integration table
				src = texture2D(transferFunction, scalar.xy);
				
				if(useIBL == 1) 
					src = computeIllumination(src, position);
			
				if(src.a > 0.125)
					dst = (1.0 - dst.a) * src + dst;
				
				//Save previous scalar value
				scalar.x = scalar.y;
				if(useIBL == 1)
					scalar.x = src.a;

			} else {

				if(useIBL == 1)
					value = computeIllumination(value, position);

				if(value.a > 0.125)
					dst = (1.0 - dst.a) * value + dst;
				
			}
			
			//Advance ray position along ray direction
			position = position + direction * stepSize;
			accLength += dirLength * stepSize;
			
		} else {
			
			position = position + direction * maxStepSize;
			accLength += dirLength * maxStepSize;
		
		}
		
		if(dst.a > 0.95)
			break;

		//Ray termination: Test if outside volume...
		if(accLength > len)
			break;

	}

	gl_FragColor = dst;
	
}