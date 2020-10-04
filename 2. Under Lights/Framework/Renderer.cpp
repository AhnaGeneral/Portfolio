#include"stdafx.h"
#include "Renderer.h"

char* filetobuf(const char * TargetFile)
{
	FILE *fptr;
	long length; char *buf;
	fopen_s(&fptr, TargetFile, "rb"); // Open file for reading 
	if (!fptr) // Return NULL on failure
		return NULL; fseek(fptr, 0, SEEK_END); // Seek to the end of the file 
	length = ftell(fptr); // Find out how many bytes into the file we are
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator 
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file 
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer 
	fclose(fptr); // Close the file 
	buf[length] = 0; // Null terminator 

	return buf; // Return the buffer 
}



Renderer::Renderer(InitDesc initDesc)
{
	this->initDesc = initDesc;
}


void Renderer::SetViewMatrix(const glm::vec3& cameraPosition,const glm::vec3& lookDir,const glm::vec3& cameraUp)
{
	this->cameraPosition = cameraPosition;
	this->lookDir = lookDir;
	this->cameraUp = cameraUp;

	viewMatrix = glm::lookAt(
		cameraPosition, // ī�޶�� (4,3,3) �� �ִ�. ���� ��ǥ����
		lookDir, // �׸��� ī�޶� ������ ����
		cameraUp  // �Ӹ��� �����̴� (0,-1,0 ���� �غ���, ������ �����̴�)
	);
}


void Renderer::SetProjMatrix(float fov,float nearZ,float farZ)
{
	projMatrix = glm::perspective(
		glm::radians(fov),  // �������� �þ߰��Դϴ� : "��"�� ũ��. "ī�޶� ����" �� �����غ�����. �̵��� ���� 90�� (����Ʈ�� ���̵�) ���� 30�� (ũ�� Ȯ���� ���) ���̿� �ֽ��ϴ�
		initDesc.width/initDesc.height, // ȭ�� �� �Դϴ�. �̰��� ����� ������ ũ�⿡ �����մϴ�. 4/3 == 800/600 == 1280/960 �ε�, ��� ���� ���� ?
		nearZ,        // Near clipping plane (�ٰŸ� �߶󳻱� ���). �ִ��� ũ�� �ϼ���. �ƴϸ� ��Ȯ�� ������ ���� �� �ֽ��ϴ�.
		farZ       // Far clipping plane (���Ÿ� �߶󳻱� ���). �ִ��� �۰� �ϼ���.
	);
}

bool Renderer::AddShaderWithTwoParam(const string& vertexShaderPath, const string & pixelShaderPath, const string& shaderName)
{
	GLuint ShaderProgramID;

	auto vertexsource = filetobuf(vertexShaderPath.c_str());
	auto fragmentsource = filetobuf(pixelShaderPath.c_str());

	//---���ؽ����̴��о������ϰ��������ϱ� 
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexsource, NULL);
	glCompileShader(vertexShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		cerr << "ERROR: vertex shader �����Ͻ���\n" << errorLog << endl;
		return false;
	}

	//---�����׸�Ʈ���̴��о������ϰ��������ϱ�
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentsource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		cerr << "ERROR: fragment shader �����Ͻ���\n" << errorLog << endl;
		return false;
	}
	ShaderProgramID = glCreateProgram(); //---���̴����α׷������
	glAttachShader(ShaderProgramID, vertexShader); // ���̴����α׷������ؽ����̴����̱� 
	glAttachShader(ShaderProgramID, fragmentShader); // ���̴����α׷��������׸�Ʈ���̴����̱�

	glBindAttribLocation(ShaderProgramID, 0, "in_Position");
	glBindAttribLocation(ShaderProgramID, 1, "in_Color");


	glLinkProgram(ShaderProgramID); // ���̴����α׷���ũ�ϱ�
	glDeleteShader(vertexShader); // ���̴����α׷�����ũ�Ͽ����̴���ü��ü�»������� 
	glDeleteShader(fragmentShader);
	glGetProgramiv(ShaderProgramID, GL_LINK_STATUS, &result); // ���̴����߿���Ǿ�����üũ�ϱ� 
	if (!result) {
		glGetProgramInfoLog(ShaderProgramID, 512, NULL, errorLog);
		cerr << "ERROR: shader program �������\n" << errorLog << endl;
		return false;
	}

	shaderProgramMap[shaderName] = ShaderProgramID;

	return true;
}

