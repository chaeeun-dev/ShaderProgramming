#version 330

uniform float u_Time;
in vec3 a_Position;
in float a_Mass;
in vec2 a_Vel;
in float a_RV;
in float a_RV1;

const float c_PI = 3.141592;
const float c_G = -9.8;

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

void Falling()
{
    float t = mod(u_Time, 1.0); // 0 ~ 1 구간 반복
    float tt = t*t;
    float vx, vy;
    vx = a_Vel.x;
    vy = a_Vel.y;

    vec4 newPos;
    newPos.x = a_Position.x + vx * t;
    newPos.y = a_Position.y + vy * t + 0.5 * c_G * tt;
    newPos.z = 0;
    newPos.w = 1;

    gl_Position = newPos;
}

float random(float x)
{
    return fract(sin(x) * 43758.5453123);
}

void CircleParticleFalling()
{
    // float starTime = u_Time; // -> 원을 그리며 파티클 생겨남
    //float startTime = a_RV1;    // -> 원에서 계속 파티클 생겨남
    float startTime = random(a_RV);     // -> 랜덤 값(Pseudo Random)
    float newTime = u_Time - startTime; // StartTime이 지나지 않으면 그리지 않도록
  
    // Particle의 사이즈를 다르게 하려면?


    if (newTime > 0)
    {
        float t = mod(newTime, 1.0);
        float tt = t*t;
        float vx, vy;
        float sx, sy;
        vx = a_Vel.x;
        vy = a_Vel.y;

        sx = a_Position.x * a_RV1 + cos(a_RV*2*c_PI);
        sy = a_Position.y * a_RV1 + sin(a_RV*2*c_PI);

        vec4 newPos;
        newPos.x = sx + vx*t;
        newPos.y = sy + vy*t + 0.5*c_G*tt;
        newPos.z = 0.0;
        newPos.w = 1.0;

        gl_Position = newPos;
    }
    else 
    {
        gl_Position = vec4(-1000, 0, 0, 0);
    }
}

void MagicSpiral()
{
    // 1. 파티클 개별 시작 시간 (0 ~ 2.0초 사이 랜덤 대기)
    float startTime = a_RV1 * 2.0; 
    float newTime = u_Time - startTime;

    if (newTime > 0)
    {
        // 0 ~ 1.0 사이로 반복되는 로컬 타임
        float t = mod(newTime, 1.5) / 1.5; 
        
        // --- [사이즈 다르게 하기] ---
        // a_RV를 이용해 0.5배 ~ 1.5배 사이의 랜덤 크기 결정
        float sizeScale = 0.5 + random(a_RV) * 1.0;
        vec3 scaledPos = a_Position * sizeScale;

        // --- [소용돌이 치며 퍼지는 움직임] ---
        float angle = a_RV * 2.0 * c_PI + (t * 5.0); // 시간에 따라 회전 가속
        float radius = t * 0.8; // 시간이 흐를수록 중심으로 부터 멀어짐
        
        float sx = radius * cos(angle);
        float sy = radius * sin(angle);

        // --- [최종 위치 계산] ---
        vec4 newPos;
        newPos.x = scaledPos.x + sx;
        newPos.y = scaledPos.y + sy + (0.5 * c_G * t * t); // 중력 적용
        newPos.z = 0.0;
        newPos.w = 1.0;

        gl_Position = newPos;
    }
    else 
    {
        gl_Position = vec4(-1000.0, 0.0, 0.0, 0.0);
    }
}

void SupernovaBurst()
{
    // 1. 파티클별 생명주기 설정 (1.5초 주기)
    float duration = 1.5;
    float startTime = a_RV1 * 2.0; 
    float localTime = u_Time - startTime;

    if (localTime > 0.0)
    {
        float t = mod(localTime, duration) / duration; // 0.0 ~ 1.0 순환
        
        // 2. 역동적인 크기 변화 (맥동)
        // 처음에 작았다가 중간에 커지고 다시 사라짐
        float pulse = sin(t * c_PI); 
        float individualScale = (0.5 + random(a_RV) * 2.0) * pulse;
        vec3 scaledPos = a_Position * individualScale;

        // 3. 비선형 폭발 움직임
        // sqrt(t)를 사용하여 초기에는 빠르고 나중에는 완만하게 퍼짐
        float burstSpeed = pow(t, 0.4) * 1.5; 
        
        // 방향성 결정 (a_RV를 이용한 방사형 확산)
        float angle = a_RV * 2.0 * c_PI + (t * 4.0); // 퍼지면서 스스로 회전
        
        float offsetX = cos(angle) * burstSpeed;
        float offsetY = sin(angle) * burstSpeed;

        // 4. 최종 위치 (약간의 소용돌이 효과 추가)
        vec4 newPos;
        newPos.x = scaledPos.x + offsetX;
        newPos.y = scaledPos.y + offsetY;
        newPos.z = 0.0;
        newPos.w = 1.0;

        gl_Position = newPos;
    }
    else 
    {
        // 시작 전에는 화면 밖으로 치워둠
        gl_Position = vec4(-1000.0, 0.0, 0.0, 1.0);
    }
}

void main()
{
    //CircleParticleFalling();
    SupernovaBurst();
}
