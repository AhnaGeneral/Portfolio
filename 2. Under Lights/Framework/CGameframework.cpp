#include "stdafx.h"
#include "readshader.h"
#include "CGameframework.h"
#include "CGameobject.h"

#include"Renderer.h"
#include"MeshObject.h"
#include"Mesh.h"

bool Initialize();
unordered_map<string, Mesh*> meshMap;
vector<MeshObject*> objectList;
vector<MeshObject*> bgList;
Renderer* renderer;
auto prevTime = chrono::high_resolution_clock::now();
GLdouble wx, wy, wz;	//픽킹좌표

//=========================================
GLuint ProgramID;
bool startAnimalMove = true;
bool On_BackView = true, On_CameraView = false, Frist_Person =false;
bool Start_Follow_Camera1 = false, Start_Follow_Camera2 = false, Start_Follow_Camera3 = false, Start_Follow_Camera4 = false;
float f_camerapos[3]{ 0.0f, 4.0f, 20.0f }; 
glm::vec3 BackView3 = glm::vec3(0.0f, 2.0f, 8.0f);
glm::mat4 destcamera; 
glm::vec3 cameraDirection= glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraDirectioncenter = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 viewDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec4 fildCameraPosition;
float lightposz = 0.0f;

void Follow_Camera_Robot(glm::vec3 TargetcameraPos, glm::vec3 Robot, int numberrobot);

CCube* m_Robot,*m_camera, *m_Animal,*m_Villain, *m_bullet;// 얼굴, 팔 2개, 다리 2개 , 몸통, 코 = 총 7개 
CStage* m_Stage;
CFrontView* mFrontStage;
int currentObjectIndex = 0;

//======================================================================================
typedef struct POSITION
{
	float x; 
	float y; 
	float z;
}S_Pos;

S_Pos s_pos01{ 0,0,0 }, s_pos02{0,0,0};
POSITION s_posColor{ 1.0, 1.0, 1.0 };

typedef struct MOVEROBOT
{
	bool b_Right; 
	bool b_Left; 
	bool b_Back; 
	bool b_Front;
}s_moveRobot;

typedef struct INFO_BODYSTRUCTURE
{
	float g_rLeg; 
	float g_lLeg; 
	float g_rArm; 
	float g_lArm; 
}INFO_BODYSTRUCTURE;

INFO_BODYSTRUCTURE info_people, info_animal;

s_moveRobot s_robot01{ false, false, false, false };
float angle[4]{};
float PeopleTick = 0.8f  , AnimalTick = 0.8f, rotateangle = 0.0f;
int num01 = 0, num02 = 1, Animalnum03 = 2, num04 = 3;
float lipOpen = -0.8, villainposz=0.0f, bullet_velocity = s_pos02.z + 5; 
bool Bang = false; 
int countbot = 0; 
bool Summons01 = false, Summons02 = false;

//========================Animal==============================================================

CGameFrameWork::CGameFrameWork() {}
CGameFrameWork::~CGameFrameWork() {}

void CGameFrameWork::FrameAdvance(int argc, char** argv)
{
	glutInit(&argc, argv); //glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); //디스플레이 모드 설정
	glutInitWindowPosition(100, 100); //윈도우의 위치 지정
	glutInitWindowSize(WINDOW_SIZE_WIDTH, WINDOW_SIZE_HEIGHT); //윈도우의 크기 지정
	glutCreateWindow("using index buffer");//윈도우 생성
	


//[ GLEW 초기화 하기 ]====================================================================
	glewExperimental = GL_TRUE; //glew 초기화
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else std::cout << "GLEW Initialized\n";
	ProgramID = (Complie_Shaders());

	if (!Initialize())
	{
		cout << "Initialize fail\n";
		return;
	}

	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUP);
	glutTimerFunc(1000, Timerfunction, 1);
	glutDisplayFunc(RenderScene); // 출력 함수의 지정 
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMainLoop();
}

