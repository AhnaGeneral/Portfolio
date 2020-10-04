#include "stdafx.h"
#include "MeshObject.h"
#include"Renderer.h"
#include"Mesh.h"

MeshObject::MeshObject()
{
}


MeshObject::~MeshObject()
{
}

bool MeshObject::Initialize(const BasicObjectDesc& desc, Renderer* renderer, Mesh* mesh)
{
	primitiveType = desc.primitiveType;

	this->renderer = renderer;
	this->mesh = mesh;
	return true;
}

bool MeshObject::Initialize(const BasicObjectDesc & desc, Renderer * renderer, Mesh * mesh, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	Initialize(desc, renderer, mesh);
	this->position = glm::translate(this->position, position);
	this->rotation = this->rotation * glm::rotate(this->rotation, glm::radians(rotation.x), { 1,0,0 });
	this->rotation = this->rotation * glm::rotate(this->rotation, glm::radians(rotation.y), { 0,1,0 });
	this->rotation = this->rotation * glm::rotate(this->rotation, glm::radians(rotation.z), { 0,0,1 });
	this->scale = glm::scale(this->scale, scale);
	return true;
}

bool MeshObject::Initialize(const BasicObjectDesc & desc, Renderer * renderer, Mesh* mesh , glm::vec3 position, glm::vec3 rotation, glm::vec3 scale,glm::vec3 movementSpeed)
{
	Initialize(desc, renderer, mesh, position, rotation, scale);
	this ->movementSpeed = glm::vec3(movementSpeed.x, movementSpeed.y, movementSpeed.z);

	if (mesh)
		return false;

	return true;
}

void MeshObject::Render()
{
	if(mesh->meshType == MeshType::BASIC_MESH)
		renderer->Draw(worldMatrix, primitiveType, mesh->GetVAO(), mesh->size);
	if (mesh->meshType == MeshType::OBJ_MESH)
		renderer->Draw(worldMatrix, primitiveType, mesh->GetVAO(), mesh->size, color);
}

void MeshObject::Update( const float elapsedTime)
{

	Translate(movementSpeed * (float)elapsedTime);
	Roll(rotationSpeed.x* elapsedTime);
	Pitch(rotationSpeed.y * elapsedTime);
	Yaw(rotationSpeed.z * elapsedTime);

	worldMatrix = GetFinalMatrix();

}


