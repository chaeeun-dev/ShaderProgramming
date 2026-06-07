#include "stdafx.h"
#include "Renderer.h"
#include "LoadPng.h"
#include <assert.h>
#include "Windows.h"

Renderer::Renderer(int windowSizeX, int windowSizeY)
{
	Initialize(windowSizeX, windowSizeY);
}


Renderer::~Renderer()
{
}

void Renderer::Initialize(int windowSizeX, int windowSizeY)
{
	std::srand((unsigned int)std::time(nullptr));

	//Set window size
	m_WindowSizeX = windowSizeX;
	m_WindowSizeY = windowSizeY;

	//Load shaders
	m_SolidRectShader = CompileShaders(
		"./Shaders/SolidRect.vs", 
		"./Shaders/SolidRect.fs");
	m_TriangleShader = CompileShaders(
		"./Shaders/Triangle.vs",
		"./Shaders/Triangle.fs");
	m_FSShader = CompileShaders(
		"./Shaders/FS.vs",
		"./Shaders/FS.fs");
	m_DummyShader = CompileShaders(
		"./Shaders/Dummy.vs",
		"./Shaders/Dummy.fs");
	m_TextureShader = CompileShaders(
		"./Shaders/Texture.vs",
		"./Shaders/Texture.fs");

	//Load Textures
	m_RgbTexture = CreatePngTexture("./textures/rgb.png", GL_NEAREST); //0 slot
	m_NumsTexture = CreatePngTexture("./textures/numbers.png", GL_NEAREST); //1slot
	m_ParticleTexture = CreatePngTexture("./textures/particle.png", GL_NEAREST); 
	m_ParticleSpriteTexture = CreatePngTexture("./textures/explosion.png", GL_NEAREST);
	m_TacopiTexture = CreatePngTexture("./textures/tacopi.png", GL_NEAREST);

	for (int i = 0; i < 10; i++)
	{
		std::string path = "./textures/" + std::to_string(i) + ".png";
		m_NumTexture[i] = CreatePngTexture((char*)path.c_str(), GL_NEAREST); //2~11slot
	}

	//Create VBOs
	CreateVertexBufferObjects();

	// Create Dummy
	GenDummyMesh(100, 100);

	GenParticles(1000);

	GenFBOs();

	//Gen Drop Info
	int index = 0;
	for (int i = 0; i < 1000; i++)
	{
		float x = (float)rand() / (float)RAND_MAX;
		float y = (float)rand() / (float)RAND_MAX;
		float sTime = 3*(float)rand() / (float)RAND_MAX;
		float lTime = (float)rand() / (float)RAND_MAX;

		m_DropPoints[index] = x; index++;
		m_DropPoints[index] = y; index++;
		m_DropPoints[index] = sTime; index++;
		m_DropPoints[index] = lTime; index++;
	}

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

	glGenBuffers(1, &m_VBORect);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);

	float centerX = 0;
	float centerY = 0;
	float size = 0.1;
	float mass = 1;
	float vx = 1;
	float vy = 3;
	float triangle[]
		=
	{
		centerX - size / 2, centerY - size / 2, 0, 
		mass, vx, vy,
		centerX + size / 2, centerY - size / 2, 0, 
		mass, vx, vy,
		centerX + size / 2, centerY + size / 2, 0, 
		mass, vx, vy,//triangle1

		centerX - size / 2, centerY - size / 2, 0, 
		mass, vx, vy,
		centerX + size / 2, centerY + size / 2, 0, 
		mass, vx, vy,
		centerX - size / 2, centerY + size / 2, 0, 
		mass, vx, vy//triangle2
	};
	glGenBuffers(1, &m_VBOTriangle);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOTriangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle,
		GL_STATIC_DRAW);


	float rectFS[] //x, y, z, tx, ty : stride 5
		=
	{
		-1.f, -1.f, 0.f, 0, 1,
		 1.f,  1.f, 0.f, 1, 0,
		-1.f,  1.f, 0.f, 0, 0,//Triangle1

		-1.f, -1.f, 0.f, 0, 1,
		 1.f, -1.f, 0.f, 1, 1, 
		 1.f,  1.f, 0.f, 1, 0//Triangle2
	};	
	
	float texRect[] //x, y, z, tx, ty : stride 5
		=
	{
		-1.f, -1.f, 0.f,
		-1.f,  1.f, 0.f,
		1.f,  1.f, 0.f,	//Triangle1

		-1.f, -1.f, 0.f,
		 1.f, 1.f, 0.f, 
		 1.f, -1.f, 0.f	//Triangle2
	};

	glGenBuffers(1, &m_TextureVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_TextureVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texRect),
		texRect, GL_STATIC_DRAW);

	glGenBuffers(1, &m_VBOFS);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOFS);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectFS), 
		rectFS, GL_STATIC_DRAW);

}

