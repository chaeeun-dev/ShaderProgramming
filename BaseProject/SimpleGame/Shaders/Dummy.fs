#version 330

layout(location=0) out vec4 FragColor;

uniform sampler2D u_tacopiTex;

in float v_Grey;
in vec2 v_Tex;

const float c_PI = 3.141592;

mat2 rotate2D(float radian)
{
	return mat2(cos(radian), -sin(radian), sin(radian), cos(radian));
}

void Frag()
{
	FragColor = vec4(v_Grey);
	FragColor = vec4(v_Tex, 0, 1);

	// vec2 newTex = rotate2D(c_PI/2.0) * v_Tex;	// È¸Àü
	// FragColor = texture(u_tacopiTex, newTex);
	FragColor = v_Grey * texture(u_tacopiTex, v_Tex);
}

void main()
{
	FragColor = vec4(v_Grey);
}
