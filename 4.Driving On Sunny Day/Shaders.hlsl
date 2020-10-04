struct MATERIAL
{
	float4					m_cAmbient;
	float4					m_cDiffuse;
	float4					m_cSpecular; //a = power
	float4					m_cEmissive;
};

cbuffer cbCameraInfo : register(b1)
{
	matrix					gmtxView : packoffset(c0);
	matrix					gmtxProjection : packoffset(c4);
	float3					gvCameraPosition : packoffset(c8);
};

cbuffer cbGameObjectInfo : register(b2)
{
	matrix					gmtxGameObject : packoffset(c0);
	MATERIAL				gMaterial : packoffset(c4);
};

cbuffer cbTsInfo : register(b5)
{
	float			 gvTSTactor : packoffset(c0);
	float            gvTSInsideFactor : packoffset(c1);
};


#include "Light.hlsl"
//============================================================================================
Texture2D gtxtTexture : register(t0);
SamplerState gWrapSamplerState : register(s0);
SamplerState gClampSamplerState : register(s1);

struct VS_LIGHTING_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};
struct VS_LIGHTING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 uv : TEXCOORD;
};

VS_LIGHTING_OUTPUT VSLighting(VS_LIGHTING_INPUT input)
{
	VS_LIGHTING_OUTPUT output;

	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv; 

	return(output);
}

float4 PSLighting(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
	input.normalW = normalize(input.normalW);
	float4 color = Lighting(input.positionW, input.normalW);
	float4 cColor = gtxtTexture.Sample(gWrapSamplerState, input.uv);
	
	cColor = lerp(color, cColor, 0.5f);
	return(cColor);
}
//=================================================================================================
Texture2D gtxTerrainBaseTexture : register(t1); 
Texture2D gtxTerrainDetailTexture: register(t2);  

struct VS_TERRAIN_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

struct VS_TERRAIN_TESSELLATION_OUTPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

VS_TERRAIN_TESSELLATION_OUTPUT VSTerrainTessellation(VS_TERRAIN_INPUT input)
{
	VS_TERRAIN_TESSELLATION_OUTPUT output;

	output.position = input.position;
	output.color = input.color;
	output.uv0 = input.uv0;
	output.uv1 = input.uv1;

	return(output);
}

// HS-> TS -> DS 


struct HS_TERRAIN_TESSELLATION_CONSTANT
{
	float fTessEdges[4] : SV_TessFactor;
	float fTessInsides[2] : SV_InsideTessFactor;
};

struct HS_TERRAIN_TESSELLATION_OUTPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};


void BernsteinCoeffcient5x5(float t, out float fBernstein[5])
{
	float tInv = 1.0f - t;
	fBernstein[0] = tInv * tInv * tInv * tInv;
	fBernstein[1] = 4.0f * t * tInv * tInv * tInv;
	fBernstein[2] = 6.0f * t * t * tInv * tInv;
	fBernstein[3] = 4.0f * t * t * t * tInv;
	fBernstein[4] = t * t * t * t;
}

float3 CubicBezierSum5x5(OutputPatch<HS_TERRAIN_TESSELLATION_OUTPUT, 25> patch, float uB[5], float vB[5])
{
	float3 f3Sum = float3(0.0f, 0.0f, 0.0f);
	f3Sum = vB[0] * (uB[0] * patch[0].position + uB[1] * patch[1].position + uB[2] * patch[2].position + uB[3] * patch[3].position + uB[4] * patch[4].position);
	f3Sum += vB[1] * (uB[0] * patch[5].position + uB[1] * patch[6].position + uB[2] * patch[7].position + uB[3] * patch[8].position + uB[4] * patch[9].position);
	f3Sum += vB[2] * (uB[0] * patch[10].position + uB[1] * patch[11].position + uB[2] * patch[12].position + uB[3] * patch[13].position + uB[4] * patch[14].position);
	f3Sum += vB[3] * (uB[0] * patch[15].position + uB[1] * patch[16].position + uB[2] * patch[17].position + uB[3] * patch[18].position + uB[4] * patch[19].position);
	f3Sum += vB[4] * (uB[0] * patch[20].position + uB[1] * patch[21].position + uB[2] * patch[22].position + uB[3] * patch[23].position + uB[4] * patch[24].position);
	return(f3Sum);
}

