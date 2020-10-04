#pragma once
class Renderer;
class Mesh;

class MeshObject
{
public:
	MeshObject();
	~MeshObject();
	//어떤 메쉬를 쓸건지. Primitive Type, 시작 위치, 회전, 크기 값들을 지정.
	//초기화 함수. 반드시 불러야 함.
	bool Initialize(const BasicObjectDesc& desc, Renderer* renderer, Mesh* mesh);
	bool Initialize(const BasicObjectDesc& desc, Renderer* renderer, Mesh* mesh, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
	bool Initialize(const BasicObjectDesc& desc, Renderer* renderer, Mesh* mesh, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 movementSpeed);
	
	//Object를 그리는 함수. 자신의 위치정보와 쉐이더에 넘겨줘야 하는 정보들을 Renderer에 넘겨준다. Renderer의 그리기 함수 호출.
	virtual void Render();

	//Object의 위치, 회전, 크기 등 상태를 갱신.
	virtual void Update(const float elapsedTime);

	//공전하는 순서로 월드 행렬 계산.
	glm::mat4 GetRevolveFinalMatrix() { return rotation * position * scale; };
	//자전하는 순서로 월드 행렬 계산.
	glm::mat4 GetFinalMatrix() { return position * rotation * scale; }

	//RotationDelta
	void Roll(float angle) { rotation = glm::rotate(rotation, glm::radians(angle), { 1,0,0 }); }//X축 회전
	void Pitch(float angle) { rotation = glm::rotate(rotation, glm::radians(angle), { 0,1,0 }); }//Y축 회전
	void Yaw(float angle) { rotation = glm::rotate(rotation, glm::radians(angle), { 0,0,1 }); }//Z축 회전
	void RollRadian(float angle) { rotation = glm::rotate(rotation, angle, { 1,0,0 }); }//X축 회전
	void PitchRadian(float angle) { rotation = glm::rotate(rotation, angle, { 0,1,0 }); }//Y축 회전
	void YawRadian(float angle) { rotation = glm::rotate(rotation, angle, { 0,0,1 }); }//Z축 회전

	//SetRotation
	void SetRotation(const glm::vec3& angle) { 
	rotation = glm::mat4(1.0f); 
	rotation = glm::rotate(rotation, glm::radians(angle.x), {1,0,0});
	rotation = glm::rotate(rotation, glm::radians(angle.y), { 0,1,0 });
	rotation = glm::rotate(rotation, glm::radians(angle.z), { 0,0,1 });
	}
	void SetRotationRadian(const glm::vec3& angle) {
		rotation = glm::mat4(1.0f);
		rotation = glm::rotate(rotation, angle.x, { 1,0,0 });
		rotation = glm::rotate(rotation,angle.y, { 0,1,0 });
		rotation = glm::rotate(rotation, angle.z, { 0,0,1 });
	}

	void Translate(glm::vec3 amount) { position = glm::translate(position, amount); }//이동 함수.
	void SetRotationSpeed(const glm::vec3 rotationSpeed) { this->rotationSpeed = rotationSpeed; }
	void SetMovementSpeed(const glm::vec3& movementSpeed) { this->movementSpeed = movementSpeed; }
	void ResetRotation() { rotation = glm::mat4(1.0f); }

	glm::vec3 GetPosition() const { return { position[3][0], position[3][1], position[3][2] }; }

public:
	Mesh* mesh = nullptr;
	glm::mat4 worldMatrix = glm::mat4(1.0f);
	bool isActive = true;
	glm::vec4 color = {0.8, 0, 0.4, 1};

protected:
	GLuint primitiveType = GLU_LINE;//어떤 프리미티브로 그릴건지

	glm::vec3 rotationSpeed = glm::vec3(0,0,0);
	glm::vec3 movementSpeed = glm::vec3(0, 0, 0);

	glm::mat4 position = glm::mat4(1.0f);
	glm::mat4 rotation = glm::mat4(1.0f);
	glm::mat4 scale = glm::mat4(1.0f);

	Renderer* renderer = nullptr;

};

