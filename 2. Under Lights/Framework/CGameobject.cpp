#include "CGameobject.h"

GLfloat m_CubeShape[8][3] =
{ -0.3f, -0.3f, -0.3f,  -0.3f, -0.3f, +0.3f, -0.3f, +0.3f, -0.3f,
-0.3f, +0.3f, +0.3f, +0.3f, -0.3f, -0.3f,  +0.3f, -0.3f, +0.3f,
+0.3f, +0.3f, -0.3f,  +0.3f, +0.3f, +0.3f };

GLfloat m_CubeColor[8][3] =
{ { 0.0f, 0.0f, 1.0f},  {0.5f, 0.0f, 0.0f}, {0.5f, 0.5f, 0.0f},
  {0.5f, 0.0f, 0.5f}, {0.8f, 0.2f, 0.8f},  {0.5f, 0.5f, 1.0f},
  {0.5f, 0.5f, 1.0f},  {0.0f, 0.0f, 1.0f} };

GLfloat m_Cubenomal[8][3] =
{ {0.0f, 1.0f, 0.0f},{0.0f, 1.0f, 0.0f},
{0.0f, 1.0f, 0.0f},  {0.0f, 1.0f, 0.0f},{0.0f, 1.0f, 0.0f},{0.0f, 1.0f, 0.0f},
{0.0f, 1.0f, 0.0f},  {0.0f, 1.0f, 0.0f} };

GLfloat Downstage[4][3] =
{ { -0.3f, -0.3f, -0.3f},{ -0.3f, -0.3f, +0.3f},
{+0.3f, -0.3f, -0.3f},  {+0.3f, -0.3f, +0.3f} };

GLfloat Backstage[4][3] =
{ -0.3f, -0.3f, -0.3f ,-0.3f, +0.3f, -0.3f ,
 +0.3f, +0.3f, -0.3f, +0.3f, -0.3f, -0.3f };

GLfloat Frontstage[4][3] =
{ { -0.3f, -0.3f, +0.3f},{ -0.3f, +0.3f, +0.3f},
{+0.3f, -0.3f, +0.3f},  {+0.3f, +0.3f, +0.3f } };

GLfloat Downcolor[4][3] =
{ { 0.0f, 0.0f, 1.0f},  {0.5f, 0.0f, 0.0f}, 
{0.5f, 0.5f, 0.0f},  {0.5f, 0.0f, 0.5f} };

GLfloat Backcolor[4][3] =
{ {0.8f, 0.2f, 0.8f},  {0.5f, 0.5f, 1.0f}, 
{0.5f, 0.5f, 1.0f},  {0.0f, 0.0f, 1.0f} };

GLfloat Frontcolor[4][3] =
{ {0.8f, 0.1f, 0.2f},  {0.1f, 0.5f, 0.9f},
{0.5f, 0.5f, 1.0f},  {0.1f, 0.7f, 0.2f} };

GLfloat sidecolor[4][3] =
{ {0.5f, 0.5f, 0.0f}, {0.5f, 0.0f, 0.5f}, 
{0.8f, 0.2f, 0.8f},  {0.5f, 0.5f, 1.0f} };

GLfloat sideNomal[4][3] =
{ {0.0f, 1.0f, 0.0f},{0.0f, 1.0f, 0.0f},
{0.0f, 1.0f, 0.0f},  {0.0f, 1.0f, 0.0f} };

unsigned int m_stageIndex[] = {0, 2, 3, 1, 0, 3}; 
unsigned int m_BackIndex[] = { 1, 2, 3, 0,1 , 3 };
unsigned int m_FrontIndex[] = { 0, 2, 1, 2,3,1 };
unsigned int m_sideIndex[] = { 0,3,2, 0,1,3, 5,6,7,5,4,6 };
unsigned int m_CubeIndex[] =
{ 2,6,4, 0,2,4, 5,6,7,5,4,6,2,7,6,2,3,7,3,2,1,2,0,1,5,7,3,5,3,1,0,4,5,0,5,1 };

CGameobject::CGameobject()
{
}
CGameobject::~CGameobject()
{
}

void CGameobject::Render()
{
	
}

CCube::CCube()
{
}

