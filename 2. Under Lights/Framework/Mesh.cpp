#include "stdafx.h"
#include "Mesh.h"
#include"Renderer.h"
#include <sstream>



void stringTokenize(vector<string>& result, string & s, char delim)
{
	string token;
	stringstream ss(s);

	while (getline(ss, token, delim)) {
		result.push_back(token);
	}
}

void Mesh::CreateCube()
{
	vector<glm::vec3> colors;
	vector<float> g_vertex_buffer_data = {
	-1.0f,-1.0f,-1.0f, // triangle 1 : begin
	-1.0f,-1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f, // triangle 1 : end
	1.0f, 1.0f,-1.0f, // triangle 2 : begin
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f, // triangle 2 : end
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f
	};

	size = g_vertex_buffer_data.size()/3;



	GLfloat g_color_buffer_data[] = {
	0.583f,  0.771f,  0.014f,
	0.609f,  0.115f,  0.436f,
	0.327f,  0.483f,  0.844f,
	0.822f,  0.569f,  0.201f,
	0.435f,  0.602f,  0.223f,
	0.310f,  0.747f,  0.185f,
	0.597f,  0.770f,  0.761f,
	0.559f,  0.436f,  0.730f,
	0.359f,  0.583f,  0.152f,
	0.483f,  0.596f,  0.789f,
	0.559f,  0.861f,  0.639f,
	0.195f,  0.548f,  0.859f,
	0.014f,  0.184f,  0.576f,
	0.771f,  0.328f,  0.970f,
	0.406f,  0.615f,  0.116f,
	0.676f,  0.977f,  0.133f,
	0.971f,  0.572f,  0.833f,
	0.140f,  0.616f,  0.489f,
	0.997f,  0.513f,  0.064f,
	0.945f,  0.719f,  0.592f,
	0.543f,  0.021f,  0.978f,
	0.279f,  0.317f,  0.505f,
	0.167f,  0.620f,  0.077f,
	0.347f,  0.857f,  0.137f,
	0.055f,  0.953f,  0.042f,
	0.714f,  0.505f,  0.345f,
	0.783f,  0.290f,  0.734f,
	0.722f,  0.645f,  0.174f,
	0.302f,  0.455f,  0.848f,
	0.225f,  0.587f,  0.040f,
	0.517f,  0.713f,  0.338f,
	0.053f,  0.959f,  0.120f,
	0.393f,  0.621f,  0.362f,
	0.673f,  0.211f,  0.457f,
	0.820f,  0.883f,  0.371f,
	0.982f,  0.099f,  0.879f
	};


	// VAO 를지정하고할당하기 
	glGenVertexArrays(1, &vao);
	// VAO를바인드하기 
	glBindVertexArray(vao);


	// 2개의 VBO를지정하고할당하기 
	glGenBuffers(2, vbo);
	//--- 1번째 VBO를활성화하여바인드하고, 버텍스속성 (좌표값)을저장 
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

	// 변수 diamond 에서버텍스데이터값을버퍼에복사한다.
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (size * 3) , &g_vertex_buffer_data[0], GL_STATIC_DRAW);
	// 좌표값을 attribute 인덱스 0번에명시한다: 버텍스당 3* float 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// attribute 인덱스 0번을사용가능하게함 
	glEnableVertexAttribArray(0);

	//---2번째 VBO를활성화하여바인드하고, 버텍스속성 (색상)을저장 
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	// 변수 colors에서버텍스색상을복사한다. 
	// colors 배열의사이즈: 9 *float 
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data[0]) * 108, &g_color_buffer_data[0], GL_STATIC_DRAW);
	// 색상값을 attribute 인덱스 1번에명시한다: 버텍스당3*float 
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// attribute 인덱스 1번을사용가능하게함. 
	glEnableVertexAttribArray(1);

	meshType = MeshType::BASIC_MESH;
}

void Mesh::CreatePyramid()
{
	vector<glm::vec3> colors;
	vector<float> g_vertex_buffer_data = {
		0,1,0,
		-1,-1,1,
		1,-1,1,
		0,1,0,
		-1,-1,1,
		0,-1,-1,
		0,1,0,
		0,-1,-1,
		1,-1,1,
		-1,-1,1,
		0,-1,-1,
		1,-1, 1
	};

	size = g_vertex_buffer_data.size() / 3;

	for (int i = 0; i < size; i += 3)
	{
		colors.push_back({ rand()/(float)RAND_MAX, rand() / (float)RAND_MAX, 1});
	}

	// VAO 를지정하고할당하기 
	glGenVertexArrays(1, &vao);
	// VAO를바인드하기 
	glBindVertexArray(vao);


	// 2개의 VBO를지정하고할당하기 
	glGenBuffers(2, vbo);
	//--- 1번째 VBO를활성화하여바인드하고, 버텍스속성 (좌표값)을저장 
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

	// 변수 diamond 에서버텍스데이터값을버퍼에복사한다.
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (size * 3), &g_vertex_buffer_data[0], GL_STATIC_DRAW);
	// 좌표값을 attribute 인덱스 0번에명시한다: 버텍스당 3* float 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// attribute 인덱스 0번을사용가능하게함 
	glEnableVertexAttribArray(0);

	//---2번째 VBO를활성화하여바인드하고, 버텍스속성 (색상)을저장 
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	// 변수 colors에서버텍스색상을복사한다. 
	// colors 배열의사이즈: 9 *float 
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors[0]) * colors.size(), &colors[0], GL_STATIC_DRAW);
	// 색상값을 attribute 인덱스 1번에명시한다: 버텍스당3*float 
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// attribute 인덱스 1번을사용가능하게함. 
	glEnableVertexAttribArray(1);

	meshType = MeshType::BASIC_MESH;
}

