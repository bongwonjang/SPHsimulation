in vec2 TexCoords;

out vec4 FragColor;

uniform samplerCube skybox; 
uniform sampler2D backscene;
uniform sampler2D particlescne;

uniform mat4 inv_ProjectionMatrix;
uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;
uniform vec3 ViewPos;

//////////////////////////////////////////////
// I couldn't understand how this algorithm works..
//////////////////////////////////////////////
vec3 getEyePos(sampler2D tex, vec2 texCoord_n)  // This code should be improved
{
	float z = texture(tex, texCoord_n).r;
	vec2 xyPos = texCoord_n * 2.0f - vec2(1.0f, -1.0f);
	vec4 clipPos = vec4(xyPos, z, 1.0);
	vec4 viewPos = inv_ProjectionMatrix * clipPos;
	return(viewPos.xyz / viewPos.w);
}
vec3 getReflection(vec3 n)  // This code should be improved
{
	float ratio = 1.0f / 1.33f;
	vec3 I = vec3(TexCoords.xy, texture(particlescne, TexCoords).r) - ViewPos;
	vec3 R1 = refract(normalize(I), -n, ratio);
	vec3 col1 = texture(skybox, R1).rgb;
	vec3 R2 = reflect(normalize(I), -n);
	vec3 col2 = texture(skybox, R2).rgb;

	return 0.5 * col1 + 0.5 * col2;
}
void main()
{
	float depth = texture(particlescne, TexCoords).r;
	
	if (depth >= 0.999f)
	{
		FragColor = vec4(texture(backscene, TexCoords).rgb, 1.0);
		return;
	}

	float del_x = 1.0f / 1000.0f;
	float del_y = 1.0f / 1000.0f;

	vec3 posEye = getEyePos(particlescne, TexCoords);
	vec2 texCoord1 = vec2(TexCoords.x + del_x, TexCoords.y);
	vec2 texCoord2 = vec2(TexCoords.x - del_x, TexCoords.y);

	vec3 ddx = getEyePos(particlescne, texCoord1) - posEye;
	vec3 ddx2 = posEye - getEyePos(particlescne, texCoord2);
	if (abs(ddx.z) > abs(ddx2.z))
	{
		ddx = ddx2;
	}

	texCoord1 = vec2(TexCoords.x, TexCoords.y + del_y);
	texCoord2 = vec2(TexCoords.x, TexCoords.y - del_y);

	vec3 ddy = getEyePos(particlescne, texCoord1) - posEye;
	vec3 ddy2 = posEye - getEyePos(particlescne, texCoord2);
	if (abs(ddy2.z) < abs(ddy.z))
	{
		ddy = ddy2;
	}

	vec3 n = cross(ddx, ddy);
	n = normalize(n);

	FragColor = vec4(getReflection(n), 1.0);
}