CCube::~CCube()
{
}
void CCube::InitBuffer()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(3, VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_CubeShape), m_CubeShape, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_CubeIndex), m_CubeIndex, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_CubeColor), m_CubeColor, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_Cubenomal), m_Cubenomal, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	//glDeleteBuffers(2, VBO);
	//glDeleteBuffers(1, &EBO);


}

void CCube::Update()
{
}
void CCube::CreatObject()
{

}
void CCube::Render()
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(VAO);
	InitBuffer();
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


}

void CCube::ChangeColor(float C1, float C2, float C3)
{

	for (int i = 0; i < 8; ++i)
	{
		m_CubeColor[i][0] = C1;
		m_CubeColor[i][1] = C2;
		m_CubeColor[i][2] = C3;
	}
}

void CCube::Delete()
{
	glDeleteBuffers(2, VBO);
    glDeleteBuffers(1, &EBO);
}

void CStage::InitBuffer1()
{
	glGenVertexArrays(1, &VAO1);
	glGenBuffers(3, VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Downstage), Downstage, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_stageIndex), m_stageIndex, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Downcolor), Downcolor, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sideNomal), sideNomal, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	//glDeleteBuffers(2, VBO); 
	//glDeleteBuffers(1, &EBO); 

}

void CStage::InitBuffer2()
{
	glGenVertexArrays(1, &VAO2);
	glGenBuffers(3, VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_CubeShape), m_CubeShape, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_sideIndex), m_sideIndex, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_CubeColor), m_CubeColor, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sideNomal), sideNomal, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);



}

void CStage::InitBuffer3()
{
	glGenVertexArrays(1, &VAO3);
	glGenBuffers(3, VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO3);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Backstage),Backstage, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_BackIndex), m_BackIndex, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_CubeColor), m_CubeColor, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sideNomal), sideNomal, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

}

void CStage::CreatObject()
{
}

void CStage::Render()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(VAO1);
	InitBuffer1();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(VAO2);
	InitBuffer2();
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

	//glBindVertexArray(VAO3);
	//InitBuffer3();
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

void CStage::SetColor(float Color)
{
	for (int i = 0; i < 1; ++i)
	{
		m_CubeColor[0][i] = Color;
		m_CubeColor[1][i] = Color;
		m_CubeColor[2][i] = Color;
		m_CubeColor[3][i] = Color;
		m_CubeColor[4][i] = Color;
		m_CubeColor[5][i] = Color;
		m_CubeColor[6][i] = Color;
		m_CubeColor[7][i] = Color;

		m_CubeColor[0][i+1] = 0.0f;
		m_CubeColor[1][i+1] = 0.0f;
		m_CubeColor[2][i+1] = 0.0f;
		m_CubeColor[3][i+1] = 0.0f;
		m_CubeColor[4][i+1] = 0.0f;
		m_CubeColor[5][i+1] = 0.0f;
		m_CubeColor[6][i+1] = 0.0f;
		m_CubeColor[7][i+1] = 0.0f;

		m_CubeColor[0][i + 2] = 0.2f;
		m_CubeColor[1][i + 2] = 0.2f;
		m_CubeColor[2][i + 2] = 0.2f;
		m_CubeColor[3][i + 2] = 0.2f;
		m_CubeColor[4][i + 2] = 0.2f;
		m_CubeColor[5][i + 2] = 0.2f;
		m_CubeColor[6][i + 2] = 0.2f;
		m_CubeColor[7][i + 2] = 0.2f;
	}
}

void CStage::Delete()
{
	glDeleteBuffers(2, VBO);
	glDeleteBuffers(1, &EBO);
}

CFrontView::CFrontView()
{
}

CFrontView::~CFrontView()
{
}

void CFrontView::InitBuffer()
{
	glGenVertexArrays(1, &VAO);
	GLuint  VBO[2], EBO, Back[2];
	glGenBuffers(2, VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Frontstage), Frontstage, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_FrontIndex), m_FrontIndex, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Frontcolor), Frontcolor, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	//glDeleteBuffers(2, VBO); 
	//glDeleteBuffers(1, &EBO); 
}

void CFrontView::Render()
{
	glBindVertexArray(VAO);
	InitBuffer();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