void Mesh::CreateTriangle()
{
	vector<glm::vec3> position;
	vector<glm::vec3> colors;

	position.push_back({ 0,0.5,0 });
	position.push_back({ -0.5,0,0 });
	position.push_back({ 0.5,0,0 });
	

	size = position.size();

	for (int i = 0; i < size; i += 3)
	{
		colors.push_back({ rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, 1 });
	}

	// VAO 를지정하고할당하기 
	glGenVertexArrays(1, &vao);
	// VAO를바인드하기 
	glBindVertexArray(vao);

	// 2개의 VBO를지정하고할당하기 
	glGenBuffers(2, vbo);
	//--- 1번째 VBO를활성화하여바인드하고, 버텍스속성 (좌표값)을저장 
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

	// 변수 diamond 에서버텍스데이터값을버퍼에복사한다.
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * (size), &position[0], GL_STATIC_DRAW);
	// 좌표값을 attribute 인덱스 0번에명시한다: 버텍스당 3* float 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// attribute 인덱스 0번을사용가능하게함 
	glEnableVertexAttribArray(0);

	//---2번째 VBO를활성화하여바인드하고, 버텍스속성 (색상)을저장 
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	// 변수 colors에서버텍스색상을복사한다. 
	// colors 배열의사이즈: 9 *float 
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors[0]) * colors.size(), &colors[0], GL_STATIC_DRAW);
	// 색상값을 attribute 인덱스 1번에명시한다: 버텍스당3*float 
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// attribute 인덱스 1번을사용가능하게함. 
	glEnableVertexAttribArray(1);

	VertexData temp;

	for (auto& data : position)
	{
		temp.position = data;
		vertices.push_back(temp);
	}

	meshType = MeshType::BASIC_MESH;
}

void Mesh::CreateRectangle()
{
	vector<glm::vec3> position;
	vector<glm::vec3> colors;

	position.push_back({ -0.5,0.5,0 });
	position.push_back({ -0.5,-0.5,0 });
	position.push_back({ 0.5,0.5,0 });
	position.push_back({ 0.5,0.5,0 });
	position.push_back({ -0.5,-0.5,0 });
	position.push_back({ 0.5,-0.5,0 });
	
	size = position.size();



	for (int i = 0; i < size; i += 1)
	{
		colors.push_back({ 1,0.5,0 });
	}

	// VAO 를지정하고할당하기 
	glGenVertexArrays(1, &vao);
	// VAO를바인드하기 
	glBindVertexArray(vao);

	// 2개의 VBO를지정하고할당하기 
	glGenBuffers(2, vbo);
	//--- 1번째 VBO를활성화하여바인드하고, 버텍스속성 (좌표값)을저장 
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

	// 변수 diamond 에서버텍스데이터값을버퍼에복사한다.
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * (size), &position[0], GL_STATIC_DRAW);
	// 좌표값을 attribute 인덱스 0번에명시한다: 버텍스당 3* float 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// attribute 인덱스 0번을사용가능하게함 
	glEnableVertexAttribArray(0);

	//---2번째 VBO를활성화하여바인드하고, 버텍스속성 (색상)을저장 
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	// 변수 colors에서버텍스색상을복사한다. 
	// colors 배열의사이즈: 9 *float 
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors[0]) * colors.size(), &colors[0], GL_STATIC_DRAW);
	// 색상값을 attribute 인덱스 1번에명시한다: 버텍스당3*float 
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// attribute 인덱스 1번을사용가능하게함. 
	glEnableVertexAttribArray(1);

	VertexData temp;

	for (auto& data : position)
	{
		temp.position = data;
		vertices.push_back(temp);
	}

	meshType = MeshType::BASIC_MESH;
}

