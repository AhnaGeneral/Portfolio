
#include "Shaders.hlsl"

struct VS_TERRAIN_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float3 normal : NORMAL;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

struct VS_TERRAIN_OUTPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float3 normal : NORMAL;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
	float4 posj : TEXCOORD2;
};

VS_TERRAIN_OUTPUT VSTerrain(VS_TERRAIN_INPUT input)
{
	VS_TERRAIN_OUTPUT output;

	output.color = input.color;
	output.position = input.position;
	output.normal = input.normal; 

	output.uv0 = input.uv0;
	output.uv1 = input.uv1;

	output.posj = float4 (input.position, 1.0f);
	return(output);
}

struct HS_TERRAIN_TESSELLATION_CONSTANT
{
	float fTessEdges[4] : SV_TessFactor;
	float fTessInsides[2] : SV_InsideTessFactor;
};

struct HS_TERRAIN_TESSELLATION_OUTPUT
{
	float3 position : POSITION;
	float4 color : COLOR;

	float3 normal : NORMAL;

	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
	float4 posj : TEXCOORD2;
};

struct DS_TERRAIN_TESSELLATION_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float4 color : COLOR;
	float3 normal : NORMAL;
	float3 tangent : TANGET;
	float3 bitanget : BITANGENT;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
	float4 posj : TEXCOORD2;
	float4 LightViewPosition : TEXCOORD3;
	float4 LightPosition : TEXCOORD4; 
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

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(25)]
[patchconstantfunc("VSTerrainTessellationConstant")]
[maxtessfactor(64.0f)]
HS_TERRAIN_TESSELLATION_OUTPUT HSTerrainTessellation
(InputPatch<VS_TERRAIN_OUTPUT, 25> input,
	uint i : SV_OutputControlPointID)
{
	HS_TERRAIN_TESSELLATION_OUTPUT output;

	output.position = input[i].position;
	output.color = input[i].color;
	output.uv0 = input[i].uv0;
	output.uv1 = input[i].uv1;
	output.normal = input[i].normal; 
	output.posj = input[i].posj;

	return(output);
}

HS_TERRAIN_TESSELLATION_CONSTANT VSTerrainTessellationConstant(InputPatch<VS_TERRAIN_OUTPUT, 25> input)
{

	float fTessFactor = 3;
	if (gf3RadiationLevel != 0)
	{
		float3 vCenter;
		for (int i = 0; i < 25; i++)
			vCenter += input[i].position;
		vCenter = vCenter / 25.f;
	
		float fDistanceToCamera = distance(vCenter, gvCameraPosition);
		fTessFactor = round(fDistanceToCamera / 100);
		switch (fTessFactor)
		{
		case 0:
		case 1:
			fTessFactor = 60;
			break;
		case 2:
			fTessFactor = 20;
			break;
		case 3:
			fTessFactor = 10;
			break;
		case 4:
		case 5:
			fTessFactor = 2;
			break;

		}

		//if (fDistanceToCamera > 100.f)
		//{
		//	fTessFactor = 1000.f / fDistanceToCamera;
		//	if(fDistanceToCamera > 200.f)
		//		fTessFactor = 1000.f / fDistanceToCamera;
		//}

	}
	HS_TERRAIN_TESSELLATION_CONSTANT output;

	output.fTessEdges[0] = fTessFactor;
	output.fTessEdges[1] = fTessFactor;
	output.fTessEdges[2] = fTessFactor;
	output.fTessEdges[3] = fTessFactor;
	output.fTessInsides[0] = fTessFactor;
	output.fTessInsides[1] = fTessFactor;

	// 일정한 격자로 보고 싶을 때 
	//output.fTessEdges[0] = 3.0f;
	//output.fTessEdges[1] = 3.0f;
	//output.fTessEdges[2] = 3.0f;
	//output.fTessEdges[3] = 3.0f;
	//output.fTessInsides[0] = 1.0f;
	//output.fTessInsides[1] = 1.0f;

	return(output);
}

static matrix gmtxProjectToTexture =
{ 0.5f, 0.0f, 0.0f, 0.0f,
  0.0f,-0.5f, 0.0f, 0.0f,
  0.0f, 0.0f, 1.0f, 0.0f,
  0.5f, 0.5f, 0.0f, 1.0f };


