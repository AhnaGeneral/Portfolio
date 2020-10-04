
struct MATERIAL
{
	float4		    m_cAmbient;
	float4		    m_cDiffuse;
	float4		    m_cSpecular; //a = power
	float4		    m_cEmissive;
};

cbuffer cbCameraInfo : register(b1)
{
	matrix		    gmtxView : packoffset(c0);
	matrix		    gmtxProjection : packoffset(c4);
	matrix			gmtxInverseView : packoffset(c8);
	matrix			gmtxInverseProjection : packoffset(c12);
	float3		    gvCameraPosition : packoffset(c16);
	float3			gvCameraNoraml :packoffset(c17);
};

cbuffer cbGameObjectInfo : register(b2)
{
	matrix		    gmtxGameObject : packoffset(c0); // 16
	uint            gnObjectID : packoffset (c4); 
	uint			gnTextureMask : packoffset(c5);
	float           gnSwordEffectTime : packoffset(c6);
};

cbuffer cbOrthoInfo : register(b4)
{
	matrix		    gmtxOrtho: packoffset(c0);
	matrix		    gmtxOrthoView: packoffset(c4);
};

cbuffer cbRadiationLevel : register(b11) // 플레이어 위치
{
	uint	 gf3RadiationLevel :packoffset(c0);
}

cbuffer cbShadowCameraInfo : register(b5) // 그림자 카메라 
{
	matrix		    shadowgmtxView : packoffset(c0);
	matrix		    shadowgmtxProjection : packoffset(c4);
	matrix			shadowgmtxInverseView : packoffset(c8);
	matrix			shadowgmtxInverseProjection : packoffset(c12);
	float3		    shadowgvCameraPosition : packoffset(c16);
};


cbuffer cbPlayerPosition : register(b6) // 플레이어 위치
{
	float3			gf3PlayerPos :packoffset(c0);
}



cbuffer cbNoiseBuffer : register(b9)
{
	float    frameTime : packoffset(c0);
	float3   scrollSpeeds : packoffset(c0.y);
	float3   scales : packoffset (c1);
	float    padding : packoffset (c1.w);
	float3	 angle : packoffset(c2);
	float    fogTime : packoffset(c2.w);
};


#include "Light.hlsl"

SamplerState gSamplerState : register(s0);
SamplerState gSamplerClamp : register(s1);

Texture2D gtxtScene : register(t1);  // 0  
Texture2D gtxtNormal : register(t2); // 1
Texture2D gtxtDepth : register(t3);  // 2 
//Texture2D gtxtT4RenderTarget : register(t4);
Texture2D gtxtNonLightNoise : register(t5); // 4 
Texture2D gtxtEmmisive : register(t6); // 4 



Texture2D gtxtLight : register(t21);
Texture2D gtxtShadowCameraTexture : register(t22);

Texture2D gtxtAlbedoTexture : register(t7);
Texture2D gtxtSpecularTexture : register(t8);
Texture2D gtxtNormalTexture : register(t9);
Texture2D gtxtMetallicTexture : register(t10);
Texture2D gtxtEmissionTexture : register(t11);

TextureCube gtxtSkyCubeTexture : register(t12);
Texture2D gtxCloudTextures : register(t13);

Texture2D gtxtBaseColorNoiseTex : register(t14);
Texture2D gtxtAlphaNoiseTex : register(t15);
Texture2D gtxtNoiseTex : register(t16);

Texture2D gtxtFinalAlpha : register(t17);
Texture2D gtxtAlpha01 : register(t18);
Texture2D gtxtAlpha02 : register(t19);

Texture2D gtxtWaterNormal : register(t20);
Texture2D gtxtSceneDepthTexture : register(t23);
Texture2D gtxtCopySunTex : register(t24); // 썬카메라를 새롭게 리소스 만들었어요
Texture2D gtxtLobbyTex : register(t25); // 썬카메라를 새롭게 리소스 만들었어요

Texture2D gtxtRootUITexture : register(t26);
Texture2D gtxtHandLighTexture : register(t27);
Texture2D gtxtHPKitdTexture : register(t28);
Texture2D gtxtPillddsTexture : register(t29);
Texture2D gtxtMinimapFogTexture : register(t30);


Texture2D gtxtEffectTexture : register(t31);
Texture2D gtxtDissolveTexture : register(t69);
Texture2D gtxtRealEffectTexture : register(t70);


static float gtx_Buffer_width = 1366.0f;
static float gtx_Buffer_height = 768.f;

struct PS_MULTIPLE_RENDER_TARGETS_OUTPUT
{
	float4 color        : SV_TARGET0;
	float4 normal       : SV_TARGET1;
	float4 depth        : SV_TARGET2;
	float4 ShadowCamera : SV_TARGET3;
	float4 NonLight		: SV_TARGET4;
	float4 EmmisiveMRT	: SV_TARGET5;
};

struct PS_MULTIPLE_SPECIAL_FOG
{
	float4 color        : SV_TARGET0;
	float4 normal       : SV_TARGET1;
};

struct PS_NONLIGHT_MRT_OUTPUT
{
	float4 NonLight : SV_TARGET4;
};

struct PS_EMMISIVE_MRT_OUTPUT
{
	float4 EmmisiveMRT : SV_TARGET5;
};

struct PS_SHADOW_OUTPUT
{
	float4 ShadowTex : SV_TARGET0;
};

static int2 gnOffsets[9] = { { -1,-1 },{ 0,-1 },{ 1,-1 },{ -1,0 },{ 0,0 },{ 1,0 },{ -1,1 },{ 0,1 },{ 1,1 } };

#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_SPECULAR_MAP		0x02
#define MATERIAL_NORMAL_MAP			0x04
#define MATERIAL_METALLIC_MAP		0x08
#define MATERIAL_EMISSION_MAP		0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40
