#pragma once
class Light
{
public:
	Light();
	~Light();


public:
	glm::vec3 sunLight = {0,0,0};
	float ambientLight = 0.2f;

	bool sunLightOff = false;
};