bool Renderer::AddShaderWithFourParam(const string & vertexShaderPath, const string & pixelShaderPath, const string & shaderName)
{
	GLuint ShaderProgramID;

	auto vertexsource = filetobuf(vertexShaderPath.c_str());
	auto fragmentsource = filetobuf(pixelShaderPath.c_str());

	//---���ؽ����̴��о������ϰ��������ϱ� 
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexsource, NULL);
	glCompileShader(vertexShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		cerr << "ERROR: vertex shader �����Ͻ���\n" << errorLog << endl;
		return false;
	}

	//---�����׸�Ʈ���̴��о������ϰ��������ϱ�
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentsource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		cerr << "ERROR: fragment shader �����Ͻ���\n" << errorLog << endl;
		return false;
	}
	ShaderProgramID = glCreateProgram(); //---���̴����α׷������
	glAttachShader(ShaderProgramID, vertexShader); // ���̴����α׷������ؽ����̴����̱� 
	glAttachShader(ShaderProgramID, fragmentShader); // ���̴����α׷��������׸�Ʈ���̴����̱�

	glBindAttribLocation(ShaderProgramID, 0, "in_Position");
	glBindAttribLocation(ShaderProgramID, 1, "in_TexCoord");
	glBindAttribLocation(ShaderProgramID, 2, "in_Normal");
	glBindAttribLocation(ShaderProgramID, 3, "in_Color");


	glLinkProgram(ShaderProgramID); // ���̴����α׷���ũ�ϱ�
	glDeleteShader(vertexShader); // ���̴����α׷�����ũ�Ͽ����̴���ü��ü�»������� 
	glDeleteShader(fragmentShader);
	glGetProgramiv(ShaderProgramID, GL_LINK_STATUS, &result); // ���̴����߿���Ǿ�����üũ�ϱ� 
	if (!result) {
		glGetProgramInfoLog(ShaderProgramID, 512, NULL, errorLog);
		cerr << "ERROR: shader program �������\n" << errorLog << endl;
		return false;
	}

	shaderProgramMap[shaderName] = ShaderProgramID;

	return true;
}

Renderer::~Renderer()
{
}

void Renderer::Draw(const glm::mat4& worldMatrix,const int primitiveType,const GLuint vao, const int count)
{
	glm::mat4 finalMatrix =projMatrix * viewMatrix *  worldMatrix;

	unsigned int modelLocation = glGetUniformLocation(currentShaderID, "u_finalMatrix");  //---���ؽ����̴������𵨺�ȯ��ġ�������� 
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(finalMatrix));

	glBindVertexArray(vao);
	glDrawArrays(primitiveType, 0, count);
}

void Renderer::Draw(const glm::mat4 & worldMatrix, const int primitiveType, const GLuint vao, const int count, const glm::vec4& color)
{
	glm::mat4 finalMatrix = projMatrix * viewMatrix *  worldMatrix;

	unsigned int location = glGetUniformLocation(currentShaderID, "u_finalMatrix");  //---���ؽ����̴������𵨺�ȯ��ġ�������� 
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(finalMatrix));

	 location = glGetUniformLocation(currentShaderID, "u_worldMatrix");  //---���ؽ����̴������𵨺�ȯ��ġ�������� 
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(worldMatrix));

	location = glGetUniformLocation(currentShaderID, "u_color");
	glUniform4f(location, color.r, color.g, color.b, color.a);


	location = glGetUniformLocation(currentShaderID, "u_sunLight");
	glUniform3fv(location,sizeof(glm::vec3), glm::value_ptr(light.sunLight));


	location = glGetUniformLocation(currentShaderID, "u_ambientLight");
	glUniform1f(location, light.ambientLight);


	glBindVertexArray(vao);
	glDrawArrays(primitiveType, 0, count);
}