[domain("quad")]                                       // 사각형
[partitioning("integer")]                              // 파이셔널을 가우스로
[outputtopology("triangle_cw")]                        // 시계방향
[outputcontrolpoints(25)]                              // 제어점의 개수
[patchconstantfunc("VSTerrainTessellationConstant")]   // 패치상수함수의 이름
[maxtessfactor(64.0f)]                                 // 최대 테셀레이션 인자
HS_TERRAIN_TESSELLATION_OUTPUT HSTerrainTessellation(InputPatch<VS_TERRAIN_TESSELLATION_OUTPUT, 25> input, uint i : SV_OutputControlPointID)
{
	HS_TERRAIN_TESSELLATION_OUTPUT output;

	output.position = input[i].position;
	output.color = input[i].color;
	output.uv0 = input[i].uv0;
	output.uv1 = input[i].uv1;

	return(output);
}

HS_TERRAIN_TESSELLATION_CONSTANT VSTerrainTessellationConstant(InputPatch<VS_TERRAIN_TESSELLATION_OUTPUT, 25> input)
{
	HS_TERRAIN_TESSELLATION_CONSTANT output;

	output.fTessEdges[0] = gvTSTactor;
	output.fTessEdges[1] = gvTSTactor;
	output.fTessEdges[2] = gvTSTactor;
	output.fTessEdges[3] = gvTSTactor;
	output.fTessInsides[0] = gvTSInsideFactor;
	output.fTessInsides[1] = gvTSInsideFactor;

	return(output);
}

struct DS_TERRAIN_TESSELLATION_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

[domain("quad")]
DS_TERRAIN_TESSELLATION_OUTPUT DSTerrainTessellation(HS_TERRAIN_TESSELLATION_CONSTANT patchConstant, float2 uv : SV_DomainLocation, OutputPatch<HS_TERRAIN_TESSELLATION_OUTPUT, 25> patch)
{
	DS_TERRAIN_TESSELLATION_OUTPUT output = (DS_TERRAIN_TESSELLATION_OUTPUT)0;

	float uB[5], vB[5];
	BernsteinCoeffcient5x5(uv.x, uB);
	BernsteinCoeffcient5x5(uv.y, vB);

	output.color = lerp(lerp(patch[0].color, patch[4].color, uv.x), lerp(patch[20].color, patch[24].color, uv.x), uv.y);
	output.uv0 = lerp(lerp(patch[0].uv0, patch[4].uv0, uv.x), lerp(patch[20].uv0, patch[24].uv0, uv.x), uv.y);
	output.uv1 = lerp(lerp(patch[0].uv1, patch[4].uv1, uv.x), lerp(patch[20].uv1, patch[24].uv1, uv.x), uv.y);

	float3 position = CubicBezierSum5x5(patch, uB, vB);
	matrix mtxWorldViewProjection = mul(mul(gmtxGameObject, gmtxView), gmtxProjection);
	output.position = mul(float4(position, 1.0f), mtxWorldViewProjection);

	return(output);
}

float4 PSTerrainTessellation(DS_TERRAIN_TESSELLATION_OUTPUT input) : SV_TARGET
{
	float4 cBaseTexColor = gtxTerrainBaseTexture.Sample(gWrapSamplerState, input.uv0);
	float4 cDetailTexColor = gtxTerrainDetailTexture.Sample(gWrapSamplerState, input.uv1);
	float4 cColor = saturate(lerp(cBaseTexColor, cDetailTexColor, 0.6));

	return(cColor);
}

//============================================================================================
Texture2D gtxWaterTexture: register(t3);

