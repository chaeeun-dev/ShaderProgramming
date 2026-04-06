#version 330

layout(location=0) out vec4 FragColor;

in vec2 V_TPos;

const float c_PI = 3.141592;

uniform float u_Time;

uniform vec4 u_DropInfo[1000];	// vec4(x, y, startTime, LifeTime)

void Simple()
{
	if (V_TPos.x + V_TPos.y < 0.5 )
	{
		FragColor = vec4(0);
	}
	else
	{
		FragColor = vec4(V_TPos, 0, 1);
	}
}

void Pattern()
{
	float lineCountH = 10;
	float lineCountV = 2;
	float lineWidth = 1;
	lineCountH = lineCountH / 2;
	lineCountV = lineCountV / 2;
	lineWidth = 50 / lineWidth;
	float per = -0.5*c_PI;

	float grey = pow(abs(sin((V_TPos.y*2*c_PI+per)*lineCountH)), lineWidth);
	float grey1 = pow(abs(sin((V_TPos.x*2*c_PI+per)*lineCountV)), lineWidth);

	FragColor = vec4(grey1 + grey);
}

void Circle()
{
	vec2 center = vec2(0.5, 0.5);
	vec2 currPos = V_TPos.xy + u_Time * 2*c_PI;
	float d = distance(center, currPos);
	float lineWidth = 0.01;
	float radius = 0.1;

	if (d > radius - lineWidth && d < radius)
	{
		FragColor = vec4(1);
	}
	else
	{
		FragColor = vec4(0);
	}
}

void CircleSin()
{
	vec2 center = vec2(0.5, 0.5);
	vec2 currPos = V_TPos.xy;
	float d = distance(center, currPos);
	float value = sin(d*2*c_PI*16 - u_Time);
	FragColor = vec4(pow(value, 16));
}

void RainDrop()
{
	float accum = 0;
	// RainDrop
	for (int i = 0; i < 1000; ++i)
	{
		float lTime = u_DropInfo[i].w;
		float sTime = u_DropInfo[i].z;
		float newTime = u_Time - sTime;

		if (newTime > 0)
		{
			newTime = fract(newTime/lTime);	// 0~1
			float oneMinus = 1 - newTime;	// 1~0
			float t = newTime * lTime;

			vec2 center = u_DropInfo[i].xy;
			vec2 currPos = V_TPos.xy;

			float range = t / 10;
			float d = distance(center, currPos);
	
			float fade = 100 * clamp(range - d, 0, 1);

			float value = pow(abs(sin(d * 2 * c_PI * 10 - t * 15)), 16);
			accum += value * fade * oneMinus;
		}
		else
		{
		}
	}

	FragColor = vec4(accum);
}

void main()
{
	RainDropFancy();
}