GLuint Renderer::CreatePngTexture(char* filePath, GLuint samplingMethod)

{
	//Load Png
	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, filePath);

	if (error != 0)
	{
		std::cout << "PNG image loading failed:" << filePath << std::endl;
		assert(0);
	}

	GLuint temp;

	glGenTextures(1, &temp);
	glBindTexture(GL_TEXTURE_2D, temp);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, &image[0]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, samplingMethod);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, samplingMethod);

	return temp;

}

void Renderer::GenDummyMesh(int resolX, int resolY)
{
	float basePosX = -0.5f;
	float basePosY = -0.5f;
	float targetPosX = 0.5f;
	float targetPosY = 0.5f;
	int pointCountX = resolX;
	int pointCountY = resolY;

	float width = targetPosX - basePosX;
	float height = targetPosY - basePosY;
	float* point = new float[pointCountX * pointCountY * 2];
	float* vertices = new float[(pointCountX - 1) * (pointCountY - 1) * 2 * 3 * 3];

	m_VBODummyCount = (pointCountX - 1) * (pointCountY - 1) * 2 * 3;

	//Prepare points

	for (int x = 0; x < pointCountX; x++)

	{
		for (int  y = 0; y < pointCountY; y++)
		{
			point[(y * pointCountX + x) * 2 + 0] = basePosX + width * (x / (float)(pointCountX - 1));
			point[(y * pointCountX + x) * 2 + 1] = basePosY + height * (y / (float)(pointCountY - 1));
		}
	}

	//Make triangles
	int vertIndex = 0;
	for (int x = 0; x < pointCountX - 1; x++)
	{
		for (int y = 0; y < pointCountY - 1; y++)
		{
			//Triangle part 1

			vertices[vertIndex] = point[(y * pointCountX + x) * 2 + 0];
			vertIndex++;

			vertices[vertIndex] = point[(y * pointCountX + x) * 2 + 1];
			vertIndex++;

			vertices[vertIndex] = 0.f;
			vertIndex++;

			vertices[vertIndex] = point[((y + 1) * pointCountX + (x + 1)) * 2 + 0];
			vertIndex++;

			vertices[vertIndex] = point[((y + 1) * pointCountX + (x + 1)) * 2 + 1];
			vertIndex++;

			vertices[vertIndex] = 0.f;
			vertIndex++;

			vertices[vertIndex] = point[((y + 1) * pointCountX + x) * 2 + 0];
			vertIndex++;

			vertices[vertIndex] = point[((y + 1) * pointCountX + x) * 2 + 1];
			vertIndex++;

			vertices[vertIndex] = 0.f;
			vertIndex++;

			//Triangle part 2
			vertices[vertIndex] = point[(y * pointCountX + x) * 2 + 0];
			vertIndex++;

			vertices[vertIndex] = point[(y * pointCountX + x) * 2 + 1];
			vertIndex++;

			vertices[vertIndex] = 0.f;
			vertIndex++;

			vertices[vertIndex] = point[(y * pointCountX + (x + 1)) * 2 + 0];
			vertIndex++;

			vertices[vertIndex] = point[(y * pointCountX + (x + 1)) * 2 + 1];
			vertIndex++;

			vertices[vertIndex] = 0.f;
			vertIndex++;

			vertices[vertIndex] = point[((y + 1) * pointCountX + (x + 1)) * 2 + 0];
			vertIndex++;

			vertices[vertIndex] = point[((y + 1) * pointCountX + (x + 1)) * 2 + 1];
			vertIndex++;

			vertices[vertIndex] = 0.f;
			vertIndex++;
		}
	}

	glGenBuffers(1, &m_VBODummy);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBODummy);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (pointCountX - 1) * (pointCountY - 1) * 2 * 3 * 3, vertices, GL_STATIC_DRAW);
}

