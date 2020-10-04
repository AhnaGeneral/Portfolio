#pragma once

#define RANDOM_COLOR	XMFLOAT4(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX))

//[ Scene ]=====================================
#define ROOT_PARAMETER_CAMERA				0
#define ROOT_PARAMETER_OBJECT				1 
#define ROOT_PARAMETER_PLAYER				1

#define ROOT_PARAMETER_ALBEDO_TEX		    2
#define ROOT_PARAMETER_SPECULAR_TEX		    3
#define ROOT_PARAMETER_NORMAL_TEX		    4
#define ROOT_PARAMETER_METALLIC_TEX		    5
#define ROOT_PARAMETER_EMISSION_TEX		    6

#define ROOT_PARAMETER_BONE_TRANSFORMS      7
#define ROOT_PARAMETER_BONE_OFFSETS         8

#define ROOT_PARAMETER_SKYBOX_TEX		    9
#define ROOT_PARAMETER_CLOUD_TEX		    10
#define ROOT_PARAMETER_TERRAIN_TEX		    11

#define ROOT_PARMAMETER_WATER_NORMAL_TEX	12
#define ROOT_PARAMETER_NOISEBUFFER		    13
#define ROOT_PARAMETER_DISTORTIONBUFFER     14
#define ROOT_PARAMETER_NOISE_TEX            15
#define ROOT_PARAMETER_FOG_TEX              16

#define ROOT_PARAMETER_SHADOWCAMERA	        17
#define ROOT_PARAMETER_SHADOWMAP	        18
#define ROOT_PARAMETER_SCENEDEPTHTEX	    19

#define ROOT_PARAMETER_RADIATIONLEVEL       20 // 터레인 테셀레이션 팩터로 재활용
#define ROOT_PARAMETER_EFFECT               21

#define ROOT_PARAMETER_DISSOLVE_MAP			22
#define ROOT_PARAMETER_PARTICLE				23 
#define ROOT_PARAMETER_EFFECTCBV		    24






//[ FinalMRT ]=====================================
#define ROOT_PARAMETER_CDN_MRT				0 
// 1.   ROOT_PARAMETER_OBJECT               1
#define ROOT_PARAMETER_ORTHO				2
#define ROOT_PARAMETER_LIGHT_MRT			3
#define ROOT_PARAMETER_SHADOW_MRT           4
//ForMinimap
#define ROOT_PARAMETER_PLAYER_POS			5
#define ROOT_PARAMETER_HP_TEX               6 
#define ROOT_PARAMETER_ITEM_TEX             7  
#define ROOT_PARAMETER_ITEM_REACT           8  
#define ROOT_PARAMETER_PLAYER_HP            9  
#define ROOT_PARAMETER_MINIIMAPFOG         10
//#define



//[ LightMRT ]=====================================
// 0.   ROOT_PARAMETER_CDN_MRT              0
#define ROOT_PARAMETER_LIGHT				1
#define ROOT_PARAMETER_CAMERA2				2 


//[LobbyScene]=====================================
#define ROOT_PARAMETER_LOBBYTEX             0


//[ Object Type ]====================================
//Static
#define OBJECT_TYPE_ROCK				0
#define OBJECT_TYPE_FENCE				1
#define OBJECT_TYPE_TRMTOWER			2
#define OBJECT_TYPE_BONE				3
#define OBJECT_TYPE_HOUSE				4
#define OBJECT_TYPE_TREE				5
#define OBJECT_TYPE_BRIDGE				6
#define OBJECT_TYPE_FENCE2			    7 
#define OBJECT_TYPE_WARNING		        8 




// Dynamic
#define OBJECT_TYPE_ITEMBOX				0
#define OBJECT_TYPE_DRUGMAKER			1
#define OBJECT_TYPE_RESEARCHER			2



//[IndoorScene] =======================================
#define OBJECT_INDOOR_TYPE_DESK         0

#define OBJECT_INDOOR_TYPE_DOGGY        1
#define OBJECT_INDOOR_TYPE_DUGGY        2
#define OBJECT_INDOOR_TYPE_MOUSE        3
#define OBJECT_INDOOR_TYPE_CAT          4
#define OBJECT_INDOOR_TYPE_GRANNY       5
#define OBJECT_INDOOR_TYPE_TOSM         6
#define OBJECT_INDOOR_TYPE_YANGMAL      7

#define OBJECT_INDOOR_TYPE_CHAIR        8
#define OBJECT_INDOOR_TYPE_BOARD        9
#define OBJECT_INDOOR_TYPE_DOOR         10
#define OBJECT_INDOOR_TYPE_PASSWORD     11
#define OBJECT_INDOOR_TYPE_ANIMALALL    12
#define OBJECT_INDOOR_TYPE_BOSSMACHINE  13
#define OBJECT_INDOOR_TYPE_BOSSMONITER  14









#define OBJECT_INDOOR_TYPE_DESKOPEN  	0
#define OBJECT_INDOOR_TYPE_LABORATOR	3
#define OBJECT_INDOOR_TYPE_DRUGMAKER	1
#define OBJECT_INDOOR_TYPE_DOOROPEN  	2






/*Monster*/
//---------------------------------------------------
#define MONSTER_TYPE_YANGMAL			0
#define MONSTER_TYPE_TOSM				1

#define MONSTER_STATE_IDLE				0
#define MONSTER_STATE_WALK				1
#define MONSTER_STATE_ATTACK			2
#define MONSTER_STATE_STUN				3
#define MONSTER_STATE_DAMAGEING			4
#define MONSTER_STATE_RETURNING			5
#define MONSTER_STATE_PURIFYING			6
#define MONSTER_STATE_BACK				7


/*JohnsonAnimation*/
//---------------------------------------------------
#define JOHNSON_ANIAMATION_IDLE			0
#define JOHNSON_ANIAMATION_WALK			1
#define JOHNSON_ANIAMATION_ATTACK		2
#define JOHNSON_ANIAMATION_RUN  		3
#define JOHNSON_ANIAMATION_TIPING  		4


/*ItemType*/
//------------------------------------------------
#define ITEM_TYPE_HANDLIGHT				0
#define ITEM_TYPE_HPKIT					1
#define ITEM_TYPE_PILL					2
#define ITEM_TYPE_MAPSEGMENT			3
#define ITEM_NONE                       4


/*CameraCullRange*/
#define CAMERA_CULL_RANGE				500	

/*SCENEPROGRESS*/
//-----------------------------------------------
#define PROGRESS_FILED1					1
#define PROGRESS_FILED2					2
#define PROGRESS_FILED3					3
//-------------------------------------------------


//Scene State ________________________________ 
#define SCENE_LOBBY           0  
#define SCENE_STAGE_OUTDOOR   1 
#define SCENE_STAGE_INDOOR    2 
#define SCENE_ENDING           3
//____________________________________________


#define EPSILON						1.0e-10f