void RenderScene()
{
	m_Villain = new CCube; 
	m_camera = new CCube; 
	m_Robot = new CCube;
	m_Stage = new CStage;
	mFrontStage = new CFrontView;
	m_Animal = new CCube;
	m_bullet = new CCube; 

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glUseProgram(ProgramID);

	glm::vec3 cameraPos = glm::vec3(f_camerapos[0], f_camerapos[1], f_camerapos[2]);
	glm::vec4 vec3_cameraPos;
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));

	int modelLoc = glGetUniformLocation(ProgramID, "model");
	int viewLoc = glGetUniformLocation(ProgramID, "view");
	int projLoc = glGetUniformLocation(ProgramID, "projection");


	int camerapos = glGetUniformLocation(ProgramID, "viewPos");
	glUniform3f(camerapos, cameraPos.r, cameraPos.g, cameraPos.b);
	//cout << "camerapos"<< camerapos <<endl;

	int lightPosLocation = glGetUniformLocation(ProgramID, "lightPos");
	glm::vec4 InitLightPos = glm::vec4(0.0f, 5.0f, lightposz, 1.0f);

	glm::mat4 destLight = glm::rotate(glm::mat4(1.0f), rotateangle, glm::vec3(0.0f, 1.0f, 0.0f));
	InitLightPos = destLight * InitLightPos;
	glUniform3f(lightPosLocation, InitLightPos.r, InitLightPos.g, InitLightPos.b);

	int lightColorLocation = glGetUniformLocation(ProgramID, "lightColor");
	glUniform3f(lightColorLocation, s_posColor.x, s_posColor.y, s_posColor.z);
	//cout << "lightColorLocation" << lightColorLocation << endl;

	int objColorLocation = glGetUniformLocation(ProgramID, "objectColor");
	glUniform3f(objColorLocation, 1.0, 1.0, 1.0);

	if (s_pos01.x >= 7.0f) { num01 = 2; }
	if (s_pos01.x <= -7.0f) { num01 = 3; }
	if (s_pos01.z >= 7.0f) { num01 = 0; }
	if (s_pos01.z <= -7.0f) { num01 = 1; }

	if (num01 == 0)
	{
		s_robot01.b_Back = true;
		s_robot01.b_Front = false;
		s_robot01.b_Left = false;
		s_robot01.b_Right = false;
	}
	//======================================================================================
	glm::mat4 view = glm::mat4(1.0f);
	if(On_BackView)
	view = glm::lookAt(BackView3, viewDirection, cameraUp);
	if (Frist_Person)
	{
		if (s_robot01.b_Front == true)
			view = glm::lookAt(glm::vec3(s_pos02.x, s_pos02.y + 1.5f, s_pos02.z+7.0f ), cameraDirection, cameraUp);
		if (s_robot01.b_Back == true)
			view = glm::lookAt(glm::vec3(s_pos02.x, s_pos02.y + 1.0f, s_pos02.z+7.0f ), cameraDirection, cameraUp);
	}	
	if (On_CameraView)
	{
		view = glm::lookAt(cameraPos, cameraDirectioncenter, cameraUp);
	}
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(60.0f), (float)WINDOW_SIZE_WIDTH / (float)WINDOW_SIZE_HEIGHT, 0.1f, 200.0f);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
	//======================================================================================


	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//	//[몸통]===============================================================================
	//	glm::mat4 CenterBody = glm::mat4(1.0f); // 단위행렬로초기화 
	//	float directionbody = 0.0f ;		//픽킹한 위치로 이동
	//	if (objectList.size() > 0)
	//	{
	//		glm::vec3 pos = { s_pos01.x,s_pos01.y,s_pos01.z };
	//		glm::vec3 dir = glm::normalize(objectList[objectList.size() - 1]->GetPosition() - pos); //방향벡터

	//		if (abs(objectList[objectList.size() - 1]->GetPosition().x - s_pos01.x) > DBL_EPSILON)
	//		{
	//			s_pos01.x += dir.x * 0.3f;
	//		}
	//		if (abs(objectList[objectList.size() - 1]->GetPosition().z - s_pos01.z) > DBL_EPSILON)
	//		{
	//			s_pos01.z += dir.z * 0.3f;
	//		}




	glm::mat4 AnimalCenterBody = glm::mat4(1.0f); // 단위행렬로초기화 
	// 동물 애니메이션
	{
		//[몸통]===============================================================================
		//픽킹한 위치로 이동
		if (startAnimalMove)
		{	 
			if (countbot == 5)
			{
				s_pos02.x = 0.0f; 
				s_pos02.y = 0.0f; 
				s_pos02.z = 0.0f; 
				lightposz = 0.0f;
				++countbot; 
				cout << "END" << endl;
				CreateFood();
			}
			else
				AnimalCenterBody = glm::translate(AnimalCenterBody, glm::vec3(s_pos02.x, s_pos02.y, s_pos02.z));
				AnimalCenterBody = glm::rotate(AnimalCenterBody, angle[0], glm::vec3(0.0, 1.0, 0.0));
				AnimalCenterBody = glm::scale(AnimalCenterBody, glm::vec3(1.0f, 1.0f, 2.0f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(AnimalCenterBody));
				m_Animal->ChangeColor(0.5f, 0.2f, 0.0f);//몸통
				m_Animal->Render();
				m_Animal->Delete();		

				//[얼굴,코]============================================================
				glm::mat4 Animal_Head = glm::mat4(1.0f);
				Animal_Head = glm::translate(AnimalCenterBody, glm::vec3(0.0, 0.7, +0.25));
				Animal_Head = glm::scale(Animal_Head, glm::vec3(0.5, 0.5, 0.5));
				Animal_Head = glm::scale(Animal_Head, glm::vec3(1.0f, 2.0f, 0.6f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Animal_Head));
				m_Animal->ChangeColor(0.2f, 0.2f, 0.2f);//
				m_Animal->Render();
				m_Animal->Delete();
				
				glm::mat4 Animal_Neck = glm::mat4(1.0f);
				Animal_Neck = glm::translate(AnimalCenterBody, glm::vec3(0.0, 0.1, +0.25));
				Animal_Neck = glm::scale(Animal_Neck, glm::vec3(0.2, 0.7, 0.2));
				Animal_Neck = glm::scale(Animal_Neck, glm::vec3(1.0f, 2.0f, 0.6f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Animal_Neck));
				m_Animal->ChangeColor(0.2f, 0.2f, 0.2f);//
				m_Animal->Render();
				m_Animal->Delete();

				glm::mat4 Animal_Tail = glm::mat4(1.0f);
				Animal_Tail = glm::translate(AnimalCenterBody, glm::vec3(0.0, 0.1, -0.35));
				Animal_Tail = glm::rotate(Animal_Tail, rotateangle, glm::vec3(0.5, 1.0, 0.0));
				Animal_Tail = glm::scale(Animal_Tail, glm::vec3(0.1, 0.1, 0.35));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Animal_Tail));
				m_Animal->ChangeColor(0.2f, 0.2f, 0.2f);//
				m_Animal->Render();
				m_Animal->Delete();

				Animal_Head = glm::translate(Animal_Head, glm::vec3(0.0, 0.0, 0.4));
				Animal_Head = glm::scale(Animal_Head, glm::vec3(0.2, 0.2, 0.3));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Animal_Head));
				m_Animal->ChangeColor(0.1f, 0.1f, 0.1f);//
				m_Animal->Render();
				m_Animal->Delete();

				glm::mat4 Animal_EARS = glm::mat4(1.0f);
				Animal_EARS = glm::translate(AnimalCenterBody, glm::vec3(+0.2, 0.9, +0.25));
				Animal_EARS = glm::scale(Animal_EARS, glm::vec3(0.1, 0.4, 0.18));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Animal_EARS));
				m_Animal->ChangeColor(0.5f, 0.2f, 0.0f);//
				m_Animal->Render();
				m_Animal->Delete();

				Animal_EARS = glm::translate(AnimalCenterBody, glm::vec3(-0.2, 0.9, +0.25));
				Animal_EARS = glm::scale(Animal_EARS, glm::vec3(0.1, 0.4, 0.18));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Animal_EARS));
				m_Animal->ChangeColor(0.5f, 0.2f, 0.0f);//
				m_Animal->Render();
				m_Animal->Delete();

				//[오른팔]===========================================================
				glm::mat4 AnimalRightArm = glm::mat4(1.0f);
				AnimalRightArm = glm::translate(AnimalCenterBody, glm::vec3(+0.3f, -0.20f, +0.18f));
				AnimalRightArm = glm::rotate(AnimalRightArm, info_animal.g_rArm, glm::vec3(1.0f, 0.0f, 0.0f));
				AnimalRightArm = glm::scale(AnimalRightArm, glm::vec3(0.2, 0.6, 0.2));
				AnimalRightArm = glm::translate(AnimalRightArm, glm::vec3(0.0f, -0.18f, 0.0f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(AnimalRightArm));
				m_Animal->ChangeColor(0.2f, 0.2f, 0.2f);//
				m_Animal->Render();
				m_Animal->Delete();
				//[왼팔]============================================================
				glm::mat4 AnimalLeftArm = glm::mat4(1.0f);
				AnimalLeftArm = glm::translate(AnimalCenterBody, glm::vec3(-0.3f, -0.20f, +0.18f));
				AnimalLeftArm = glm::rotate(AnimalLeftArm, info_animal.g_lArm, glm::vec3(1.0f, 0.0f, 0.0f));
				AnimalLeftArm = glm::scale(AnimalLeftArm, glm::vec3(0.2, 0.6, 0.2));
				AnimalLeftArm = glm::translate(AnimalLeftArm, glm::vec3(0.0f, -0.18f, 0.0f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(AnimalLeftArm));
				m_Animal->ChangeColor(0.2f, 0.2f, 0.2f);//
				m_Animal->Render();
				m_Animal->Delete();
				//[오른다리]========================================================
				glm::mat4 AnimalRightLeg = glm::mat4(1.0f);
				AnimalRightLeg = glm::translate(AnimalCenterBody, glm::vec3(0.3f, -0.20f, -0.18f));
				AnimalRightLeg = glm::rotate(AnimalRightLeg, info_animal.g_rLeg, glm::vec3(1.0f, 0.0f, 0.0f));
				AnimalRightLeg = glm::scale(AnimalRightLeg, glm::vec3(0.3, 0.6, 0.18));
				AnimalRightLeg = glm::translate(AnimalRightLeg, glm::vec3(0.0f, -0.15f, 0.0f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(AnimalRightLeg));
				m_Animal->ChangeColor(0.2f, 0.2f, 0.2f);//
				m_Animal->Render();
				m_Animal->Delete();

				//[왼다리]==========================================================
				glm::mat4 AnimalLeftLeg = glm::mat4(1.0f);
				AnimalLeftLeg = glm::translate(AnimalCenterBody, glm::vec3(-0.3f, -0.20f, -0.18f));
				AnimalLeftLeg = glm::rotate(AnimalLeftLeg, info_animal.g_lLeg, glm::vec3(1.0f, 0.0f, 0.0f));
				AnimalLeftLeg = glm::scale(AnimalLeftLeg, glm::vec3(0.3, 0.6, 0.18));
				AnimalLeftLeg = glm::translate(AnimalLeftLeg, glm::vec3(0.0f, -0.15f, 0.0f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(AnimalLeftLeg));
				m_Animal->ChangeColor(0.15f, 0.2f, 0.3f);//
				m_Animal->Render();
				m_Animal->Delete();

				//}
			}
		}


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//동물 소환하기
	if (Summons01)
	{
		AnimalCenterBody = glm::mat4(1.0f); // 단위행렬로초기화 
		{

			if (startAnimalMove)
			{
				if (countbot == 5)
				{
					s_pos02.x = s_pos02.x + 0.3f;
					s_pos02.y = s_pos02.y;
					s_pos02.z = s_pos02.z - 0.2f;
				}
				else
					AnimalCenterBody = glm::translate(AnimalCenterBody, glm::vec3(s_pos02.x + 0.7f, s_pos02.y, s_pos02.z + 3.4f));
				AnimalCenterBody = glm::rotate(AnimalCenterBody, angle[0], glm::vec3(0.0, 1.0, 0.0));
				AnimalCenterBody = glm::scale(AnimalCenterBody, glm::vec3(0.6f, 0.6f, 1.0f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(AnimalCenterBody));
				m_Animal->ChangeColor(1.0f, 1.0f, 1.0f);//몸통
				m_Animal->Render();
				m_Animal->Delete();


				//[얼굴,코]============================================================
				glm::mat4 Animal_Head = glm::mat4(1.0f);
				Animal_Head = glm::translate(AnimalCenterBody, glm::vec3(0.0, 0.7, +0.25));
				Animal_Head = glm::scale(Animal_Head, glm::vec3(0.5, 0.5, 0.5));
				Animal_Head = glm::scale(Animal_Head, glm::vec3(1.0f, 2.0f, 0.6f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Animal_Head));
				m_Animal->ChangeColor(0.2f, 0.2f, 0.2f);//
				m_Animal->Render();
				m_Animal->Delete();

				glm::mat4 Animal_Neck = glm::mat4(1.0f);
				Animal_Neck = glm::translate(AnimalCenterBody, glm::vec3(0.0, 0.1, +0.25));
				Animal_Neck = glm::scale(Animal_Neck, glm::vec3(0.2, 0.7, 0.2));
				Animal_Neck = glm::scale(Animal_Neck, glm::vec3(1.0f, 2.0f, 0.6f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Animal_Neck));
				m_Animal->ChangeColor(0.2f, 0.2f, 0.2f);//
				m_Animal->Render();
				m_Animal->Delete();

				glm::mat4 Animal_Tail = glm::mat4(1.0f);
				Animal_Tail = glm::translate(AnimalCenterBody, glm::vec3(0.0, 0.1, -0.35));
				Animal_Tail = glm::rotate(Animal_Tail, rotateangle, glm::vec3(0.5, 1.0, 0.0));
				Animal_Tail = glm::scale(Animal_Tail, glm::vec3(0.1, 0.1, 0.35));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Animal_Tail));
				m_Animal->ChangeColor(0.2f, 0.2f, 0.2f);//
				m_Animal->Render();
				m_Animal->Delete();

				Animal_Head = glm::translate(Animal_Head, glm::vec3(0.0, 0.0, 0.4));
				Animal_Head = glm::scale(Animal_Head, glm::vec3(0.2, 0.2, 0.3));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Animal_Head));
				m_Animal->ChangeColor(0.1f, 0.1f, 0.1f);//
				m_Animal->Render();
				m_Animal->Delete();

				glm::mat4 Animal_EARS = glm::mat4(1.0f);
				Animal_EARS = glm::translate(AnimalCenterBody, glm::vec3(+0.2, 0.9, +0.25));
				Animal_EARS = glm::scale(Animal_EARS, glm::vec3(0.1, 0.4, 0.18));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Animal_EARS));
				m_Animal->ChangeColor(0.5f, 0.2f, 0.0f);//
				m_Animal->Render();
				m_Animal->Delete();

				Animal_EARS = glm::translate(AnimalCenterBody, glm::vec3(-0.2, 0.9, +0.25));
				Animal_EARS = glm::scale(Animal_EARS, glm::vec3(0.1, 0.4, 0.18));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Animal_EARS));
				m_Animal->ChangeColor(0.5f, 0.2f, 0.0f);//
				m_Animal->Render();
				m_Animal->Delete();

				//[오른팔]===========================================================
				glm::mat4 AnimalRightArm = glm::mat4(1.0f);
				AnimalRightArm = glm::translate(AnimalCenterBody, glm::vec3(+0.3f, -0.20f, +0.18f));
				AnimalRightArm = glm::rotate(AnimalRightArm, info_animal.g_rArm, glm::vec3(1.0f, 0.0f, 0.0f));
				AnimalRightArm = glm::scale(AnimalRightArm, glm::vec3(0.2, 0.6, 0.2));
				AnimalRightArm = glm::translate(AnimalRightArm, glm::vec3(0.0f, -0.18f, 0.0f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(AnimalRightArm));
				m_Animal->ChangeColor(0.2f, 0.2f, 0.2f);//
				m_Animal->Render();
				m_Animal->Delete();
				//[왼팔]============================================================
				glm::mat4 AnimalLeftArm = glm::mat4(1.0f);
				AnimalLeftArm = glm::translate(AnimalCenterBody, glm::vec3(-0.3f, -0.20f, +0.18f));
				AnimalLeftArm = glm::rotate(AnimalLeftArm, info_animal.g_lArm, glm::vec3(1.0f, 0.0f, 0.0f));
				AnimalLeftArm = glm::scale(AnimalLeftArm, glm::vec3(0.2, 0.6, 0.2));
				AnimalLeftArm = glm::translate(AnimalLeftArm, glm::vec3(0.0f, -0.18f, 0.0f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(AnimalLeftArm));
				m_Animal->ChangeColor(0.2f, 0.2f, 0.2f);//
				m_Animal->Render();
				m_Animal->Delete();
				//[오른다리]========================================================
				glm::mat4 AnimalRightLeg = glm::mat4(1.0f);
				AnimalRightLeg = glm::translate(AnimalCenterBody, glm::vec3(0.3f, -0.20f, -0.18f));
				AnimalRightLeg = glm::rotate(AnimalRightLeg, info_animal.g_rLeg, glm::vec3(1.0f, 0.0f, 0.0f));
				AnimalRightLeg = glm::scale(AnimalRightLeg, glm::vec3(0.3, 0.6, 0.18));
				AnimalRightLeg = glm::translate(AnimalRightLeg, glm::vec3(0.0f, -0.15f, 0.0f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(AnimalRightLeg));
				m_Animal->ChangeColor(0.2f, 0.2f, 0.2f);//
				m_Animal->Render();
				m_Animal->Delete();

				//[왼다리]==========================================================
				glm::mat4 AnimalLeftLeg = glm::mat4(1.0f);
				AnimalLeftLeg = glm::translate(AnimalCenterBody, glm::vec3(-0.3f, -0.20f, -0.18f));
				AnimalLeftLeg = glm::rotate(AnimalLeftLeg, info_animal.g_lLeg, glm::vec3(1.0f, 0.0f, 0.0f));
				AnimalLeftLeg = glm::scale(AnimalLeftLeg, glm::vec3(0.3, 0.6, 0.18));
				AnimalLeftLeg = glm::translate(AnimalLeftLeg, glm::vec3(0.0f, -0.15f, 0.0f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(AnimalLeftLeg));
				m_Animal->ChangeColor(0.15f, 0.2f, 0.3f);//
				m_Animal->Render();
				m_Animal->Delete();

				//}
			}
		}
	}
	if (Summons02)
	{
		AnimalCenterBody = glm::mat4(1.0f); // 단위행렬로초기화 
		{

			if (startAnimalMove)
			{
				if (countbot == 5)
				{
					s_pos02.x = s_pos02.x + 0.3f;
					s_pos02.y = s_pos02.y;
					s_pos02.z = s_pos02.z - 0.2f;
				}
				else
					AnimalCenterBody = glm::translate(AnimalCenterBody, glm::vec3(s_pos02.x - 0.7f, s_pos02.y, s_pos02.z + 3.4f));
				AnimalCenterBody = glm::rotate(AnimalCenterBody, angle[0], glm::vec3(0.0, 1.0, 0.0));
				AnimalCenterBody = glm::scale(AnimalCenterBody, glm::vec3(0.6f, 0.6f, 1.0f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(AnimalCenterBody));
				m_Animal->ChangeColor(0.0f, 1.0f, 1.0f);//몸통
				m_Animal->Render();
				m_Animal->Delete();


				//[얼굴,코]============================================================
				glm::mat4 Animal_Head = glm::mat4(1.0f);
				Animal_Head = glm::translate(AnimalCenterBody, glm::vec3(0.0, 0.7, +0.25));
				Animal_Head = glm::scale(Animal_Head, glm::vec3(0.5, 0.5, 0.5));
				Animal_Head = glm::scale(Animal_Head, glm::vec3(1.0f, 2.0f, 0.6f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Animal_Head));
				m_Animal->ChangeColor(0.2f, 0.2f, 0.2f);//
				m_Animal->Render();
				m_Animal->Delete();

				glm::mat4 Animal_Neck = glm::mat4(1.0f);
				Animal_Neck = glm::translate(AnimalCenterBody, glm::vec3(0.0, 0.1, +0.25));
				Animal_Neck = glm::scale(Animal_Neck, glm::vec3(0.2, 0.7, 0.2));
				Animal_Neck = glm::scale(Animal_Neck, glm::vec3(1.0f, 2.0f, 0.6f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Animal_Neck));
				m_Animal->ChangeColor(0.2f, 0.2f, 0.2f);//
				m_Animal->Render();
				m_Animal->Delete();

				glm::mat4 Animal_Tail = glm::mat4(1.0f);
				Animal_Tail = glm::translate(AnimalCenterBody, glm::vec3(0.0, 0.1, -0.35));
				Animal_Tail = glm::rotate(Animal_Tail, rotateangle, glm::vec3(0.5, 1.0, 0.0));
				Animal_Tail = glm::scale(Animal_Tail, glm::vec3(0.1, 0.1, 0.35));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Animal_Tail));
				m_Animal->ChangeColor(0.2f, 0.2f, 0.2f);//
				m_Animal->Render();
				m_Animal->Delete();

				Animal_Head = glm::translate(Animal_Head, glm::vec3(0.0, 0.0, 0.4));
				Animal_Head = glm::scale(Animal_Head, glm::vec3(0.2, 0.2, 0.3));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Animal_Head));
				m_Animal->ChangeColor(0.1f, 0.1f, 0.1f);//
				m_Animal->Render();
				m_Animal->Delete();

				glm::mat4 Animal_EARS = glm::mat4(1.0f);
				Animal_EARS = glm::translate(AnimalCenterBody, glm::vec3(+0.2, 0.9, +0.25));
				Animal_EARS = glm::scale(Animal_EARS, glm::vec3(0.1, 0.4, 0.18));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Animal_EARS));
				m_Animal->ChangeColor(0.5f, 0.2f, 0.0f);//
				m_Animal->Render();
				m_Animal->Delete();

				Animal_EARS = glm::translate(AnimalCenterBody, glm::vec3(-0.2, 0.9, +0.25));
				Animal_EARS = glm::scale(Animal_EARS, glm::vec3(0.1, 0.4, 0.18));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Animal_EARS));
				m_Animal->ChangeColor(0.5f, 0.2f, 0.0f);//
				m_Animal->Render();
				m_Animal->Delete();

				//[오른팔]===========================================================
				glm::mat4 AnimalRightArm = glm::mat4(1.0f);
				AnimalRightArm = glm::translate(AnimalCenterBody, glm::vec3(+0.3f, -0.20f, +0.18f));
				AnimalRightArm = glm::rotate(AnimalRightArm, info_animal.g_rArm, glm::vec3(1.0f, 0.0f, 0.0f));
				AnimalRightArm = glm::scale(AnimalRightArm, glm::vec3(0.2, 0.6, 0.2));
				AnimalRightArm = glm::translate(AnimalRightArm, glm::vec3(0.0f, -0.18f, 0.0f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(AnimalRightArm));
				m_Animal->ChangeColor(0.2f, 0.2f, 0.2f);//
				m_Animal->Render();
				m_Animal->Delete();
				//[왼팔]============================================================
				glm::mat4 AnimalLeftArm = glm::mat4(1.0f);
				AnimalLeftArm = glm::translate(AnimalCenterBody, glm::vec3(-0.3f, -0.20f, +0.18f));
				AnimalLeftArm = glm::rotate(AnimalLeftArm, info_animal.g_lArm, glm::vec3(1.0f, 0.0f, 0.0f));
				AnimalLeftArm = glm::scale(AnimalLeftArm, glm::vec3(0.2, 0.6, 0.2));
				AnimalLeftArm = glm::translate(AnimalLeftArm, glm::vec3(0.0f, -0.18f, 0.0f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(AnimalLeftArm));
				m_Animal->ChangeColor(0.2f, 0.2f, 0.2f);//
				m_Animal->Render();
				m_Animal->Delete();
				//[오른다리]========================================================
				glm::mat4 AnimalRightLeg = glm::mat4(1.0f);
				AnimalRightLeg = glm::translate(AnimalCenterBody, glm::vec3(0.3f, -0.20f, -0.18f));
				AnimalRightLeg = glm::rotate(AnimalRightLeg, info_animal.g_rLeg, glm::vec3(1.0f, 0.0f, 0.0f));
				AnimalRightLeg = glm::scale(AnimalRightLeg, glm::vec3(0.3, 0.6, 0.18));
				AnimalRightLeg = glm::translate(AnimalRightLeg, glm::vec3(0.0f, -0.15f, 0.0f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(AnimalRightLeg));
				m_Animal->ChangeColor(0.2f, 0.2f, 0.2f);//
				m_Animal->Render();
				m_Animal->Delete();

				//[왼다리]==========================================================
				glm::mat4 AnimalLeftLeg = glm::mat4(1.0f);
				AnimalLeftLeg = glm::translate(AnimalCenterBody, glm::vec3(-0.3f, -0.20f, -0.18f));
				AnimalLeftLeg = glm::rotate(AnimalLeftLeg, info_animal.g_lLeg, glm::vec3(1.0f, 0.0f, 0.0f));
				AnimalLeftLeg = glm::scale(AnimalLeftLeg, glm::vec3(0.3, 0.6, 0.18));
				AnimalLeftLeg = glm::translate(AnimalLeftLeg, glm::vec3(0.0f, -0.15f, 0.0f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(AnimalLeftLeg));
				m_Animal->ChangeColor(0.15f, 0.2f, 0.3f);//
				m_Animal->Render();
				m_Animal->Delete();

				//}
			}
		}
	}

	if (Bang)
	{
		
		glm::mat4 bulletmat = glm::mat4(1.0f);
		bulletmat = glm::translate(bulletmat, glm::vec3(0.0f, +0.0f, bullet_velocity));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(bulletmat));
		m_bullet->ChangeColor(1.0f, 1.0f, 0.0f);
		m_Animal->Render();

		if (bullet_velocity < villainposz - 10)
		{
			++countbot;
			bullet_velocity = s_pos02.z + 5 ;
			Bang = false;
		    m_Animal->Delete();
		}
	}

	if (Summons02&&Summons01)
	{
		if (Bang)
		{
			glm::mat4 bulletmat = glm::mat4(1.0f);
			bulletmat = glm::translate(bulletmat, glm::vec3(2.0f, +0.0f, bullet_velocity));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(bulletmat));
			m_bullet->ChangeColor(1.0f, 1.0f, 0.0f);
			m_Animal->Render();

			if (bullet_velocity < villainposz - 10)
			{
				++countbot;
				bullet_velocity = s_pos02.z + 5;
				Bang = false;
				m_Animal->Delete();
			}
		}
		if (Bang)
		{

			glm::mat4 bulletmat = glm::mat4(1.0f);
			bulletmat = glm::translate(bulletmat, glm::vec3(-2.0f, +0.0f, bullet_velocity));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(bulletmat));
			m_bullet->ChangeColor(1.0f, 1.0f, 0.0f);
			m_Animal->Render();

			if (bullet_velocity < villainposz - 10)
			{
				++countbot;
				bullet_velocity = s_pos02.z + 5;
				Bang = false;
				m_Animal->Delete();
			}
		}
	}

	if (countbot < 5)
	{
		glm::mat4 m_VillainCenterBody = glm::mat4(1.0f); // 단위행렬로초기화 
		m_VillainCenterBody = glm::translate(m_VillainCenterBody, glm::vec3(0.0f, 2.5f, villainposz));

		//m_VillainCenterBody = glm::rotate(m_VillainCenterBody, -1.6f, glm::vec3(0.0, 1.0, 0.0));
		m_VillainCenterBody = glm::scale(m_VillainCenterBody, glm::vec3(8.0f, 2.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m_VillainCenterBody));
		m_Villain->ChangeColor(1.0f, 1.0f, 0.5f);
		m_Villain->InitBuffer();
		m_Villain->Render();
		m_Villain->Delete();

		glm::mat4 m_Villainlip = glm::mat4(1.0f); // 단위행렬로초기화 
		m_Villainlip = glm::translate(m_VillainCenterBody, glm::vec3(0.0f, lipOpen, 0.0f));
		//m_VillainCenterBody = glm::rotate(m_VillainCenterBody, -1.6f, glm::vec3(0.0, 1.0, 0.0));
		m_Villainlip = glm::scale(m_Villainlip, glm::vec3(1.0f, 0.3f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m_Villainlip));
		m_Villain->ChangeColor(1.0f, 1.0f, 0.5f);
		m_Villain->InitBuffer();
		m_Villain->Render();
		m_Villain->Delete();

		glm::mat4 m_VillainEye01 = glm::mat4(1.0f);
		m_VillainEye01 = glm::translate(m_VillainCenterBody, glm::vec3(-0.15f, 0.2f, +0.4f));
		m_VillainEye01 = glm::scale(m_VillainEye01, glm::vec3(0.15f, 0.2f, 0.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m_VillainEye01));
		m_Villain->ChangeColor(0.0f, 0.0f, 0.0f);
		m_Villain->InitBuffer();
		m_Villain->Render();
		m_Villain->Delete();

		glm::mat4 m_VillainEyeEye01 = glm::mat4(1.0f);
		m_VillainEyeEye01 = glm::translate(m_VillainEye01, glm::vec3(0.0f, 0.0f, +0.4f));
		m_VillainEyeEye01 = glm::scale(m_VillainEyeEye01, glm::vec3(0.5f, 0.5f, 0.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m_VillainEyeEye01));
		m_Villain->ChangeColor(1.0f, 1.0f, 1.0f);
		m_Villain->InitBuffer();
		m_Villain->Render();
		m_Villain->Delete();

		glm::mat4 m_VillainEye02 = glm::mat4(1.0f);
		m_VillainEye02 = glm::translate(m_VillainCenterBody, glm::vec3(+0.15f, 0.2f, +0.4f));
		m_VillainEye02 = glm::scale(m_VillainEye02, glm::vec3(0.15f, 0.2f, 0.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m_VillainEye02));
		m_Villain->ChangeColor(0.0f, 0.0f, 0.0f);
		m_Villain->InitBuffer();
		m_Villain->Render();
		m_Villain->Delete();

		glm::mat4 m_VillainEyeEye02 = glm::mat4(1.0f);
		m_VillainEyeEye02 = glm::translate(m_VillainEye02, glm::vec3(0.0f, 0.0f, +0.4f));
		m_VillainEyeEye02 = glm::scale(m_VillainEyeEye02, glm::vec3(0.5f, 0.5f, 0.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m_VillainEyeEye02));
		m_Villain->ChangeColor(1.0f, 1.0f, 1.0f);
		m_Villain->InitBuffer();
		m_Villain->Render();
		m_Villain->Delete();
	}

	glm::mat4 groung = glm::mat4(1.0f);
	groung = glm::translate(groung, glm::vec3(0.0f, +1.5f, -30.0f));
	groung = glm::scale(groung, glm::vec3(30.0, 8.0, 120.0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(groung));
	m_Stage->SetColor(0.6);
	m_Stage->InitBuffer();
	m_Stage->Render();
	m_Stage->Delete();


	renderer->viewMatrix = view;
	renderer->projMatrix = projection;
	std::chrono::duration<double> diff = chrono::high_resolution_clock::now() - prevTime;
	prevTime = chrono::high_resolution_clock::now();

	renderer->SetCurrentShader("obj");//그리기 전에 어떤 쉐이더로 그릴지 지정
	//for (auto& data : objectList)
	//{
	//	data->Update(diff.count());//월드행렬 계산
	//	data->Render();//쉐이더한테 값 넘겨주고
	//}
	for (auto& data : bgList)
	{
		data->Update(diff.count());//월드행렬 계산
		data->Render();//쉐이더한테 값 넘겨주고
	}
	//====================================================================

	delete[] m_Robot; 
	delete m_Stage; 
	delete mFrontStage;
	delete m_camera; 
	delete m_Animal; 

	glutSwapBuffers();
}

