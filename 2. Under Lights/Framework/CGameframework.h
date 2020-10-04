#pragma once 
//[window size]=======================
#define WINDOW_SIZE_WIDTH 800
#define WINDOW_SIZE_HEIGHT 680
//====================================
void RenderScene();
void Keyboard(unsigned char key, int x, int y);
void Timerfunction(int value);
void SpecialKeyboard(int key, int x, int y);
void KeyboardUP(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void unProject(int xCursor, int yCursor);
void Reshape(int w, int h);
void CreateFood(/*float pos01, float pos02*/);
class CGameFrameWork
{
public:
	CGameFrameWork(); 
	virtual ~CGameFrameWork();

public:
	void FrameAdvance(int argc, char** argv);
};

