layout(location = 0) in vec3 aPos;

out vec4 Position;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;
uniform vec3 ViewPos;

void main() 
{
	Position = vec4(aPos, 1.0);
	gl_Position = Projection * View * Model * vec4(aPos, 1.0);

	vec4 particlePos = View * Model * vec4(aPos, 1.0f);
	float dist = length(particlePos.xyz);
	gl_PointSize = 200.0f / dist;
}