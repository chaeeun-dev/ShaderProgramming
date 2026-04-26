#pragma once

#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include "Dependencies\glew.h"

class Renderer
{
public:
	Renderer(int windowSizeX, int windowSizeY);
	~Renderer();

	bool IsInitialized();
	void DrawSolidRect(float x, float y, float z, float size, float r, float g, float b, float a);
	void DrawTriangle();

	void CreateParticle(const int num);
	void DrawParticles();
	void DrawFS();

private:
	void Initialize(int windowSizeX, int windowSizeY);
	GLuint CreatePngTexture(char* filePath, GLuint samplingMethod);
	bool ReadFile(char* filename, std::string *target);
	void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
	GLuint CompileShaders(char* filenameVS, char* filenameFS);
	void CreateVertexBufferObjects();
	void CreateVertexBufferObjects2();
	void GetGLPosition(float x, float y, float *newX, float *newY);
	void GenParticle(int count);

	bool m_Initialized = false;
	
	unsigned int m_WindowSizeX = 0;
	unsigned int m_WindowSizeY = 0;

	GLuint m_VBORect = 0;
	GLuint m_SolidRectShader = 0;

	GLuint m_VBOTriangle = 0;
	GLuint m_TriangleShader = 0;

	GLuint m_VBOParticle = 0;
	int m_ParticleCount = 0;

	// Fragment Shader
	GLuint m_VBOFS = 0;
	GLuint m_FSShader = 0;

	// RainDrops
	float m_DropPoints[1000 * 4];

	// Texture
	GLuint m_RGBTexture = 0;
	GLuint m_NumTexture[10];
	GLuint m_NumsTexture = 0;
};