struct VS_WATER_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

struct VS_WATER_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VS_WATER_OUTPUT VSWater(VS_WATER_INPUT input)
{
	VS_WATER_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

float4 PSWater(VS_WATER_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxWaterTexture.Sample(gWrapSamplerState, input.uv) *0.6f ;
	//float4 cColor = (1.0f, 1.0f, 0.0f, 1.0f);
	return(cColor);
}

//============================================================================================
Texture2D gtxtBillboardTextures: register(t4);

struct VS_BILLBOARD_INPUT
{
	float3 posW : POSITION;
	float2 sizeW : SIZE; 
};

struct VS_BILLBOARD_OUTPUT
{
	float3 centerW : POSITION;
	float2 sizeW : SIZE;
};

struct GS_BILLBOARD_OUTPUT
{
	float4 posH :SV_POSITION;
	float3 posW :POSITION; 
	float2 UV: TEXCOORD; 
};

VS_BILLBOARD_OUTPUT VSBillboard(VS_BILLBOARD_INPUT input)
{
	VS_BILLBOARD_OUTPUT output;

	output.sizeW              = input.sizeW;
	output.centerW            = input.posW;
	return(output);
}

[maxvertexcount(4)]
void GS(point VS_BILLBOARD_OUTPUT input[1],  inout TriangleStream<GS_BILLBOARD_OUTPUT> outStream)
{
	float3 vUP           = float3(0.0f, 1.0f, 0.0f); 
	float3 vLook         = gvCameraPosition.xyz - input[0].centerW;
	vLook                = normalize(vLook);
	float3 vRight        = cross(vUP, vLook);
					     
	float fHalfW         = input[0].sizeW.x * 0.5f;
	float fHalfH         = input[0].sizeW.y * 0.5f;

	float4 pVertices[4]; 
	pVertices[0]         = float4(input[0].centerW + fHalfW * vRight - fHalfH * vUP, 1.0f);
	pVertices[1]         = float4(input[0].centerW + fHalfW * vRight + fHalfH * vUP, 1.0f);
	pVertices[2]         = float4(input[0].centerW - fHalfW * vRight - fHalfH * vUP, 1.0f);
	pVertices[3]         = float4(input[0].centerW - fHalfW * vRight + fHalfH * vUP, 1.0f);

	float2 pUVs[4] = { float2 (0.0f, 1.0f), float2(0.0f, 0.0f), float2(1.0f, 1.0f), float2(1.0f, 0.0f) }; 

	GS_BILLBOARD_OUTPUT output;

	for (int i = 0; i < 4; ++i)
	{
		output.posW       = pVertices[i].xyz;
		output.posH       = mul(mul(pVertices[i], gmtxView), gmtxProjection);
		output.UV         = pUVs[i];
		outStream.Append(output);
	}
}

float4 PSBillboard(GS_BILLBOARD_OUTPUT input) : SV_TARGET
{
	float3 uvw = float3 (input.UV,0.0f);
    float4 cColor = gtxtBillboardTextures.Sample(gWrapSamplerState, uvw);
	clip(cColor.a - 0.1f);
	return(cColor);
}

//============================================================================================
Texture2D gtxStartViewTexture: register(t5);

struct VS_STARTVIEW_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

struct VS_STARTVIEW_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VS_STARTVIEW_OUTPUT VSStartboard(VS_STARTVIEW_INPUT input)
{
	VS_STARTVIEW_OUTPUT output;

	output.position = float4(input.position, 1.0f);
	output.uv = input.uv;

	return(output);
}

float4 PSStartboard(VS_STARTVIEW_OUTPUT output) : SV_TARGET
{
	float4 cColor = gtxStartViewTexture.Sample(gWrapSamplerState, output.uv);
	//float4 cColor = (1.0f, 1.0f, 0.0f, 1.0f);
	//clip(cColor.a - 0.5f);
	return(cColor);
}

//============================================================================================