bool movemode = false; 
bool animalmode = false; 
void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		animalmode = true;
		s_robot01.b_Back = true;
		s_robot01.b_Front = false; 
		s_robot01.b_Left = false; 
		s_robot01.b_Right = false;
		break;
	case'a':
		animalmode = true;
		s_robot01.b_Back = false;
		s_robot01.b_Front = false;
		s_robot01.b_Left = true;
		s_robot01.b_Right = false;
		break; 
	case's':
		animalmode = true;
		s_robot01.b_Back = false;
		s_robot01.b_Front = true;
		s_robot01.b_Left = false;
		s_robot01.b_Right = false;
		break; 
	case'd':
		animalmode = true;
		s_robot01.b_Back = false;
		s_robot01.b_Front = false;
		s_robot01.b_Left = false;
		s_robot01.b_Right = true;
		break;
	case ' ':
		Bang = true; 
		break;
	case'o':
		On_CameraView = false;
		On_BackView = true;
		Frist_Person = false;
		break; 
	case'i':
		On_CameraView = true;
		On_BackView = false;
		Frist_Person = false;
		break;
	case 'p':
		On_CameraView = false;
		On_BackView = false;
		Frist_Person = true; 
		break;

	case 'l':
		Summons01 = true; 
		Summons02 = true; 
		break; 
	default:
		break;
	}
	//glutPostRedisplay();
}
bool RotateRateCheck = false;
bool LIPopen = false, LIPclose =false; 

