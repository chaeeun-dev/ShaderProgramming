#version 330

in vec3 a_Pos;
uniform vec4 u_Trans;
uniform bool u_Flip;

out vec2 v_Tex;

void main()
{
	vec4 newPosition;
	// w: scale 값, xy: trans
	newPosition.xy = a_Pos.xy*u_Trans.w + u_Trans.xy;
	newPosition.z = 0;
	newPosition.w= 1;
	gl_Position = newPosition;

	v_Tex.x = (a_Pos.x + 1.0) / 2.0;
	if (u_Flip)
		v_Tex.y = 1.0 - (a_Pos.y + 1.0) / 2.0;	// 좌표계 반대로
	else
		v_Tex.y = (a_Pos.y + 1.0) / 2.0;
}
