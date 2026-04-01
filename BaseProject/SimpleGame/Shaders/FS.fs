#version 330

layout(location=0) out vec4 FragColor;

in vec2 V_TPos;

const float c_PI = 3.141592;

uniform float u_Time;

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

void FractalCircle()
{
    vec2 uv = V_TPos.xy;
    vec2 center = vec2(0.5, 0.5);

    // Áß½É ±âÁØ ÁÂÇ¥
    vec2 p = uv - center;

    float t = u_Time * 0.5;
    float accum = 0.0;
    float scale = 1.0;

    // ÇÁ·¢Å» ¹Ýº¹
    for(int i = 0; i < 5; i++)
    {
        // È¸Àü
        float angle = t + float(i) * 1.2;
        mat2 rot = mat2(cos(angle), -sin(angle),
                        sin(angle),  cos(angle));
        p = rot * p;

        // °Å¸® ±â¹Ý ÆÄµ¿
        float d = length(p);
        float wave = sin(d * 20.0 - u_Time * 2.0);

        // ´©Àû (ÇÁ·¢Å» ´À³¦)
        accum += abs(wave) / scale;

        // ÁÂÇ¥ ¿Ö°î (ÇÙ½É!)
        p = abs(p) / dot(p, p) - 0.5;

        scale *= 1.5;
    }

    // »ö»ó
    vec3 col = vec3(accum);
    col = pow(col, vec3(1.2));

    FragColor = vec4(col, 1.0);
}

void main()
{
}
