#version 330

uniform float u_Time;

in vec3 a_Pos;	// -0.5~0.5

out float v_Grey;
out vec2 v_Tex;

float c_PI = 3.141592;

void Frag()
{
	float tX, tY;
	tX = a_Pos.x + 0.5;
	tY = 1.0 - (a_Pos.y + 0.5);
	v_Tex = vec2(tX, tY);

	float value = a_Pos.x + 0.5;
	float newX = a_Pos.x;
	float newY = a_Pos.y * (1.0 - value * 0.5) +
				value * 0.25 * sin((newX + 0.5) * 2.0 * c_PI - u_Time);
	vec4 final = vec4(newX, newY, 0.0, 1.0);

	vec4 newPosition = final;
	
	float grey = (sin((newX + 0.5) * 2.0 * c_PI - u_Time) + 1.0) / 2.0;
	v_Grey = grey;

	gl_Position = newPosition;
}

void Circle()
{
	vec4 points[2];
	points[0] = vec4(0.0, 0.0, 1.0, 0.2); // x, y, w(lifeTime), x(startTime)
	points[1] = vec4(0.2, 0.2, 0.5, 0.0);
	
	float accum = 0;

	for (int i = 0; i < 2; ++i)
	{
		vec2 center = points[i].xy;
		vec2 pos = a_Pos.xy;
		float lTime = points[i].z;
		float sTime = points[i].w;
		float nTime = u_Time - sTime;
		
		if (nTime > 0)
		{
			float lVal = fract(nTime / lTime);
			float t = lVal * lTime;

			float d = distance(center, pos);
			
			float range = t/5.0;
			float fade = 30 * clamp(range - d, 0, 1.0);	// 시간에 따라서

			float sinValue = sin(d * 4 * c_PI * 8 - t * 2);

			accum += sinValue * fade;
		}

		float d = distance(center, pos);

		accum += abs(sin(d * 4 * c_PI * 8 + u_Time * 2));
	}

	v_Grey = accum;

	gl_Position = vec4(a_Pos, 1.0);
}

void main()
{
	Circle();
}