void Renderer::DrawTexture(GLuint texID, float x, float y, float scale, bool flip)
{
	int shader = m_TextureShader;
	glUseProgram(shader);

	int uTex = glGetUniformLocation(shader, "u_Tex");
	glUniform1i(uTex, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);

	int uTrans = glGetUniformLocation(shader, "u_Trans");
	glUniform4f(uTrans, x, y, 1, scale);

	int uFlip = glGetUniformLocation(shader, "u_Flip");
	glUniform1i(uFlip, flip);

	int aPos = glGetAttribLocation(shader, "a_Pos");
	glEnableVertexAttribArray(aPos);
	glBindBuffer(GL_ARRAY_BUFFER, m_TextureVBO);
	glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(aPos);
}

void Renderer::GenFBOs()
{
	//Gen Texture
	glGenTextures(1, &m_FBO_Texture);
	glBindTexture(GL_TEXTURE_2D, m_FBO_Texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 340, 340, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	//Gen Render Buffer
	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 340, 340);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//GenFBO
	glGenFramebuffers(1, &m_FBO);
	//Binding before Attach
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	//Attach Color Buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FBO_Texture, 0);
	//Attach Depth Buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	//Check status
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		assert(0);
	}

	//Gen Texture
	glGenTextures(1, &m_FBO_Texture1);
	glBindTexture(GL_TEXTURE_2D, m_FBO_Texture1);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 340, 340, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	//Gen Render Buffer
	GLuint depthBuffer1;
	glGenRenderbuffers(1, &depthBuffer1);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer1);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 340, 340);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//GenFBO
	glGenFramebuffers(1, &m_FBO1);
	//Binding before Attach
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO1);
	//Attach Color Buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FBO_Texture1, 0);
	//Attach Depth Buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer1);

	//Check status
	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		assert(0);
	}

	//Gen Texture
	glGenTextures(1, &m_FBO_Texture2);
	glBindTexture(GL_TEXTURE_2D, m_FBO_Texture2);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 340, 340, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	//Gen Render Buffer
	GLuint depthBuffer2;
	glGenRenderbuffers(1, &depthBuffer2);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer2);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 340, 340);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//GenFBO
	glGenFramebuffers(1, &m_FBO2);
	//Binding before Attach
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO2);
	//Attach Color Buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FBO_Texture2, 0);
	//Attach Depth Buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer2);

	//Check status
	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		assert(0);
	}

	//Gen Texture
	glGenTextures(1, &m_MRT_FBO_Texture0);
	glBindTexture(GL_TEXTURE_2D, m_MRT_FBO_Texture0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 340, 340, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glGenTextures(1, &m_MRT_FBO_Texture1);
	glBindTexture(GL_TEXTURE_2D, m_MRT_FBO_Texture1);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 340, 340, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glGenTextures(1, &m_MRT_FBO_Texture2);
	glBindTexture(GL_TEXTURE_2D, m_MRT_FBO_Texture2);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 340, 340, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	//Gen Render Buffer
	GLuint MRTdepthBuffer;
	glGenRenderbuffers(1, &MRTdepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, MRTdepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 340, 340);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//GenFBO
	glGenFramebuffers(1, &m_MRT_FBO);
	//Binding before Attach
	glBindFramebuffer(GL_FRAMEBUFFER, m_MRT_FBO);
	//Attach Color Buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_MRT_FBO_Texture0, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_MRT_FBO_Texture1, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_MRT_FBO_Texture2, 0);
	//Attach Depth Buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, MRTdepthBuffer);

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		assert(0);
	}

	{
		// MRT_HDR
		glGenTextures(1, &m_MRT_HDR_FBO_High_Texture);
		glBindTexture(GL_TEXTURE_2D, m_MRT_HDR_FBO_High_Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1024, 1024, 0, GL_RGBA, GL_FLOAT, NULL);

		glGenTextures(1, &m_MRT_HDR_FBO_Low_Texture);
		glBindTexture(GL_TEXTURE_2D, m_MRT_HDR_FBO_Low_Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1024, 1024, 0, GL_RGBA, GL_FLOAT, NULL);

		glGenFramebuffers(1, &m_MRT_HDR_FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_MRT_HDR_FBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_MRT_HDR_FBO_Low_Texture, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_MRT_HDR_FBO_High_Texture, 0);

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			assert(0);
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::GenParticles(int count)
{
	if (count <= 0)
	{
		m_VBOParticleCount = 0;
		return;
	}

	const float size = 0.1f;
	const float mass = 1.0f;

	// 파티클 1개 = 정점 6개
	// 정점 1개 = x, y, z, mass, vx, vy, RV, RV1, RV2, tx, ty, r, g, b  -> 총 11개 float
	const int floatsPerVertex = 14;
	const int verticesPerParticle = 6;
	const int floatsPerParticle 
		= floatsPerVertex * verticesPerParticle;

	std::vector<float> particles;
	particles.reserve(count * floatsPerParticle);

	for (int i = 0; i < count; ++i)
	{
		// 중심 위치도 랜덤으로 주고 싶다면 여기서 바꿔도 됨
		float centerX = 0.0f;
		float centerY = 0.0f;

		// vx, vy 랜덤값 부여
		// 예: -3.0 ~ 3.0 범위
		float vx = -3.0f + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 6.0f;
		float vy = -3.0f + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 6.0f;

		float RV = static_cast<float>(std::rand()) 
			/ static_cast<float>(RAND_MAX); //0~1 random value
		float RV1 = static_cast<float>(std::rand())
			/ static_cast<float>(RAND_MAX); //0~1 random value
		float RV2 = static_cast<float>(std::rand())
			/ static_cast<float>(RAND_MAX); //0~1 random value

		float R = static_cast<float>(std::rand())
			/ static_cast<float>(RAND_MAX); //0~1 random value
		float G = static_cast<float>(std::rand())
			/ static_cast<float>(RAND_MAX); //0~1 random value
		float B = static_cast<float>(std::rand())
			/ static_cast<float>(RAND_MAX); //0~1 random value

		float left = centerX - size / 2.0f;
		float right = centerX + size / 2.0f;
		float bottom = centerY - size / 2.0f;
		float top = centerY + size / 2.0f;

		// triangle 1
		particles.push_back(left);  
		particles.push_back(bottom); 
		particles.push_back(0.0f);
		particles.push_back(mass);  
		particles.push_back(vx);     
		particles.push_back(vy);
		particles.push_back(RV);
		particles.push_back(RV1);
		particles.push_back(RV2);
		particles.push_back(0.f);
		particles.push_back(1.f);
		particles.push_back(R);
		particles.push_back(G);
		particles.push_back(B);

		particles.push_back(right); 
		particles.push_back(bottom); 
		particles.push_back(0.0f);
		particles.push_back(mass);  
		particles.push_back(vx);     
		particles.push_back(vy);
		particles.push_back(RV);
		particles.push_back(RV1);
		particles.push_back(RV2);
		particles.push_back(1.f);
		particles.push_back(1.f);
		particles.push_back(R);
		particles.push_back(G);
		particles.push_back(B);

		particles.push_back(right); 
		particles.push_back(top);    
		particles.push_back(0.0f);
		particles.push_back(mass);  
		particles.push_back(vx);     
		particles.push_back(vy);
		particles.push_back(RV);
		particles.push_back(RV1);
		particles.push_back(RV2);
		particles.push_back(1.f);
		particles.push_back(0.f);
		particles.push_back(R);
		particles.push_back(G);
		particles.push_back(B);

		// triangle 2
		particles.push_back(left);  
		particles.push_back(bottom); 
		particles.push_back(0.0f);
		particles.push_back(mass);  
		particles.push_back(vx);     
		particles.push_back(vy);
		particles.push_back(RV);
		particles.push_back(RV1);
		particles.push_back(RV2);
		particles.push_back(0.f);
		particles.push_back(1.f);
		particles.push_back(R);
		particles.push_back(G);
		particles.push_back(B);

		particles.push_back(right); 
		particles.push_back(top);    
		particles.push_back(0.0f);
		particles.push_back(mass);  
		particles.push_back(vx);     
		particles.push_back(vy);
		particles.push_back(RV);
		particles.push_back(RV1);
		particles.push_back(RV2);
		particles.push_back(1.f);
		particles.push_back(0.f);
		particles.push_back(R);
		particles.push_back(G);
		particles.push_back(B);

		particles.push_back(left);  
		particles.push_back(top);    
		particles.push_back(0.0f);
		particles.push_back(mass);  
		particles.push_back(vx);     
		particles.push_back(vy);
		particles.push_back(RV);
		particles.push_back(RV1);
		particles.push_back(RV2);
		particles.push_back(0.f);
		particles.push_back(0.f);
		particles.push_back(R);
		particles.push_back(G);
		particles.push_back(B);
	}

	if (m_VBOParticle == 0)
	{
		glGenBuffers(1, &m_VBOParticle);
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOParticle);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(float) * particles.size(),
		particles.data(),
		GL_STATIC_DRAW
	);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// 전체 생성된 버텍스 개수 저장
	m_VBOParticleCount = count * verticesPerParticle;
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
}

