#version 330

layout(location=0) out vec4 FragColor;

// Vertex Shader의 Output(개수가 달라지지만, 데이터 타입은 같음)
in float v_Grey;

void main()
{
	FragColor = vec4(v_Grey);
}
