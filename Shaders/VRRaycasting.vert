varying vec3 v;

void main(void)
{

	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	v = vec3(gl_ModelViewMatrix * gl_Vertex);

}