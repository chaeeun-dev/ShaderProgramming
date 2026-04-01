#include "stdafx.h"
#include "Renderer.h"
#include <vector>

Renderer::Renderer(int windowSizeX, int windowSizeY)
{
	Initialize(windowSizeX, windowSizeY);
}


Renderer::~Renderer()
{
}

void Renderer::Initialize(int windowSizeX, int windowSizeY)
{
	//Set window size
	m_WindowSizeX = windowSizeX;
	m_WindowSizeY = windowSizeY;

	//Load shaders
	m_SolidRectShader = CompileShaders("./Shaders/SolidRect.vs", "./Shaders/SolidRect.fs");
	m_TriangleShader = CompileShaders("./Shaders/Triangle.vs", "./Shaders/Triangle.fs");
	m_FSShader = CompileShaders("./Shaders/FS.vs", "./Shaders/FS.fs");

	//Create VBOs
	//CreateVertexBufferObjects();
	//CreateParticle(1000);
	CreateVertexBufferObjects2();

	if (m_SolidRectShader > 0 && m_VBORect > 0)
	{
		m_Initialized = true;
	}
}

bool Renderer::IsInitialized()
{
	return m_Initialized;
}

void Renderer::CreateVertexBufferObjects()
{
	float rect[]
		=
	{
		-1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f, -1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, 1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, //Triangle1
		-1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f,  1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, 1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f, //Triangle2
	};

	glGenBuffers(1, &m_VBORect);	// VBO(GPU 전용 보관함)의 ID 생성
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);	// Binding, VBO를 사용할 것이라고 선언
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);	// 데이터를 CPU에서 GPU 보관함으로 복사

	float centerX = 0;
	float centerY = 0;
	float size = 0.1;
	float mass = 1;	// 단위 kg
	// 초기 속도
	float vx = 1;
	float vy = 3;
	float triangle[]
		=
	{
		// triangle1
		centerX - size / 2, centerY - size / 2, 0, 
		mass, vx, vy,
		centerX + size / 2, centerY - size / 2, 0,
		mass, vx, vy,
		centerX + size / 2, centerY + size / 2, 0,
		mass, vx, vy,
		
		// triangle2
		centerX - size / 2, centerY - size / 2, 0, 
		mass, vx, vy,
		centerX + size / 2, centerY + size / 2, 0,
		mass, vx, vy,
		centerX - size / 2, centerY + size / 2, 0,
		mass, vx, vy,
	};

	glGenBuffers(1, &m_VBOTriangle);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOTriangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
}

void Renderer::CreateVertexBufferObjects2()
{
	float rectFS[]
		=
	{
		-1.f, -1.f, 0.f, 0, 1,
		1.f, 1.f, 0.f, 1, 0,
		-1.f, 1.f, 0.f, 0, 0,	//Triangle1

		-1.f, -1.f, 0.f, 0, 1,
		1.f, -1.f, 0.f, 1, 1,
		1.f, 1.f, 0.f, 1, 0,	 //Triangle2
	};

	glGenBuffers(1, &m_VBOFS);	// VBO(GPU 전용 보관함)의 ID 생성
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOFS);	// Binding, VBO를 사용할 것이라고 선언
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectFS), rectFS, GL_STATIC_DRAW);	// 데이터를 CPU에서 GPU 보관함으로 복사
}

void Renderer::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	//쉐이더 오브젝트 생성
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	//쉐이더 코드를 쉐이더 오브젝트에 할당
	glShaderSource(ShaderObj, 1, p, Lengths);

	//할당된 쉐이더 코드를 컴파일
	glCompileShader(ShaderObj);

	GLint success;
	// ShaderObj 가 성공적으로 컴파일 되었는지 확인
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];

		//OpenGL 의 shader log 데이터를 가져옴
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		printf("%s \n", pShaderText);
	}

	// ShaderProgram 에 attach!!
	glAttachShader(ShaderProgram, ShaderObj);
}