int colorcount = 0;
void Timerfunction(int value)
{
	/*if (movemode)
	{
		if (RotateRateCheck == false)
		{
			info_people.g_rArm += PeopleTick;
			info_people.g_lArm -= PeopleTick;
			info_people.g_rLeg -= PeopleTick;
			info_people.g_lLeg += PeopleTick;
			if (info_people.g_rArm > 0.4f) { RotateRateCheck = true; }
		}
		else if (RotateRateCheck == true)
		{
			info_people.g_rArm -= PeopleTick;
			info_people.g_lArm += PeopleTick;
			info_people.g_rLeg += PeopleTick;
			info_people.g_lLeg -= PeopleTick;
			if (info_people.g_rArm < -0.4f) { RotateRateCheck = false; }
		}*/
	if (animalmode)
	{
		//1번 로봇 움직임=====================
		if (s_robot01.b_Back == true)
		{
			angle[0] = 3.2f;
			s_pos02.z -= 0.255f;
			cameraDirection.b -= 0.255f;
			lightposz -= 0.3; 
		}
		if (s_robot01.b_Front == true)
		{
			angle[0] = 0.0f;
			s_pos02.z += 0.25f;
			cameraDirection.b += 0.255f;
			lightposz += 0.3;
		}
		if (s_robot01.b_Left == true)
		{
			angle[0] = -1.6f;
			s_pos02.x -= 0.25f;
			cameraDirection.r -= 0.255f;
		}
		if (s_robot01.b_Right == true)
		{
			angle[0] = 1.6f;
			s_pos02.x += 0.25f;
			cameraDirection.r += 0.255f;
		}

		if (RotateRateCheck == false)
		{
			info_animal.g_rArm += AnimalTick;
			info_animal.g_lArm -= AnimalTick;
			info_animal.g_rLeg -= AnimalTick;
			info_animal.g_lLeg += AnimalTick;
			if (info_animal.g_rArm > 0.4f) { RotateRateCheck = true; }
		}
		else if (RotateRateCheck == true)
		{
			info_animal.g_rArm -= AnimalTick;
			info_animal.g_lArm += AnimalTick;
			info_animal.g_rLeg += AnimalTick;
			info_animal.g_lLeg -= AnimalTick;
			if (info_animal.g_rArm < -0.4f) { RotateRateCheck = false; }
		}
	}

	if (LIPopen == false)
	{
		lipOpen -= 0.05;
		if (lipOpen <= -0.8f) { LIPopen = true; }
	}
	else if (LIPopen == true)
	{
		lipOpen += 0.05;
		if (lipOpen >= -0.2f) { LIPopen = false; }
	}
	villainposz -= 0.3;

	bullet_velocity -= 1.3;
	if (countbot < 5)
	{
		if (colorcount < 5)
		{
			++colorcount;
		}
		if (colorcount == 5)
		{
			s_posColor.x = rand() / (float)RAND_MAX;
			s_posColor.y = rand() / (float)RAND_MAX;
			s_posColor.z = rand() / (float)RAND_MAX;
			colorcount = 0;
		}
	}
	if (countbot == 5)
	{
		s_posColor.x = 1.0f ;
		s_posColor.y = 1.0f ;
		s_posColor.z = 1.0f ;
	}

	glutTimerFunc(200, Timerfunction, 1);
	glutPostRedisplay();
}
void SpecialKeyboard(int key, int x, int y) {}
void KeyboardUP(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		animalmode = false;
		break;
	case'a':
		animalmode = false;
		break;
	case's':
		animalmode = false;
		break;
	case'd':
		animalmode = false;
		break;
	}
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		animalmode = true;

	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		unProject(x, y);
		//꽃생성
		auto flower = new MeshObject();
		flower->color = { 1, 1, 1,1 };  //0.5,1,1 - 잔디색
		BasicObjectDesc objDesc;
		objDesc.primitiveType = GL_TRIANGLES;
		flower->Initialize(objDesc, renderer, meshMap["Flower"], { wx,wy,wz }, { 5,0,0 }, { 0.1,0.1,0.1 });
		bgList.push_back(flower);
	}
}

