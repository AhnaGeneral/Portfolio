#pragma once 
#include "stdafx.h"
//fopen_s error

char* FileToBuf(const char * TargetFile)
{
	FILE *fptr; 
	long length; char *buf;
	fopen_s(&fptr,TargetFile, "rb"); // Open file for reading 
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

GLuint Complie_Shaders() //���̴� ������ �ϱ� �Լ� ============================
{
	GLchar* VertexSource, *FragmentSource; // �ҽ��ڵ� ���� ����
	GLuint VertexShader, FragmentShader;   // ���̴�

	GLuint ShaderProgram;                  // ���̴� ���α׷�

	// [���̴� ���α׷� �����]================================================
	VertexSource = FileToBuf("vertex.glvs");
	FragmentSource = FileToBuf("fragment.glfs");

	// [���̴� ��ü �����]====================================================
	VertexShader = glCreateShader(GL_VERTEX_SHADER);
	FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// [���̴� �ڵ带 ���̴� ��ü�� �ֱ�: GL�� ��������.]======================
	glShaderSource(VertexShader, 1, (const GLchar**)&VertexSource, NULL);
	glCompileShader(VertexShader); //���ؽ� ���̴� ������ �ϱ�

	glShaderSource(FragmentShader, 1, (const GLchar**)&FragmentSource, NULL);
	glCompileShader(FragmentShader); //���ؽ� ���̴� ������ �ϱ�

	// VS, FS�� ������ �ư�, ������ ���� ��� GL ���̴� ��ü�� ����� �� ���̴��� ��ũ�Ѵ�. 
	ShaderProgram = glCreateProgram();
	glAttachShader(ShaderProgram, VertexShader);
	glAttachShader(ShaderProgram, FragmentShader);

	glBindAttribLocation(ShaderProgram, 0, "vPos");
	glBindAttribLocation(ShaderProgram, 1, "vColor");

	// ���α׷� ��ũ, ���̴� ���α׷��� ���� ���� ���̳ʸ� �ڵ尡 ���̴��� ���Ͽ� 
	// �����ǰ� �� �ڵ尡 GPU�� ���ε� �ȴ�. 
	glLinkProgram(ShaderProgram);

	// ��ũ�� �Ǿ����� üũ�ϱ� 
	GLint isLinked; 
	//GLchar errorLog[512];
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &isLinked);

	return ShaderProgram;
}