bool Renderer::ReadFile(char* filename, std::string *target)
{
	std::ifstream file(filename);
	if (file.fail())
	{
		std::cout << filename << " file loading failed.. \n";
		file.close();
		return false;
	}
	std::string line;
	while (getline(file, line)) {
		target->append(line.c_str());
		target->append("\n");
	}
	return true;
}

GLuint Renderer::CompileShaders(char* filenameVS, char* filenameFS)
{
	GLuint ShaderProgram = glCreateProgram(); //빈 쉐이더 프로그램 생성

	if (ShaderProgram == 0) { //쉐이더 프로그램이 만들어졌는지 확인
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs;

	//shader.vs 가 vs 안으로 로딩됨
	if (!ReadFile(filenameVS, &vs)) {
		printf("Error compiling vertex shader\n");
		return -1;
	};

	//shader.fs 가 fs 안으로 로딩됨
	if (!ReadFile(filenameFS, &fs)) {
		printf("Error compiling fragment shader\n");
		return -1;
	};

	// ShaderProgram 에 vs.c_str() 버텍스 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	// ShaderProgram 에 fs.c_str() 프레그먼트 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	//Attach 완료된 shaderProgram 을 링킹함
	glLinkProgram(ShaderProgram);

	//링크가 성공했는지 확인
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		// shader program 로그를 받아옴
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error linking shader program\n" << ErrorLog;
		return -1;
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error validating shader program\n" << ErrorLog;
		return -1;
	}

	glUseProgram(ShaderProgram);
	std::cout << filenameVS << ", " << filenameFS << " Shader compiling is done.";

	return ShaderProgram;
}