float g_time = 0;

void Renderer::DrawTriangle()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	g_time += 0.016;

	//Program select
	glUseProgram(m_TriangleShader);

	int uTime = glGetUniformLocation(
		m_TriangleShader, "u_Time"); 
	glUniform1f(uTime, g_time);
	int uParticle = glGetUniformLocation(
		m_TriangleShader, "u_ParticleTex");
	glUniform1i(uParticle, 0);
	int uParticleSprite = glGetUniformLocation(
		m_TriangleShader, "u_ParticleSpriteTex");
	glUniform1i(uParticleSprite, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_ParticleTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_ParticleSpriteTexture);

	int attribPosition = glGetAttribLocation(
		m_TriangleShader, "a_Pos");
	int attribMass = glGetAttribLocation(
		m_TriangleShader, "a_Mass");
	int attribVel = glGetAttribLocation(
		m_TriangleShader, "a_Vel");
	int attribRV = glGetAttribLocation(
		m_TriangleShader, "a_RV");
	int attribRV1 = glGetAttribLocation(
		m_TriangleShader, "a_RV1");
	int attribRV2 = glGetAttribLocation(
		m_TriangleShader, "a_RV2");
	int attribTex = glGetAttribLocation(
		m_TriangleShader, "a_Tex");
	int attribRGB = glGetAttribLocation(
		m_TriangleShader, "a_RGB");
	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(attribMass);
	glEnableVertexAttribArray(attribVel);
	glEnableVertexAttribArray(attribRV);
	glEnableVertexAttribArray(attribRV1);
	glEnableVertexAttribArray(attribRV2);
	glEnableVertexAttribArray(attribTex);
	glEnableVertexAttribArray(attribRGB);

	int stride = 14;

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOParticle);
	glVertexAttribPointer(attribPosition, 
		3, GL_FLOAT, 
		GL_FALSE, 
		sizeof(float) * stride, 0);
	glVertexAttribPointer(attribMass,
		1, GL_FLOAT,
		GL_FALSE,
		sizeof(float) * stride, (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(attribVel,
		2, GL_FLOAT,
		GL_FALSE,
		sizeof(float) * stride, (GLvoid*)(sizeof(float) * 4));
	glVertexAttribPointer(attribRV,
		1, GL_FLOAT,
		GL_FALSE,
		sizeof(float) * stride, (GLvoid*)(sizeof(float) * 6));
	glVertexAttribPointer(attribRV1,
		1, GL_FLOAT,
		GL_FALSE,
		sizeof(float) * stride, (GLvoid*)(sizeof(float) * 7));
	glVertexAttribPointer(attribRV2,
		1, GL_FLOAT,
		GL_FALSE,
		sizeof(float) * stride, (GLvoid*)(sizeof(float) * 8));
	glVertexAttribPointer(attribTex,
		2, GL_FLOAT,
		GL_FALSE,
		sizeof(float) * stride, (GLvoid*)(sizeof(float) * 9));
	glVertexAttribPointer(attribRGB,
		3, GL_FLOAT,
		GL_FALSE,
		sizeof(float) * stride, (GLvoid*)(sizeof(float) * 11));

	glDrawArrays(GL_TRIANGLES, 0, m_VBOParticleCount);

	glDisable(GL_BLEND);
}

void Renderer::DrawTriangle_Bloom()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_MRT_HDR_FBO);
	GLenum DrawBuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, DrawBuffers);
	glClearDepth(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, 1024, 1024);

	DrawTriangle();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1024, 1024);

	GLenum ResetDrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, ResetDrawBuffers);

	DrawTexture(m_MRT_HDR_FBO_Low_Texture, -0.5, 0, 0.5, false);
	DrawTexture(m_MRT_HDR_FBO_High_Texture, 0.5, 0, 0.5, false);
}

