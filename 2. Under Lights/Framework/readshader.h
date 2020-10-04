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

GLuint Complie_Shaders() //셰이더 컴파일 하기 함수 ============================
{
	GLchar* VertexSource, *FragmentSource; // 소스코드 저장 변수
	GLuint VertexShader, FragmentShader;   // 셰이더

	GLuint ShaderProgram;                  // 셰이더 프로그램

	// [셰이더 프로그램 만들기]================================================
	VertexSource = FileToBuf("vertex.glvs");
	FragmentSource = FileToBuf("fragment.glfs");

	// [셰이더 객체 만들기]====================================================
	VertexShader = glCreateShader(GL_VERTEX_SHADER);
	FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// [셰이더 코드를 셰이더 객체에 넣기: GL로 보내진다.]======================
	glShaderSource(VertexShader, 1, (const GLchar**)&VertexSource, NULL);
	glCompileShader(VertexShader); //버텍스 셰이더 컴파일 하기

	glShaderSource(FragmentShader, 1, (const GLchar**)&FragmentSource, NULL);
	glCompileShader(FragmentShader); //버텍스 셰이더 컴파일 하기

	// VS, FS가 컴파일 됐고, 에러가 없는 경우 GL 셰이더 객체를 만들어 두 셰이더를 링크한다. 
	ShaderProgram = glCreateProgram();
	glAttachShader(ShaderProgram, VertexShader);
	glAttachShader(ShaderProgram, FragmentShader);

	glBindAttribLocation(ShaderProgram, 0, "vPos");
	glBindAttribLocation(ShaderProgram, 1, "vColor");

	// 프로그램 링크, 셰이더 프로그램은 에러 없이 바이너리 코드가 셰이더를 위하여 
	// 생성되고 그 코드가 GPU에 업로드 된다. 
	glLinkProgram(ShaderProgram);

	// 링크가 되었는지 체크하기 
	GLint isLinked; 
	//GLchar errorLog[512];
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &isLinked);

	return ShaderProgram;
}