Texture2D gtxtSplatAlpha   : register(t50);
Texture2D gtxtTerrain1_NM  : register(t51);
Texture2D gtxtSand2_BC     : register(t52);
Texture2D gtxtSand_NM	   : register(t53);
Texture2D gtxtDryStone_BC  : register(t54);
Texture2D gtxtDryStone_NM  : register(t55);
Texture2D gtxtGrass1_BC    : register(t56);
Texture2D gtxtGrass2_BC    : register(t57);
Texture2D gtxtSand1        : register(t58);
Texture2D gtxtGrass1_NM    : register(t59);
Texture2D gtxtDryStone_HT  : register(t60);
Texture2D gtxtSand_HT	   : register(t61);

[domain("quad")]
[earlydepthstencil]
DS_TERRAIN_TESSELLATION_OUTPUT DSTerrainTessellation
(HS_TERRAIN_TESSELLATION_CONSTANT patchConstant, 
	float2 uv : SV_DomainLocation,
	OutputPatch<HS_TERRAIN_TESSELLATION_OUTPUT, 25> patch)
{
	DS_TERRAIN_TESSELLATION_OUTPUT output =
		(DS_TERRAIN_TESSELLATION_OUTPUT)0;

	float uB[5], vB[5];
	BernsteinCoeffcient5x5(uv.x, uB);
	BernsteinCoeffcient5x5(uv.y, vB);

	output.color = float4(0,0,0,1);
	output.uv0 = lerp(lerp(patch[0].uv0, patch[4].uv0, uv.x),
		lerp(patch[20].uv0, patch[24].uv0, uv.x), uv.y);
	output.uv1 = lerp(lerp(patch[0].uv1, patch[4].uv1, uv.x),
		lerp(patch[20].uv1, patch[24].uv1, uv.x), uv.y);
	float3 worldnormal = lerp(lerp(patch[0].normal, 
		patch[4].normal, uv.x), lerp(patch[20].normal,
			patch[24].normal, uv.x), uv.y);
	
	///float3 tangentuv = float3(output.uv0, 0.0f); //텍스처의 UV값을  

    float3 worldtanget = float3(1.0f, 1.0f, 1.0f);
    float3 worldbitanget = float3(1.0f, 1.0f, 1.0f);

    float3 tmpnormal = mul(worldnormal, (float3x3)gmtxGameObject);
	output.normal = worldnormal; // normalize(tmpnormal) ;

    float3 tmptanget = cross(tmpnormal, float3(output.uv0.x, 0.0f, 0.0f));
    worldtanget = mul(tmptanget, (float3x3)gmtxGameObject);
    output.tangent = normalize(worldtanget);
    
    float3 tmpbitanget = cross(tmpnormal, tmptanget);
    worldbitanget = mul(tmpbitanget, (float3x3)gmtxGameObject);
    output.bitanget = worldbitanget;

	float3 position = CubicBezierSum5x5(patch, uB, vB);
	matrix mtxWorldViewProjection = mul(mul(gmtxGameObject, gmtxView), gmtxProjection);
	matrix mtxShadowViewProjection = mul(mul(gmtxGameObject, shadowgmtxView), shadowgmtxProjection);

	
	if (gf3RadiationLevel != 0)
	{
		float4 Tex_SplatAlpha = gtxtSplatAlpha.SampleLevel(gSamplerState, output.uv0,10);
	
		float fHeight = Tex_SplatAlpha.r * (gtxtDryStone_HT.SampleLevel(gSamplerState, output.uv1 ,0).r * 1.5f -0.1f) 
					  + Tex_SplatAlpha.g *  gtxtSand_HT.SampleLevel(gSamplerState, output.uv1, 0).r
					  + Tex_SplatAlpha.a * gtxtSand_HT.SampleLevel(gSamplerState, output.uv1 , 0).r
					  + Tex_SplatAlpha.b * 0.2f;
		position += output.normal * (fHeight * 0.35f) ;
		output.color.xyz = fHeight ;
	}

	output.positionW = mul(float4(position, 1.0f), gmtxGameObject);
	output.position = mul(float4(position, 1.0f), mtxWorldViewProjection);


	//shadow
	matrix shadowProject = mul(mul(shadowgmtxView, shadowgmtxProjection), gmtxProjectToTexture);
	matrix shadowLightPosition = mul(shadowgmtxView, shadowgmtxProjection);

	output.LightViewPosition = mul(float4(output.positionW, 1.0f), shadowProject);
	output.LightPosition = mul(float4(output.positionW, 1.0f), shadowLightPosition); 

	output.posj = output.position ;

	return(output);
}


PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTerrain(DS_TERRAIN_TESSELLATION_OUTPUT input) : SV_TARGET
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output; 

	float3x3 TBN = float3x3(input.tangent, input.bitanget, input.normal);

	float4 Tex_SplatAlpha = gtxtSplatAlpha.Sample(gSamplerState, input.uv0);

	float4 Tex_Sand2_BC = gtxtSand2_BC.Sample(gSamplerState, input.uv1);
	float4 Tex_Sand_NM = gtxtSand_NM.Sample(gSamplerState, input.uv1);

	float4 Tex_DryStone_BC = gtxtDryStone_BC.Sample(gSamplerState, input.uv1);
	float4 Tex_DryStone_NM = gtxtDryStone_NM.Sample(gSamplerState, input.uv1);

	float4 Tex_Grass1_BC = gtxtGrass1_BC.Sample(gSamplerState, input.uv1);
	float4 Tex_Grass2_BC = gtxtGrass2_BC.Sample(gSamplerState, input.uv1);
	float4 Tex_Grass1_NM = gtxtGrass1_NM.Sample(gSamplerState, input.uv1);
	
	float4 Tex_Sand1 = gtxtSand1.Sample(gSamplerState, input.uv1);

	float3 vNormal = input.normal; 

	float3 vNormal_DrySand = normalize(Tex_Sand_NM.rgb * 2.0f - 1.0f);
	float3 vNormal_DryStone  = normalize(Tex_DryStone_NM.rgb * 2.0f - 1.0f);
	float3 vNormal_Grass1 = normalize(Tex_Grass1_NM.rgb * 2.0f - 1.0f);
	
	float3 toCamera = normalize(gvCameraPosition - input.positionW);

	vNormal_DrySand = normalize(mul(vNormal_DrySand, TBN));
	vNormal_DryStone = normalize(mul(vNormal_DryStone, TBN));
	vNormal_Grass1 = normalize(mul(vNormal_Grass1, TBN));

	if (gf3RadiationLevel == 2)
	{
		vNormal = (Tex_SplatAlpha.r * vNormal_DryStone) + (Tex_SplatAlpha.g * vNormal_DrySand) + (Tex_SplatAlpha.a * vNormal_DrySand)
			+ (Tex_SplatAlpha.b * vNormal_Grass1);

	}
	vNormal = vNormal; // 조명 영향이 너무 가서..
	float4 Splat = (Tex_SplatAlpha.r * Tex_DryStone_BC) + (Tex_SplatAlpha.g * Tex_Sand1) +
		    (Tex_SplatAlpha.b * Tex_Grass1_BC) +(Tex_SplatAlpha.a * Tex_Sand2_BC);

	//===================================================================================
	if (input.color.r > 0.f)
		output.color = Splat + (input.color / 3.f - 0.13f);
	else
		output.color = Splat;

	output.normal = float4(vNormal/2.f +0.5f ,1.0f);
	output.depth = float4(input.posj.z/input.posj.w, input.posj.w/ (gtx_Buffer_width/2.0f), 0, 1);

	output.ShadowCamera = input.posj;

	//[그림자]============================================================================
	float2 projectTexCoord;
	float lightDepthValue;
	float depthValue;
	float bias = 0.00001f;

	input.LightViewPosition.xyz /= input.LightViewPosition.w; 

	depthValue = gtxtShadowCameraTexture.Sample(gSamplerClamp, input.LightViewPosition.xy).r;

	if (input.LightViewPosition.z > (depthValue + bias))
	{
		output.color = float4 (output.color.rgb * 0.4f, 1.0f) ;
	}

	//[바다와 블렌딩]==========================================================================
	if (input.positionW.y < 30.f)
	{
		float fAlpha = (30.f - input.positionW.y) / 10.f;
		output.color.w -= fAlpha;
		output.depth.w == fAlpha;
		output.normal.w -= fAlpha;
	}

	input.LightPosition.xyz /= input.LightPosition.w;
	output.ShadowCamera = input.LightPosition;

	return output;
}