void unProject(int xCursor, int yCursor)
{	//마우스 픽킹
	GLdouble projection[16];
	GLdouble modelView[16];
	GLint viewPort[4];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	glGetIntegerv(GL_VIEWPORT, viewPort);

	GLfloat zCursor, winX, winY;
	winX = (float)xCursor;
	winY = (float)viewPort[3] - (float)yCursor;
	glReadPixels((int)winX, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zCursor);

	int index = 0;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			projection[index] = renderer->projMatrix[i][j];
			modelView[index] = renderer->viewMatrix[i][j];
			index++;
		}
	}

	if (gluUnProject(winX, winY, zCursor, modelView, projection, viewPort, &wx, &wy, &wz) == GLU_FALSE) {
		printf("실패\n");
	}
}

void Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glutPostRedisplay();
}


void Follow_Camera_Robot(glm::vec3 TargetcameraPos, glm::vec3 Robot, int numberrobot)
{
	if (numberrobot == 1)
	{
		if (TargetcameraPos.r + 0.7 < Robot.r)
		{
			s_robot01.b_Left = true;
			s_robot01.b_Back = false;
			s_robot01.b_Front = false;
			s_robot01.b_Right = false;
		}
		else if (TargetcameraPos.r - 0.7 > Robot.r)
		{
			s_robot01.b_Left = false;
			s_robot01.b_Back = false;
			s_robot01.b_Front = false;
			s_robot01.b_Right = true;
		}
	}
}


