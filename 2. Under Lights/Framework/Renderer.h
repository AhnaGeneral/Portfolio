#pragma once
#include"Light.h"

struct InitDesc
{
	float width;
	float height;
};

class Renderer
{
public:
	Renderer(InitDesc initDesc);

	void SetViewMatrix(const glm::vec3& cameraPosition, const glm::vec3& lookDir, const glm::vec3& cameraUp);
	void SetProjMatrix( float fov ,  float nearZ,  float farZ);
	void SetCurrentShader(const char* shaderName) {glUseProgram(shaderProgramMap[shaderName]); currentShaderID = shaderProgramMap[shaderName]; };

	bool AddShaderWithTwoParam(const string& vertexShaderPath, const string& pixelShaderPath, const string& shaderName);
	bool AddShaderWithFourParam(const string& vertexShaderPath, const string& pixelShaderPath, const string& shaderName);
	~Renderer();

	void Draw(const glm::mat4& worldMatrix, const int primitiveType, const GLuint vao, const int count);
	void Draw(const glm::mat4& worldMatrix, const int primitiveType, const GLuint vao, const int count,const glm::vec4& color);

public:
	InitDesc initDesc;

	glm::mat4 viewMatrix = glm::mat4{};
	glm::mat4 projMatrix = glm::mat4{};

	unordered_map<string, GLuint> shaderProgramMap;

	Light light; //빛 관련 변수들

private:
	GLuint currentShaderID;

	glm::vec3 cameraPosition;
	glm::vec3 lookDir;
	glm::vec3 cameraUp;
};
