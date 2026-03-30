#version 330

layout(location=0) out vec4 FragColor;

in vec2 V_TPos;

void main()
{
	if (V_TPos.x < 0.5 )
	{
		FragColor = vec4(0);
	}
	else
	{
		FragColor = vec4(V_TPos, 0, 1);
	}
	
}
