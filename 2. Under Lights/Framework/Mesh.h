#pragma once
class Renderer;



enum class MeshType
{
	BASIC_MESH,
	OBJ_MESH
};

class Mesh
{
public:
	Mesh() = default;
	~Mesh() = default;
	void CreateCube();
	void CreatePyramid();
	void CreateTriangle();
	void CreateRectangle();
	void CreateMeshByVertices(const vector<glm::vec3>& vertex, const vector<glm::vec3>& colors);
	bool CreateMeshByObj(const char* path);


	GLuint GetVAO() const { return vao; };

	void Delete();

private:
	void ObjReadFace(vector<string>& tokens, string& s, vector<glm::vec3>& position, vector<glm::vec3>& normal, vector<glm::vec2>& uv);
public:
	int size = 0;
	vector<VertexData> vertices;
	MeshType meshType = MeshType::BASIC_MESH;

	
private:
	GLuint  vao = UINT_MAX;
	GLuint  vbo[2] = {0,0};

};

