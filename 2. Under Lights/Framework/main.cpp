#include "stdafx.h"
#include "CGameframework.h"

CGameFrameWork gGameFramework; 

void main(int argc, char** argv)
{
	gGameFramework.FrameAdvance(argc, argv);
}