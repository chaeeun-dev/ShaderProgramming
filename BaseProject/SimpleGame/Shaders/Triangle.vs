#version 330

uniform float u_Time;
in vec3 a_Position;
in float a_Mass;
in vec2 a_Vel;
in float a_RV;
in float a_RV1;
in float a_RV2;

// Vertex Shader의 Output은 Fragment Shader 입력의 근간
out float v_Grey;

const float c_PI = 3.141592;
const float c_G = -9.8;

float random(float x)
{
    return fract(sin(x) * 43758.5453123);
}

// 중간 -> 끝까지 Sin 그리며 한 주기
void sin1()
{
    float startTime  = a_RV1 * 2;
    float newTime = u_Time - startTime;

    if (newTime > 0)
    {
        float t = mod(newTime * 2, 1.0);
        float amp = t * 0.5 * (a_RV - 0.5) * 2;    
        // 아래로도 내려가도록 a_RV - 0.5
        // 끝으로 갈수록 퍼짐 (1-t), 끝으로 갈수록 줄어듦 (t)
        float fre = a_RV1;
        
        vec4 newPosition;
    
        newPosition.x = a_Position.x * a_RV2 * 0.2  + t; 
        newPosition.y = a_Position.y * a_RV2 * 0.2 
                            + amp * sin(t * 2 * c_PI * fre);
        newPosition.z = 0.0;
        newPosition.w = 1.0;

        gl_Position = newPosition;
        v_Grey = 1-t;
    }
   else
   {
        gl_Position = vec4(-1000, 0, 0, 0);
        v_Grey = 0;
   }
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

// [오류] 처음에 뒤죽박죽이었다가 나중에 괜찮아짐..?
// [원인?] Stride, 데이터가 잘못 올라갔거나 -> 데이터 잘 맞게 썼는데 머지????
void Falling()
{
    float startTime  = a_RV1 * 3;
    float newTime = u_Time - startTime;

    if (newTime > 0)
    {
        float lifescale = 2.0;
        float lifeTime = 0.5 + a_RV2 * lifescale;
        // float t = lifeTime * fract(newTime/lifeTime);   // 0 ~ 1 구간 반복
        float t = mod(newTime, lifeTime);

        float tt = t*t;
        float vx, vy;
        float sx, sy;

        vx = a_Vel.x;
        vy = a_Vel.y;
        sx = a_Position.x * (1-random(a_RV)) + sin(a_RV*2*c_PI);
        sy = a_Position.y * (1-random(a_RV)) + cos(a_RV*2*c_PI);

        vec4 newPos;
        newPos.x = sx + vx * t;
        newPos.y = sy + vy * t + 0.5 * c_G * tt;
        newPos.z = 0;
        newPos.w = 1;

        gl_Position = newPos;
    }
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
    //SupernovaBurst();
    Falling();
    //sin1();
}