bool Initialize()
{
	bool check = true;

	InitDesc desc;
	desc.width = WINDOW_SIZE_WIDTH;
	desc.height = WINDOW_SIZE_HEIGHT;


	renderer = new Renderer{ desc };
	//renderer->AddShaderWithTwoParam("vertex.glsl", "pixel.glsl", "basic");
	//renderer->AddShaderWithTwoParam("shader\lineVertex.glsl", "shader\pixel.glsl", "line");
	renderer->AddShaderWithTwoParam("shader\\vertexObJ.glsl", "shader\\pixelLight.glsl", "obj");
	renderer->light.sunLight = { 0,20,0 };
	renderer->light.ambientLight = 0.2f;

	renderer->SetViewMatrix({ 0,20,20 }, { 0,0,0 }, { 0,1,0 });
	renderer->SetProjMatrix(90.f, 0.1f, 100.0f);

	meshMap["Cube"] = new Mesh();
	check = meshMap["Cube"]->CreateMeshByObj("Mesh/cube.obj");

	meshMap["Tree"] = new Mesh();
	check = meshMap["Tree"]->CreateMeshByObj("Mesh/tree.obj");

	//==========================================트리 사용할거=========
		{
			auto Tree = new MeshObject();
			Tree->color = { 0, 1, 0,1 };
			BasicObjectDesc objDesc;
			objDesc.primitiveType = GL_TRIANGLES;
			Tree->Initialize(objDesc, renderer, meshMap["Tree"], { 5,0,0 }, { 0,0,0 }, { 0.01,0.01,0.01 });
			bgList.push_back(Tree);

			auto Tree2 = new MeshObject();
			Tree2->color = { 0, 1, 0,1 };
			Tree2->Initialize(objDesc, renderer, meshMap["Tree"], { -5,0,0 }, { 0,0,0 }, { 0.01,0.01,0.01 });
			bgList.push_back(Tree2);

			auto Tree3 = new MeshObject();
			Tree3->color = { 0, 1, 0,1 };
			Tree3->Initialize(objDesc, renderer, meshMap["Tree"], { -5,0,-10 }, { 0,0,0 }, { 0.01,0.01,0.01 });
			bgList.push_back(Tree3);

			auto Tree4 = new MeshObject();
			Tree4->color = { 0, 1, 0,1 };
			Tree4->Initialize(objDesc, renderer, meshMap["Tree"], { -5,0,-20 }, { 0,0,0 }, { 0.01,0.01,0.01 });
			bgList.push_back(Tree4);

			auto Tree5 = new MeshObject();
			Tree5->color = { 0, 1, 0,1 };
			Tree5->Initialize(objDesc, renderer, meshMap["Tree"], { -5,0,-30 }, { 0,0,0 }, { 0.01,0.01,0.01 });
			bgList.push_back(Tree5);

			auto Tree6 = new MeshObject();
			Tree6->color = { 0, 1, 0,1 };
			Tree6->Initialize(objDesc, renderer, meshMap["Tree"], { -5,0,-40 }, { 0,0,0 }, { 0.01,0.01,0.01 });
			bgList.push_back(Tree6);


			auto Tree7 = new MeshObject();
			Tree7->color = { 0, 1, 0,1 };
			Tree7->Initialize(objDesc, renderer, meshMap["Tree"], { 5,0,-10 }, { 0,0,0 }, { 0.01,0.01,0.01 });
			bgList.push_back(Tree7);

			auto Tree8 = new MeshObject();
			Tree8->color = { 0, 1, 0,1 };
			Tree8->Initialize(objDesc, renderer, meshMap["Tree"], { 5,0,-20 }, { 0,0,0 }, { 0.01,0.01,0.01 });
			bgList.push_back(Tree8);

			auto Tree9 = new MeshObject();
			Tree9->color = { 0, 1, 0,1 };
			Tree9->Initialize(objDesc, renderer, meshMap["Tree"], { 5,0,-30 }, { 0,0,0 }, { 0.01,0.01,0.01 });
			bgList.push_back(Tree9);

			auto Tree10 = new MeshObject();
			Tree10->color = { 0, 1, 0,1 };
			Tree10->Initialize(objDesc, renderer, meshMap["Tree"], { 5,0,-40 }, { 0,0,0 }, { 0.01,0.01,0.01 });
			bgList.push_back(Tree10);
	}

	//========================================================================================

	meshMap["Flower"] = new Mesh();
	check = meshMap["Flower"]->CreateMeshByObj("Mesh/flower.obj");

	//meshMap["Box"] = new Mesh();
	//check = meshMap["Box"]->CreateMeshByObj("Mesh/box.obj");

	//meshMap["Cake"] = new Mesh();
	//check = meshMap["Cake"]->CreateMeshByObj("Mesh/cake.obj");

	//==================================================================================
	/*meshMap["Chicken"] = new Mesh();
	check = meshMap["Chicken"]->CreateMeshByObj("Mesh/chicken.obj");*/

	//meshMap["Apple"] = new Mesh();
	//check = meshMap["Apple"]->CreateMeshByObj("Mesh/apple.obj");

	//meshMap["Cookie"] = new Mesh();
	//check = meshMap["Cookie"]->CreateMeshByObj("Mesh/cookie2.obj");

	//meshMap["Straberry"] = new Mesh();
	//check = meshMap["Straberry"]->CreateMeshByObj("Mesh/strawberry2.obj");

	//meshMap["Turkey"] = new Mesh();
	//check = meshMap["Turkey"]->CreateMeshByObj("Mesh/turkey.obj");

	meshMap["Flag"] = new Mesh();
	check = meshMap["Flag"]->CreateMeshByObj("Mesh/flag.obj");

	meshMap["Trophy"] = new Mesh();
	check = meshMap["Trophy"]->CreateMeshByObj("Mesh/tro.obj");
	


	//========================================================
	if (!check)
	{
		cout << "Mesh Load Failed" << endl;
		return false;
	}

	return true;
}

