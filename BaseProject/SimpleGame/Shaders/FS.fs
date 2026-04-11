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

void Flag()
{
	float amp = 0.4;
	float speed = 20;
	float sinInput = V_TPos.x * c_PI * 2 - u_Time * speed;
	float sinValue = V_TPos.x * amp * (((sin(sinInput) + 1) / 2) - 0.5) + 0.5;	// 0 ~ 1
	float finalWidth = 0.1;	
	float width = 0.5 * mix(1, finalWidth, V_TPos.x);
	float grey = 0;


	if (V_TPos.y < sinValue + width / 2 && V_TPos.y > sinValue - width / 2)  
	{
		grey = 1;
	}
	else 
	{
		grey = 0; 
		discard;
	}

	FragColor = vec4(grey);
}

void Flame()
{
	float amp = 0.4;
	float speed = 20;
	float newY = 1 - V_TPos.y;
	float sinInput = newY * c_PI * 2 - u_Time * speed;
	float sinValue = newY * amp * (((sin(sinInput) + 1) / 2) - 0.5) + 0.5;	// 0 ~ 1
	float finalWidth = 0.1;	
	float width = 0.5 * mix(1, finalWidth, newY);
	float grey = 0;


	if (V_TPos.x < sinValue + width / 2 && V_TPos.x > sinValue - width / 2)  
	{
		grey = 1;
	}
	else 
	{
		grey = 0; 
		discard;
	}

	FragColor = vec4(grey);
}

float hash(vec2 p)
{
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float noise(vec2 p)
{
    vec2 i = floor(p);
    vec2 f = fract(p);

    float a = hash(i);
    float b = hash(i + vec2(1, 0));
    float c = hash(i + vec2(0, 1));
    float d = hash(i + vec2(1, 1));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
           (c - a) * u.y * (1.0 - u.x) +
           (d - b) * u.x * u.y;
}

// 프랑스 국기
void Flag2()
{
float amp = 0.4;
    float speed = 20;

    float sinInput = V_TPos.x * c_PI * 2 - u_Time * speed;
    float wave = (((sin(sinInput) + 1.0) / 2.0) - 0.5) * amp * V_TPos.x;

    float sinValue = wave + 0.5;

    float finalWidth = 0.1;
    float width = 0.5 * mix(1.0, finalWidth, V_TPos.x);

    float edgeDist = abs(V_TPos.y - sinValue);

    if (edgeDist > width * 0.5)
        discard;

    // =========================
    // 🔥 흔들린 좌표 (핵심)
    // =========================
    vec2 uv = V_TPos;
    uv.y -= wave;

    // =========================
    // 🇫🇷 프랑스 국기 패턴
    // =========================
    vec3 blue  = vec3(0.0, 0.2, 0.6);
    vec3 white = vec3(0.95, 0.95, 0.95);
    vec3 red   = vec3(0.8, 0.1, 0.1);

    vec3 color;

    if (uv.x < 0.333)
        color = blue;
    else if (uv.x < 0.666)
        color = white;
    else
        color = red;

    // =========================
    // 🌫️ 살짝 낡은 느낌
    // =========================
    float fade = noise(uv * 5.0);
    color *= mix(0.7, 1.0, fade);

    float dirt = noise(uv * 20.0);
    color -= dirt * 0.1;

    // =========================
    // ✨ 가장자리 강조
    // =========================
    float highlight = smoothstep(width * 0.5, 0.0, edgeDist);
    color += highlight * 0.15;

    // =========================
    // 🌗 물결 음영
    // =========================
    float shading = 0.7 + 0.3 * sin(V_TPos.x * 10.0 - u_Time * 5.0);
    color *= shading;

    FragColor = vec4(color, 1.0);
}

void main()
{
	Flag2();
}
