#version 330

uniform float u_Time;

in vec3 a_Pos;
in float a_Mass;
in vec2 a_Vel;
in float a_RV;
in float a_RV1;
in float a_RV2;
in vec2 a_Tex;
in vec3 a_RGB;

out float v_Grey;
out vec3 v_Color;
out vec2 v_Tex;

const float c_PI = 3.141592;
const float c_G = -9.8;

float random(float x) {
    return fract(sin(x) * 43758.5453123);
}

void Sin0()
{
	float startTime = a_RV1 * 2;
	float newTime = u_Time - startTime;

    if(newTime>0)
    {
	    float t = mod(newTime * 2, 1.0);
        float amp = (1-t)*0.2*(a_RV-0.5)*2;
        float period = a_RV1;
	    vec4 newPosition;
	    newPosition.x = a_Pos.x * a_RV2 * 0.2 + t;
	    newPosition.y = a_Pos.y * a_RV2 * 0.2 
		    + amp*sin(t * 2 * c_PI * period);
	    newPosition.z = 0;
	    newPosition.w = 1;

	    gl_Position = newPosition;
        v_Grey = 1-t;
    }
    else
    {
        gl_Position = vec4(100000, 10000, 100, 10);
        v_Grey = 0;
    }
}

void Sin1()
{
	float t = u_Time * 2;
	vec4 newPosition;
	newPosition.x = a_Pos.x + t;
	newPosition.y = a_Pos.y + 0.5*sin(t * 2 * 3.141592);
	newPosition.z = 0;
	newPosition.w = 1;

	gl_Position = newPosition;
}

void Sin2()
{
	float t = mod(u_Time, 1.0);  // 0 ~ 1 구간 반복
	vec4 newPosition;

	// 화면 맨 왼쪽(-1)에서 오른쪽 끝(+1)까지 이동
	float xOffset = -1.0 + 2.0 * t;

	// t가 0~1일 때 사인 1주기
	float yOffset = 0.5 * sin(t * 2.0 * 3.141592);

	newPosition.x = a_Pos.x + xOffset;
	newPosition.y = a_Pos.y + yOffset;
	newPosition.z = 0.0;
	newPosition.w = 1.0;

	gl_Position = newPosition;
}

void Circle()
{
	float t = u_Time * 2.0 * 3.141592;
	float r = 1.0;

	vec4 newPosition;
	newPosition.x = a_Pos.x + r * cos(t);
	newPosition.y = a_Pos.y + r * sin(t);
	newPosition.z = 0.0;
	newPosition.w = 1.0;

	gl_Position = newPosition;
}

float PingPong(float t, float length)
{
    float m = mod(t, length * 2.0);
    return length - abs(m - length);
}

void Bounce()
{
	float t = u_Time * 20;
    vec4 newPosition;

    float halfWidth  = 0.1;
    float halfHeight = 0.1;

    float minX = -1.0 + halfWidth;
    float maxX =  1.0 - halfWidth;
    float minY = -1.0 + halfHeight;
    float maxY =  1.0 - halfHeight;

    float x = minX + PingPong(t * 0.7, maxX - minX);
    float y = minY + PingPong(t * 1.1, maxY - minY);

    newPosition.x = a_Pos.x + x;
    newPosition.y = a_Pos.y + y;
    newPosition.z = 0.0;
    newPosition.w = 1.0;

    gl_Position = newPosition;
}

void Falling()
{
	float startTime = a_RV1 * 3;
	float newTime = u_Time - startTime;

	if(newTime > 0)
	{
        float lifeScale = 2.0;
        float lifeTime = 0.5 + a_RV2 * lifeScale;
		float t = lifeTime*fract(newTime/lifeTime); //0~lifeTime구간 반복
		float tt = t*t;
		float vx, vy;
		float sx, sy;
		vx = a_Vel.x/30;
		vy = a_Vel.y/30;

		sx = a_Pos.x * (1-random(a_RV)) + sin(a_RV*2*c_PI);
		sy = a_Pos.y * (1-random(a_RV)) + cos(a_RV*2*c_PI);

		vec4 newPos;
		newPos.x = sx + vx*t;
		newPos.y = sy + vy*t + 0.5*c_G*tt;
		newPos.z = 0;
		newPos.w = 1;

		gl_Position = newPos;
		v_Grey = 1;
	}
	else
	{
		gl_Position = vec4(-1000, 0, 0, 1);
	}
}

void Shape()
{
	float lifeTime = 0.5 + 5.0*a_RV;
	float startTime = 5.0*a_RV1;

	float newTime = u_Time - startTime;
	if(newTime > 0)
	{
		float t = fract(newTime/lifeTime)*lifeTime;
		float tt = t*t;

		float newX = a_Pos.x + a_Vel.x*t*0.1;
		float newY = a_Pos.y + a_Vel.y*t*0.1;

		gl_Position = vec4(newX, newY, 0, 1);
		v_Grey = 1-fract(newTime/lifeTime);
	}
	else
	{
		gl_Position = vec4(-10000, 0, 0, 1);
		v_Grey = 1;
	}
	
	v_Color = a_RGB;
	v_Tex = a_Tex;
}

void main()
{
    Shape();
}
