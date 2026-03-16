#version 330

uniform float u_Time;
in vec3 a_Position;

// 중간 -> 끝까지 Sin 그리며 한 주기
void sin1()
{
    float t = u_Time * 2;
    vec4 newPosition;
    
    newPosition.x = a_Position.x + t; 
    newPosition.y = a_Position.y + 0.5 * sin(t * 2 * 3.141592);
    newPosition.z = 0.0;
    newPosition.w = 1.0;

    gl_Position = newPosition;
}

// 처음 -> 끝까지 Sin 그리며 한 주기
void sin2()
{
    float t = u_Time * 2;
    vec4 newPosition;

    float xOffset = -1.0 + 2.0 * t;
    float yOffset = 0.5 * sin(t * 2.0 * 3.141592);

     newPosition.x = a_Position.x + xOffset;
     newPosition.y = a_Position.y + yOffset;
     newPosition.z = 0.0;
     newPosition.w = 1.0;

     gl_Position = newPosition;
}

// 원 그리기
void circle()
{
    float t = u_Time * 2.0 * 3.141592;
    float radius = 1.0;
    vec4 newPosition;
    
    newPosition.x = a_Position.x + radius * cos(t);
    newPosition.y = a_Position.y + radius * sin(t);
    newPosition.z = 0.0;
    newPosition.w = 1.0;

    gl_Position = newPosition;
}

void main()
{
    circle();
}
