layout(location = 0) in vec3 aPos;

out vec3 TexCoord;

uniform mat4 View;
uniform mat4 Projection;

void main() {

	TexCoord = aPos;
	vec4 pos = Projection * View * vec4(aPos * 500, 1.0);
	gl_Position = pos.xyww;
}