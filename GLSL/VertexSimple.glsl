layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec3 fragColor;
uniform mat4 MVP;
void main()
{
	fragColor = aColor;
	gl_Position = MVP * vec4(aPos, 1.0f);
}