void Renderer::DrawSolidRect(float x, float y, float z, float size, float r, float g, float b, float a)
{
	float newX, newY;

	GetGLPosition(x, y, &newX, &newY);

	//Program select
	glUseProgram(m_SolidRectShader);

	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Trans"), newX, newY, 0, size);
	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Color"), r, g, b, a);

	int attribPosition = glGetAttribLocation(m_SolidRectShader, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(attribPosition);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

float g_time = 0;

void Renderer::DrawTriangle()
{
	g_time += 0.0003;

	//Program select
	glUseProgram(m_TriangleShader);

	int uTime = glGetUniformLocation(m_TriangleShader, "u_Time");
	glUniform1f(uTime, g_time);

	int attribPosition = glGetAttribLocation(m_TriangleShader, "a_Position");
	int attribMass = glGetAttribLocation(m_TriangleShader, "a_Mass");
	int attribVel = glGetAttribLocation(m_TriangleShader, "a_Vel");

	// Enable -> 사용할 수 있도록 
	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(attribMass);
	glEnableVertexAttribArray(attribVel);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOTriangle);

	// GPU에게 데이터의 구조를 알려주기
	// a_Position: 시작점 0부터 float 3개 읽어라
	glVertexAttribPointer(attribPosition, 3/*몇 개 읽음*/, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0/*시작점 주소*/);
	// a_Mass: 시작점 3부터 1개 읽어라
	glVertexAttribPointer(attribMass, 1/*몇 개 읽음*/, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (GLvoid*)(sizeof(float) * 3)/*시작점 주소*/);
	// a_Vel: 시작점 4부터 2개 읽어라
	glVertexAttribPointer(attribVel, 2/*몇 개 읽음*/, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (GLvoid*)(sizeof(float) * 4)/*시작점 주소*/);

	// Stride: 다음 정점으로 가기 위해 몇 바이트 건너뛰어야 하는지
	// x, y, z, mass, vx, vy -> 6

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::CreateParticle(const int num)
{
	m_ParticleCount = num;
	std::vector<float> particles;
	float size = 0.05f;	

	for (int i = 0; i < num; ++i)
	{
		// -1.0 ~ 1.0 랜덤 위치와 속도
		float startX = 0;
		float startY = 0;
		float mass = 1.0f;
		float vx = ((rand() % 2000) / 1000.0f) - 1.0f;
		float vy = (rand() % 1000) / 500.0f; // 위로 솟구치는 속도
		float RV = static_cast<float>(std::rand())
			/ static_cast<float>(RAND_MAX);		// 0 ~ 1 사이 랜덤값
		float RV1 = static_cast<float>(std::rand())
			/ static_cast<float>(RAND_MAX);		// 0 ~ 1 사이 랜덤값
		float RV2 = static_cast<float>(std::rand())
			/ static_cast<float>(RAND_MAX);		// 0 ~ 1 사이 랜덤값


		float vertices[] =
		{
			startX - size / 2, startY - size / 2, 0, mass, vx, vy, RV, RV1, RV2,
			startX + size / 2, startY - size / 2, 0, mass, vx, vy, RV, RV1, RV2,
			startX + size / 2, startY + size / 2, 0, mass, vx, vy, RV, RV1, RV2,
																		       
			startX - size / 2, startY - size / 2, 0, mass, vx, vy, RV, RV1, RV2,
			startX + size / 2, startY + size / 2, 0, mass, vx, vy, RV, RV1, RV2,
			startX - size / 2, startY + size / 2, 0, mass, vx, vy, RV, RV1, RV2,
		};

		for (float v : vertices)
			particles.push_back(v);
	}

	// VBO 생성 및 데이터 전송하기
	glGenBuffers(1, &m_VBOParticle);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOParticle);
	glBufferData(GL_ARRAY_BUFFER, particles.size(), particles.data(), GL_STATIC_DRAW);
}

void Renderer::DrawParticles()
{
	g_time += 0.0003;

	glUseProgram(m_TriangleShader);

	int uTime = glGetUniformLocation(m_TriangleShader, "u_Time");
	glUniform1f(uTime, g_time);

	int attribPosition = glGetAttribLocation(m_TriangleShader, "a_Position");
	int attribMass = glGetAttribLocation(m_TriangleShader, "a_Mass");
	int attribVel = glGetAttribLocation(m_TriangleShader, "a_Vel");
	int attribRV = glGetAttribLocation(m_TriangleShader, "a_RV");
	int attribRV1 = glGetAttribLocation(m_TriangleShader, "a_RV1");
	int attribRV2 = glGetAttribLocation(m_TriangleShader, "a_RV2");

	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(attribMass);
	glEnableVertexAttribArray(attribVel);
	glEnableVertexAttribArray(attribRV);
	glEnableVertexAttribArray(attribRV1);
	glEnableVertexAttribArray(attribRV2);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOParticle);

	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, 0);
	glVertexAttribPointer(attribMass, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(attribVel, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (GLvoid*)(sizeof(float) * 4));
	glVertexAttribPointer(attribRV, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (GLvoid*)(sizeof(float) * 6));
	glVertexAttribPointer(attribRV1, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (GLvoid*)(sizeof(float) * 7));
	glVertexAttribPointer(attribRV2, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (GLvoid*)(sizeof(float) * 8));

	// m_ParticleCount * 6 (사각형 하나당 삼각형 2개 = 점 6개)
	glDrawArrays(GL_TRIANGLES, 0, m_ParticleCount * 6);
}

void Renderer::DrawFS()
{
	g_time += 0.0003;

	//Program select
	glUseProgram(m_FSShader);

	int uTime = glGetUniformLocation(m_TriangleShader, "u_Time");
	glUniform1f(uTime, g_time);

	int attribPosition = glGetAttribLocation(m_FSShader, "a_Pos");
	int attribTPos = glGetAttribLocation(m_FSShader, "a_TPos");

	// Enable -> 사용할 수 있도록 
	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(attribTPos);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOFS);

	// GPU에게 데이터의 구조를 알려주기
	// a_Position: 시작점 0부터 float 3개 읽어라
	glVertexAttribPointer(attribPosition, 3/*몇 개 읽음*/, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0/*시작점 주소*/);
	glVertexAttribPointer(attribTPos, 2/*몇 개 읽음*/, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::GetGLPosition(float x, float y, float *newX, float *newY)
{
	*newX = x * 2.f / m_WindowSizeX;
	*newY = y * 2.f / m_WindowSizeY;
}

void Renderer::GenParticle(int count)
{
}
