#include "Shaders.hlsl"

struct VS_FLOOR_INPUT
{
	float3 position :  POSITION;
	float2 tex : TEXCOORD;
};

struct PS_FLOOR_INPUT
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 positionW : POSITION;
	float4 posj : TEXCOORD2;

	float4 LightViewPosition : TEXCOORD3;
	float4 LightPosition : TEXCOORD4;

	float3 tangent : TANGENT;
	float3 normal : NORMAL;
	float3 bitangent : BITANGENT;
};

static matrix gmtxProjectToTexture =
{ 0.5f, 0.0f, 0.0f, 0.0f,
  0.0f,-0.5f, 0.0f, 0.0f,
  0.0f, 0.0f, 1.0f, 0.0f,
  0.5f, 0.5f, 0.0f, 1.0f };


PS_FLOOR_INPUT FloorVS(VS_FLOOR_INPUT input)
{
	PS_FLOOR_INPUT output;
	float4 pos = mul(float4(input.position, 1.0f), gmtxGameObject);
	output.positionW = pos.xyz;
	output.position = mul(mul(pos, gmtxView), gmtxProjection);
	output.tex = input.tex * 20;

	output.normal = mul(float4(0.0f, 1.0f, 0.0f,1.f),gmtxGameObject).xyz;
	output.tangent = float3 (0.0f, 0.0f, 1.0f);
	output.bitangent = float3 (1.0f, 0.0f, 0.0f);

	//shadow
	matrix shadowProject = mul(mul(shadowgmtxView, shadowgmtxProjection), gmtxProjectToTexture);
	matrix shadowLightPosition = mul(shadowgmtxView, shadowgmtxProjection);

	output.LightViewPosition = mul(float4(output.positionW, 1.0f), shadowProject);
	output.LightPosition = mul(float4(output.positionW, 1.0f), shadowLightPosition);

	output.posj = output.position;
	return output;
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT FloorPS(PS_FLOOR_INPUT input) : SV_TARGET
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	output.color = gtxtAlbedoTexture.Sample(gSamplerState, input.tex);
	output.depth = float4(input.posj.z / input.posj.w, input.posj.w / (gtx_Buffer_width / 2.0f), 0, 1);
	float3x3 TBN;
	if (input.normal.y > 0)
		TBN = float3x3(float3(1, 0, 0), float3(0, 0, 1), float3(0, 1, 0));
	else
		TBN = float3x3(float3(-1, 0, 0), float3(0, 0, -1), float3(0, -1, 0));

	float4 cColorNormal = gtxtNormalTexture.Sample(gSamplerState, input.tex);
	float3 vNormal = normalize(cColorNormal.rgb * 2.0f - 1.0f);
	vNormal = normalize(mul(vNormal, TBN));
	output.normal =float4(vNormal / 2.f + 0.5f, 1);

	//[±×¸²ÀÚ]============================================================================
	float2 projectTexCoord;
	float lightDepthValue;
	float depthValue;
	float bias = 0.00001f;

	input.LightViewPosition.xyz /= input.LightViewPosition.w;

	depthValue = gtxtShadowCameraTexture.Sample(gSamplerClamp, input.LightViewPosition.xy).r;

	if (input.LightViewPosition.z > (depthValue + bias))
	{
		output.color = float4 (output.color.rgb * 0.6, 1.0f);
	}

	return output;
}



PS_FLOOR_INPUT FloorShadowVS(VS_FLOOR_INPUT input)
{
	PS_FLOOR_INPUT output;
	float4 pos = mul(float4(input.position, 1.0f), gmtxGameObject);
	output.positionW = pos.xyz;
	output.position = mul(mul(pos, shadowgmtxView), shadowgmtxProjection);
	output.tex = input.tex;

	output.normal = float3(0.0f, 0.5f, 0.0f);
	output.tangent = float3 (0.0f, 0.0f, 1.0f);
	output.bitangent = float3 (1.0f, 0.0f, 0.0f);
	output.posj = output.position;
	return output;
}

PS_SHADOW_OUTPUT FloorShadowPS(PS_FLOOR_INPUT input)
{
	PS_SHADOW_OUTPUT output;

	output.ShadowTex = float4(input.posj.z / input.posj.w, input.posj.w / (gtx_Buffer_width / 2.0f), 0.0f, 1.0f);
	//output.ShadowTex = float4(input.position.z  , 0, 0.0f, 1.0f);
	return output;
}

