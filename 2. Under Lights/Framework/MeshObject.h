#pragma once
class Renderer;
class Mesh;

class MeshObject
{
public:
	MeshObject();
	~MeshObject();
	//� �޽��� ������. Primitive Type, ���� ��ġ, ȸ��, ũ�� ������ ����.
	//�ʱ�ȭ �Լ�. �ݵ�� �ҷ��� ��.
	bool Initialize(const BasicObjectDesc& desc, Renderer* renderer, Mesh* mesh);
	bool Initialize(const BasicObjectDesc& desc, Renderer* renderer, Mesh* mesh, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
	bool Initialize(const BasicObjectDesc& desc, Renderer* renderer, Mesh* mesh, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 movementSpeed);
	
	//Object�� �׸��� �Լ�. �ڽ��� ��ġ������ ���̴��� �Ѱ���� �ϴ� �������� Renderer�� �Ѱ��ش�. Renderer�� �׸��� �Լ� ȣ��.
	virtual void Render();

	//Object�� ��ġ, ȸ��, ũ�� �� ���¸� ����.
	virtual void Update(const float elapsedTime);

	//�����ϴ� ������ ���� ��� ���.
	glm::mat4 GetRevolveFinalMatrix() { return rotation * position * scale; };
	//�����ϴ� ������ ���� ��� ���.
	glm::mat4 GetFinalMatrix() { return position * rotation * scale; }

	//RotationDelta
	void Roll(float angle) { rotation = glm::rotate(rotation, glm::radians(angle), { 1,0,0 }); }//X�� ȸ��
	void Pitch(float angle) { rotation = glm::rotate(rotation, glm::radians(angle), { 0,1,0 }); }//Y�� ȸ��
	void Yaw(float angle) { rotation = glm::rotate(rotation, glm::radians(angle), { 0,0,1 }); }//Z�� ȸ��
	void RollRadian(float angle) { rotation = glm::rotate(rotation, angle, { 1,0,0 }); }//X�� ȸ��
	void PitchRadian(float angle) { rotation = glm::rotate(rotation, angle, { 0,1,0 }); }//Y�� ȸ��
	void YawRadian(float angle) { rotation = glm::rotate(rotation, angle, { 0,0,1 }); }//Z�� ȸ��

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

	void Translate(glm::vec3 amount) { position = glm::translate(position, amount); }//�̵� �Լ�.
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
	GLuint primitiveType = GLU_LINE;//� ������Ƽ��� �׸�����

	glm::vec3 rotationSpeed = glm::vec3(0,0,0);
	glm::vec3 movementSpeed = glm::vec3(0, 0, 0);

	glm::mat4 position = glm::mat4(1.0f);
	glm::mat4 rotation = glm::mat4(1.0f);
	glm::mat4 scale = glm::mat4(1.0f);

	Renderer* renderer = nullptr;

};