int g_CurrNum = 0;

void Renderer::DrawFS()
{
	g_time += 0.016;

	//Program select
	GLuint shader = m_FSShader;
	glUseProgram(shader);

	int uTime = glGetUniformLocation(
		shader, "u_Time");
	glUniform1f(uTime, g_time);

	int uPoints = glGetUniformLocation(
		shader, "u_DropInfo");
	glUniform4fv(uPoints, 1000, m_DropPoints);

	int uRGBTexture = glGetUniformLocation(shader,
		"u_RGBTex");
	glUniform1i(uRGBTexture, 0);

	int uCurrNumTexture = glGetUniformLocation(shader,
		"u_CurrNumTex");
	glUniform1i(uCurrNumTexture, g_CurrNum+2);
	g_CurrNum++;
	if (g_CurrNum > 9)
		g_CurrNum = 0;
	Sleep(500);

	int uInputNum = glGetUniformLocation(shader,
		"u_InputNum");
	glUniform1i(uInputNum, 9);

	int uNumsTexture = glGetUniformLocation(shader,
		"u_NumsTex");
	glUniform1i(uNumsTexture, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_RgbTexture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_NumsTexture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_NumTexture[0]);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_NumTexture[1]);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_NumTexture[2]);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_NumTexture[3]);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, m_NumTexture[4]);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, m_NumTexture[5]);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, m_NumTexture[6]);
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, m_NumTexture[7]);
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, m_NumTexture[8]);
	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, m_NumTexture[9]);

	int attribPosition = glGetAttribLocation(
		shader, "a_Pos");
	int attribTex =	glGetAttribLocation(
		shader, "a_Tex");
	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(attribTex);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOFS);
	glVertexAttribPointer(attribPosition,
		3, GL_FLOAT,
		GL_FALSE,
		sizeof(float) * 5, 0);
	glVertexAttribPointer(attribTex,
		2, GL_FLOAT,
		GL_FALSE,
		sizeof(float) * 5, (GLvoid*)(sizeof(float)*3));
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::DrawDummy()
{
	//Program select
	int shader = m_DummyShader;
	glUseProgram(shader);

	int uTime = glGetUniformLocation(shader, "u_Time");
	glUniform1f(uTime, g_time);
	g_time += 0.016;

	int uPoints = glGetUniformLocation(
		shader, "u_DropInfo");
	glUniform4fv(uPoints, 1000, m_DropPoints);

	int u_TacopiTex = glGetUniformLocation(shader, "u_TacopiTex");
	glUniform1i(u_TacopiTex, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_TacopiTexture);

	int attribPosition = glGetAttribLocation(shader, "a_Pos");
	glEnableVertexAttribArray(attribPosition);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBODummy);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	glDrawArrays(GL_TRIANGLES, 0, m_VBODummyCount);

	glDisableVertexAttribArray(attribPosition);

	DrawTexture(m_RgbTexture, 0.5, 0, 0.1, true);
}

