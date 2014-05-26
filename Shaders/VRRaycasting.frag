uniform sampler3D volume;
uniform sampler3D minMaxOctree;
uniform sampler2D transferFunction;
uniform sampler2D frontFrameBuffer;
uniform sampler2D backFrameBuffer;
uniform sampler2D diffuseHDRImage;
uniform float stepSize;
uniform float IBLScaleFactor;
uniform int windowWidth;
uniform int windowHeight;
uniform int useTransferFunction;
uniform int useIBL;

vec3 diffuseIBLIllumination(vec3 N)
{

   float x = 1.0 + atan(N.x, -N.z)/3.14;
   float y = acos(N.y)/3.14;
   x = x * 0.5;
   if(x < 0.001 || x > 0.999) x = 0.999;

   vec4 diffuseColor = texture2D(diffuseHDRImage, vec2(x, y));
   
   //calculate Diffuse Term:  
   vec4 Idiff = IBLScaleFactor * diffuseColor;

   // write Total Color:  
   return vec3(Idiff);  

}

vec4 computeIllumination(vec4 scalar, vec3 position) 
{

	if(scalar.a > 0.075) {
	
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
		scalar.rgb += diffuseIBLIllumination(N);
		
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