void CreateFood(/*float pos01, float pos02*/)
{
	auto flagL = new MeshObject();
	flagL->color = { 1, 0, 0,1 };
	BasicObjectDesc objDesc;
	objDesc.primitiveType = GL_TRIANGLES;
	flagL->Initialize(objDesc, renderer, meshMap["Flag"], { -4,0,-5 }, { 0,0,0 }, { 1,1,1 });
	bgList.push_back(flagL);

	auto flagR = new MeshObject();
	flagR->color = { 1, 0, 0,1 };
	flagR->Initialize(objDesc, renderer, meshMap["Flag"], { 4,0,-5 }, { 0,0,0 }, { 1,1,1 });
	bgList.push_back(flagR);

	auto trophy = new MeshObject();
	trophy->color = { 0.9, 0.6, 0,1 };
	trophy->Initialize(objDesc, renderer, meshMap["Trophy"], { 0,0,-2 }, { 0,0,0 }, { 0.3,0.3,0.3 });
	bgList.push_back(trophy);

	//auto apple = new MeshObject();
	//apple->color = { 1, 0, 0,1 };
	//apple->Initialize(objDesc, renderer, meshMap["Apple"], { 1,0,-2 }, { 0,5,0 }, { 0.1,0.1,0.1 });
	//bgList.push_back(apple);

	//auto straberry = new MeshObject();
	//straberry->color = { 1, 0, 0,1 };
	//straberry->Initialize(objDesc, renderer, meshMap["Straberry"], { -1,0,-2 }, { 0,50,0 }, { 0.1,0.1,0.1 });
	//bgList.push_back(straberry);

	//auto cookie = new MeshObject();
	//cookie->color = { 1, 0.5, 0,1 };
	//cookie->Initialize(objDesc, renderer, meshMap["Cookie"], { -2,0,-2 }, { 0,0,0 }, { 0.1,0.1,0.1 });
	//bgList.push_back(cookie);



}