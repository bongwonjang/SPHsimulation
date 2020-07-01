layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

in vec3 fragColor;

void main()
{
	vec3 N;
	N.xy = gl_PointCoord * 2.0 - vec2(1.0);
	float mag = dot(N.xy, N.xy);
	if (mag > 1.0f)
		discard;
	N.z = sqrt(1.0 - mag);
	N = normalize(N) * 0.5 + 0.5; //normal mapping form [-1:1] to [0:1]

	FragColor = vec4(N, 1.0f);
	BrightColor = vec4(N * 0.000f, 1.0); //0.012 for reducing exposure

	/*
	//Star Shape Bloom
	vec2 p = gl_PointCoord * 2.0 - vec2(1.0);
	float r = sqrt(dot(p, p));
	float theta = atan(p.y, p.x);
	if (dot(p, p) > 0.5*(exp(cos(theta * 5)*0.75)))
	{
		discard;
	}
	else
	{
		FragColor = vec4(fragColor, 1.0f);
		BrightColor = vec4(fragColor * 0.013, 1.0); //0.012 for reducing exposure
	}
	*/
}