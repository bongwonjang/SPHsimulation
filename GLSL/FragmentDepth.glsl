in vec4 Position;

out vec4 FragColor;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

void main()
{
	float sphereRadius = 500.0;

	vec3 N;
	N.xy = gl_PointCoord * 2.0f - 1.0;
	float mag = dot(N.xy, N.xy);
	if (mag > 1.0) 
		discard; // kill(?) pixels outside circle
	N.z = sqrt(1.0 - mag);

	vec4 EyeSpacePos = View * Model * Position;

	vec4 pixelPos = vec4(EyeSpacePos.xyz + N * sphereRadius, 1.0f);
	vec4 clipSpacePos = Projection * pixelPos;
	float normDepth = clipSpacePos.z / clipSpacePos.w;
	normDepth = normDepth / -110; // This code should be improved

	float far = 1.0f;
	float near = 0.1f;
	float linearDepth = (2.0 * near * far) / (far + near - normDepth * (far - near));

	FragColor = vec4(vec3(linearDepth), 1.0f);
}