void Renderer::DrawDummy_FBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glViewport(0, 0, 512, 512);
	DrawDummy();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1024, 1024);

	DrawTexture(m_FBO_Texture, 0, 0, 0.5, false);
}

void Renderer::DrawAll_FBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, 512, 512);
	DrawFS();

	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO1);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, 512, 512);
	DrawTriangle();

	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO2);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, 512, 512);
	DrawDummy();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1024, 1024);

	DrawTexture(m_FBO_Texture, -0.5, 0, 0.3, false);
	DrawTexture(m_FBO_Texture1, 0, 0, 0.3, false);
	DrawTexture(m_FBO_Texture2, 0.5, 0, 0.3, false);

}

void Renderer::DrawMultipleRenderTarget()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_MRT_FBO);
	GLenum DrawBuffers[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, DrawBuffers);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, 512, 512);

	DrawFS();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1024, 1024);

	GLenum ResetDrawBuffers[1] = { GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, ResetDrawBuffers);

	DrawTexture(m_MRT_FBO_Texture0, -0.5, 0, 0.3, false);
	DrawTexture(m_MRT_FBO_Texture1, 0.0, 0, 0.3, false);
	DrawTexture(m_MRT_FBO_Texture2, 0.5, 0, 0.3, false);
}

void Renderer::GetGLPosition(float x, float y, float *newX, float *newY)
{
	*newX = x * 2.f / m_WindowSizeX;
	*newY = y * 2.f / m_WindowSizeY;
}