void Mesh::CreateMeshByVertices(const vector<glm::vec3>& vertex, const vector<glm::vec3>& colors)
{

	// VAO 를지정하고할당하기 
	glGenVertexArrays(1, &vao);
	// VAO를바인드하기 
	glBindVertexArray(vao);

	// 2개의 VBO를지정하고할당하기 
	glGenBuffers(2, vbo);
	//--- 1번째 VBO를활성화하여바인드하고, 버텍스속성 (좌표값)을저장 
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

	// 변수 diamond 에서버텍스데이터값을버퍼에복사한다.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex[0]) * (size), &vertex[0], GL_STATIC_DRAW);
	// 좌표값을 attribute 인덱스 0번에명시한다: 버텍스당 3* float 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// attribute 인덱스 0번을사용가능하게함 
	glEnableVertexAttribArray(0);

	//---2번째 VBO를활성화하여바인드하고, 버텍스속성 (색상)을저장 
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	// 변수 colors에서버텍스색상을복사한다. 
	// colors 배열의사이즈: 9 *float 
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors[0]) * colors.size(), &colors[0], GL_STATIC_DRAW);
	// 색상값을 attribute 인덱스 1번에명시한다: 버텍스당3*float 
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// attribute 인덱스 1번을사용가능하게함. 
	glEnableVertexAttribArray(1);

	
	VertexData temp;

	for (auto& data : vertex)
	{
		temp.position = data;
		vertices.push_back(temp);
	}

	meshType = MeshType::BASIC_MESH;
}

bool Mesh::CreateMeshByObj(const char * path)
{
	ifstream in(path);

	if (!in.is_open())
		return false;

	vector<glm::vec3> position;
	vector<glm::vec3> normal;
	vector<glm::vec2> uv;

	vector<GLuint> indices;


	string ignore;

	while (ignore != "v")
	{
		in >> ignore;
	}

	glm::vec3 tempVec3;
	glm::vec2 tempVec2;

	in >> tempVec3.x >> tempVec3.y >> tempVec3.z;
	position.push_back(tempVec3);


	while (true)
	{
		in >> ignore;
		if (ignore != "v")
			break;
		in >> tempVec3.x >> tempVec3.y >> tempVec3.z;
		position.push_back(tempVec3);
	}


	in >> tempVec2.x >> tempVec2.y;
	uv.push_back(tempVec2);
	while (true)
	{
		in >> ignore;
		if (ignore != "vt")
			break;
		in >> tempVec2.x >> tempVec2.y;
		uv.push_back(tempVec2);
	}

	in >> tempVec3.x >> tempVec3.y >> tempVec3.z;
	normal.push_back(tempVec3);
	while (true)
	{
		in >> ignore;
		if (ignore != "vn")
			break;
		in >> tempVec3.x >> tempVec3.y >> tempVec3.z;
		normal.push_back(tempVec3);
	}
	in >> ignore;





	int positionIndex;
	int uvIndex;
	int normalIndex;



	while (true)
	{
		in >> ignore;
		if (ignore == "f")
			break;
	}



	string s;
	vector<string> tokens;

	in >> s;
	ObjReadFace(tokens, s, position, normal, uv);
	in >> s;
	ObjReadFace(tokens, s, position, normal, uv);
	in >> s;
	ObjReadFace(tokens, s, position, normal, uv);

	while (true)
	{


		in >> ignore;

		if (in.fail())
			break;

		in >> s;
		ObjReadFace(tokens, s, position, normal, uv);

	//	in >> ignore;
		in >> s;
		ObjReadFace(tokens, s, position, normal, uv);

	//	in >> ignore;
		in >> s;
		ObjReadFace(tokens, s, position, normal, uv);
	}

	size = vertices.size();

	// VAO 를지정하고할당하기 
	glGenVertexArrays(1, &vao);
	// VAO를바인드하기 
	glBindVertexArray(vao);

	// 2개의 VBO를지정하고할당하기 
	glGenBuffers(2, vbo);
	//--- 1번째 VBO를활성화하여바인드하고, 버텍스속성 (좌표값)을저장 
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

	// 변수 diamond 에서버텍스데이터값을버퍼에복사한다.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * (size), &vertices[0], GL_STATIC_DRAW);
	// 좌표값을 attribute 인덱스 0번에명시한다: 버텍스당 3* float 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (GLvoid*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (GLvoid*)(5 * sizeof(float)));
	// attribute 인덱스 0번을사용가능하게함 
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	meshType = MeshType::OBJ_MESH;

	return true;
}

void Mesh::Delete()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(2, vbo);
}

void Mesh::ObjReadFace(vector<string>& tokens, string& s, vector<glm::vec3>& position, vector<glm::vec3>& normal,vector<glm::vec2>& uv)
{
	tokens.clear();
	stringTokenize(tokens, s, '/');
	int positionIndex = stoi(tokens[0]);
	int uvIndex = stoi(tokens[1]);
	int normalIndex = stoi(tokens[2]);	vertices.push_back({ position[positionIndex - 1],uv[uvIndex - 1],normal[normalIndex-1] });
}
