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

// 1. 리사주 궤도 + 심장 박동 + 미세 떨림 효과
void cosmicDance()
{
    // 시간 흐름 속도 조절 (기존 0.0001이 너무 느리다면 여기서 배율을 높이세요)
    float t = u_Time * 10.0; 
    float PI = 3.14159265;

    // [Step 1] 무한대(Infinity) 궤도 이동
    // 리사주 도형 원리를 이용해 8자 모양으로 움직이게 합니다.
    float xPath = cos(t); 
    float yPath = sin(2.0 * t) / 2.0; 

    // [Step 2] 심장 박동 효과 (Pulsing)
    // 사각형의 크기가 시간에 따라 '두근두근'하듯이 변합니다.
    float pulse = 1.0 + 0.3 * sin(t * 4.0);

    // [Step 3] 소용돌이 치는 왜곡 (Spiral Distortion)
    // 정점의 위치에 따라 약간의 회전 변위를 주어 사각형이 젤리처럼 출렁이게 합니다.
    float angle = sin(t) * PI * 0.2;
    mat2 rotation = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));

    vec4 newPosition;
    
    // 원래 위치(a_Position)를 회전시키고 크기를 조절한 뒤, 궤도(Path) 값을 더합니다.
    vec2 deformedPos = rotation * (a_Position.xy * pulse);
    
    newPosition.x = deformedPos.x + xPath;
    newPosition.y = deformedPos.y + yPath;
    newPosition.z = 0.0;
    newPosition.w = 1.0;

    gl_Position = newPosition;
}

void main()
{
    pulsatingHeart();
}
