uniform sampler2D volume;
uniform sampler2D scene;

void main()
{

	vec4 sceneColor = texture2D(scene, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t));
	vec4 volumeColor = texture2D(volume, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t));

	float volumeGrayLevel = (volumeColor.r + volumeColor.g + volumeColor.b)/3;
	if(volumeGrayLevel < 0.075)
		gl_FragColor = sceneColor;
	else
		gl_FragColor = volumeColor;

}