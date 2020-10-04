#include "Shaders.hlsl"
struct VS_TEXTURED_LIGHTING_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
};

struct VS_TEXTURED_LIGHTING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float3 tangentW : TANGENT;
	float3 bitangentW : BITANGENT;
	float2 uv : TEXCOORD;
	float4 vPorjPos : TEXCOORD1; 
	float4 LightViewPosition : TEXCOORD2;
	float3 LightPos : TEXCOORD3;
};

struct VS_SKINNED_STANDARD_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	uint4 indices : BONEINDEX;
	float4 weights : BONEWEIGHT;
};

#define MAX_VERTEX_INFLUENCES			4
#define SKINNED_ANIMATION_BONES			128

cbuffer cbBoneOffsets : register(b7)
{
	float4x4 gpmtxBoneOffsets[SKINNED_ANIMATION_BONES];
};

cbuffer cbBoneTransforms : register(b8)
{
	float4x4 gpmtxBoneTransforms[SKINNED_ANIMATION_BONES];
};

VS_TEXTURED_LIGHTING_OUTPUT VSSkinnedAnimationStandard(VS_SKINNED_STANDARD_INPUT input)
{
	VS_TEXTURED_LIGHTING_OUTPUT output;

	output.positionW = float3(0.0f, 0.0f, 0.0f);
	output.normalW = float3(0.0f, 0.0f, 0.0f);
	output.tangentW = float3(0.0f, 0.0f, 0.0f);
	output.bitangentW = float3(0.0f, 0.0f, 0.0f);
	float4x4 mtxVertexToBoneWorld = (float4x4)0.0f;
	for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
	{
		//		mtxVertexToBoneWorld += input.weights[i] * gpmtxBoneTransforms[input.indices[i]];
		mtxVertexToBoneWorld +=  input.weights[i] * mul(gpmtxBoneOffsets[input.indices[i]], gpmtxBoneTransforms[input.indices[i]]);
	}
	output.positionW = mul(float4(input.position, 1.0f), mtxVertexToBoneWorld).xyz;
	output.normalW = mul(input.normal, (float3x3)mtxVertexToBoneWorld).xyz;
	output.tangentW = mul(input.tangent, (float3x3)mtxVertexToBoneWorld).xyz;
	output.bitangentW = mul(input.bitangent, (float3x3)mtxVertexToBoneWorld).xyz;
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;
	output.vPorjPos = output.position;
	
	//shadow 
	output.LightViewPosition = mul(mul(float4(output.positionW, 1.0f), shadowgmtxView), shadowgmtxProjection);
	//output.LightPos = 
	return(output);
}

VS_TEXTURED_LIGHTING_OUTPUT VSLighting(VS_TEXTURED_LIGHTING_INPUT input)
{
	VS_TEXTURED_LIGHTING_OUTPUT output;
	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.tangentW = mul(input.tangent, (float3x3)gmtxGameObject);
	output.bitangentW = mul(input.bitangent, (float3x3)gmtxGameObject);
	output.uv = input.uv;
	output.vPorjPos = output.position;
	output.LightViewPosition = mul(mul(float4(output.positionW, 1.0f), shadowgmtxView), shadowgmtxProjection);
	return output;
}



/////////////////////////////////////////////////////////////////////////////////////////////



PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTexturedLightingToMultipleRTs(VS_TEXTURED_LIGHTING_OUTPUT input)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;


	float4 cColorAlbedo = gtxtAlbedoTexture.Sample(gSamplerState, input.uv);
	output.color = cColorAlbedo;

	float4 cColorNormal = gtxtNormalTexture.Sample(gSamplerState, input.uv);

	float3 normalW;
	float3x3 TBN = float3x3(normalize(input.tangentW), normalize(input.bitangentW), normalize(input.normalW));

	float3 vNormal = normalize(cColorNormal.rgb * 2.0f - 1.0f); //[0, 1] ¡æ [-1, 1]

	normalW = normalize(mul(vNormal, TBN));
	output.normal = float4(normalW /2.f + 0.5f, 1);

	output.depth = float4(input.vPorjPos.z/ input.vPorjPos.w, input.vPorjPos.w / (gtx_Buffer_width / 2.0f),0, 1);
	output.ShadowCamera = float4 (1.0f, 0.0f, 0.0f, 1.0f);
	output.EmmisiveMRT = float4(0, 0, 0, 0);


	if (gnTextureMask & MATERIAL_EMISSION_MAP)
	{
		float4 cColorEmission = gtxtEmissionTexture.Sample(gSamplerState, input.uv);
		output.EmmisiveMRT = cColorEmission;
	}

	if(gnObjectID == 1) output.color.x = 1;
	return output;
}


PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTransparent(VS_TEXTURED_LIGHTING_OUTPUT input)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	output.color = float4(1.f, 1.f, 1.f, 0.1f);
	output.normal = float4(input.normalW, 0.2f);

	output.depth = float4(input.vPorjPos.z / input.vPorjPos.w, input.vPorjPos.w / (gtx_Buffer_width / 2.0f), 0, 1);
	output.ShadowCamera = float4 (1.0f, 0.0f, 0.0f, 1.0f);
	
	float fresnelFator = dot(-gvCameraNoraml,input.normalW);
	fresnelFator = pow(max(fresnelFator, 0.0f), 1.5f) / 7;
	output.EmmisiveMRT = float4(0.8f,0.8f, 1, 1 - fresnelFator);
	return output;
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT IndoorPSTransparent(VS_TEXTURED_LIGHTING_OUTPUT input)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	output.color = float4(1.f, 1.f, 1.f, 0.1f);
	output.normal = float4(input.normalW, 0.2f);

	output.depth = float4(input.vPorjPos.z / input.vPorjPos.w, input.vPorjPos.w / (gtx_Buffer_width / 2.0f), 0, 1);
	output.ShadowCamera = float4 (1.0f, 0.0f, 0.0f, 1.0f);

	float fresnelFator = dot(-gvCameraNoraml, input.normalW);
	fresnelFator = pow(max(fresnelFator, -1.0f), 1.5f) / 7;
	output.EmmisiveMRT = float4(0.5f, 0.5f, 0.8, 1 - fresnelFator);
	return output;
}


PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSDissolveForSkinned(VS_TEXTURED_LIGHTING_OUTPUT input)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	float fDissolveTime = frameTime;
	float4 cColorAlbedo = gtxtAlbedoTexture.Sample(gSamplerState, input.uv);
	

	float4 cColorNormal = gtxtNormalTexture.Sample(gSamplerState, input.uv);
	float3 normalW;
	float3x3 TBN = float3x3(normalize(input.tangentW), normalize(input.bitangentW), normalize(input.normalW));

	float3 vNormal = normalize(cColorNormal.rgb * 2.0f - 1.0f); //[0, 1] ¡æ [-1, 1]

	normalW = normalize(mul(vNormal, TBN));
	output.normal = float4(normalW / 2.f + 0.5f, 1);

	float cColorDissolve = gtxtDissolveTexture.Sample(gSamplerState, input.uv);
	if (cColorDissolve <= fDissolveTime) {

		cColorAlbedo = float4(1,1,1,1);
		output.normal = float4(1, 1, 1, 1);
	}
	else if (cColorDissolve <= fDissolveTime + 0.05f) {

		cColorAlbedo = float4(0, 1, 1, 1);
		output.normal = float4(1, 1, 1, 1);
	}

	output.color = cColorAlbedo;


	output.depth = float4(input.vPorjPos.z / input.vPorjPos.w, input.vPorjPos.w / (gtx_Buffer_width / 2.0f), 0, 1);
	output.ShadowCamera = float4 (1.0f, 0.0f, 0.0f, 1.0f);
	output.EmmisiveMRT = float4(0, 0, 0, 0);

	if (gnTextureMask & MATERIAL_EMISSION_MAP)
	{
		float4 cColorEmission = gtxtEmissionTexture.Sample(gSamplerState, input.uv);
		output.EmmisiveMRT = cColorEmission;
	}

	return output;
}

// ================================================================================================
VS_TEXTURED_LIGHTING_OUTPUT VSTexcoord(VS_TEXTURED_LIGHTING_INPUT input)
{
	VS_TEXTURED_LIGHTING_OUTPUT output;
	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.tangentW = mul(input.tangent, (float3x3)gmtxGameObject);
	output.bitangentW = mul(input.bitangent, (float3x3)gmtxGameObject);
	output.uv = float2(input.uv.x * 30.0f, input.uv.y);
	output.vPorjPos = output.position;
	output.LightViewPosition = mul(mul(float4(output.positionW, 1.0f), shadowgmtxView), shadowgmtxProjection);
	return output;
}


PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTexCoordToMultipleRTs(VS_TEXTURED_LIGHTING_OUTPUT input)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;


	float4 cColorAlbedo = gtxtAlbedoTexture.Sample(gSamplerState, input.uv);
	output.color = cColorAlbedo;

	float4 cColorNormal = gtxtNormalTexture.Sample(gSamplerState, input.uv);

	float3 normalW;
	float3x3 TBN = float3x3(normalize(input.tangentW), normalize(input.bitangentW), normalize(input.normalW));

	float3 vNormal = normalize(cColorNormal.rgb * 2.0f - 1.0f); //[0, 1] ¡æ [-1, 1]

	normalW = normalize(mul(vNormal, TBN));

	output.normal = float4(input.normalW / 2.0f + 0.5f , 1);

	output.depth = float4(input.vPorjPos.z / input.vPorjPos.w, input.vPorjPos.w / (gtx_Buffer_width / 2.0f), 0, 1);
	output.ShadowCamera = float4 (1.0f, 0.0f, 0.0f, 1.0f);
	output.EmmisiveMRT = float4(0, 0, 0, 0);

	if (gnTextureMask & MATERIAL_EMISSION_MAP)
	{
		float4 cColorEmission = gtxtEmissionTexture.Sample(gSamplerState, input.uv);
		output.EmmisiveMRT = cColorEmission;
	}

	return output;
}


// [alphaChannelShader]
PS_MULTIPLE_RENDER_TARGETS_OUTPUT AlphaChannelForSkinned(VS_TEXTURED_LIGHTING_OUTPUT input)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	float fDissolveTime = frameTime;
	float4 cColorAlbedo = gtxtAlbedoTexture.Sample(gSamplerState, input.uv);
	float AlphaValue = cColorAlbedo.a; 

	if (AlphaValue < 0.5)
		discard;

	float4 cColorNormal = gtxtNormalTexture.Sample(gSamplerState, input.uv);
	float3 normalW;
	float3x3 TBN = float3x3(normalize(input.tangentW), normalize(input.bitangentW), normalize(input.normalW));

	float3 vNormal = normalize(cColorNormal.rgb * 2.0f - 1.0f); //[0, 1] ¡æ [-1, 1]

	normalW = normalize(mul(vNormal, TBN));
	output.normal = float4(normalW / 2.f + 0.5f, AlphaValue);

	output.color = cColorAlbedo;

	output.depth = float4(input.vPorjPos.z / input.vPorjPos.w, input.vPorjPos.w / (gtx_Buffer_width / 2.0f), 0, 1);
	output.ShadowCamera = float4 (1.0f, 0.0f, 0.0f, 1.0f);
	output.EmmisiveMRT = float4(0, 0, 0, 0);


	return output;
}


// [alphaChannelShader]
PS_MULTIPLE_RENDER_TARGETS_OUTPUT AlphaChannelDissolveForSkinned(VS_TEXTURED_LIGHTING_OUTPUT input)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	float fDissolveTime = frameTime;
	float4 cColorAlbedo = gtxtAlbedoTexture.Sample(gSamplerState, input.uv);
	float AlphaValue = cColorAlbedo.a;

	if (AlphaValue < 0.5)
		discard;

	float4 cColorNormal = gtxtNormalTexture.Sample(gSamplerState, input.uv);
	float3 normalW;
	float3x3 TBN = float3x3(normalize(input.tangentW), normalize(input.bitangentW), normalize(input.normalW));

	float3 vNormal = normalize(cColorNormal.rgb * 2.0f - 1.0f); //[0, 1] ¡æ [-1, 1]

	normalW = normalize(mul(vNormal, TBN));
	output.normal = float4(normalW / 2.f + 0.5f, AlphaValue);

	float cColorDissolve = gtxtDissolveTexture.Sample(gSamplerState, input.uv);
	if (cColorDissolve <= fDissolveTime) {

		cColorAlbedo = float4(1, 1, 1, AlphaValue);
		output.normal = float4(1, 1, 1, AlphaValue);
	}
	else if (cColorDissolve <= fDissolveTime + 0.05f) {

		cColorAlbedo = float4(0, 1, 1, AlphaValue);
		output.normal = float4(1, 1, 1, AlphaValue);
	}

	if (AlphaValue > 0.5)
	{
		output.color = float4(cColorAlbedo.rgb, AlphaValue);

	}

	output.depth = float4(input.vPorjPos.z / input.vPorjPos.w, input.vPorjPos.w / (gtx_Buffer_width / 2.0f), 0, 1);
	output.ShadowCamera = float4 (1.0f, 0.0f, 0.0f, 1.0f);
	output.EmmisiveMRT = float4(0, 0, 0, 0);

	return output;
}

VS_TEXTURED_LIGHTING_OUTPUT TreeVSTexcoord(VS_TEXTURED_LIGHTING_INPUT input)
{
	VS_TEXTURED_LIGHTING_OUTPUT output;
	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.tangentW = mul(input.tangent, (float3x3)gmtxGameObject);
	output.bitangentW = mul(input.bitangent, (float3x3)gmtxGameObject);
	output.uv = input.uv * 6;
	output.vPorjPos = output.position;
	output.LightViewPosition = mul(mul(float4(output.positionW, 1.0f), shadowgmtxView), shadowgmtxProjection);
	return output;
}


PS_EMMISIVE_MRT_OUTPUT PSSwordEffectShader(VS_TEXTURED_LIGHTING_OUTPUT input)
{
	PS_EMMISIVE_MRT_OUTPUT output;

	float2 alphaUV = input.uv;
	alphaUV.y += gnSwordEffectTime;

	float4 cColorEmission = gtxtAlbedoTexture.Sample(gSamplerClamp, alphaUV);

	float4 SwordEffectTexture = gtxtRealEffectTexture.Sample(gSamplerState, alphaUV);

	float3 mulColor = mul((float3)cColorEmission, (float3)SwordEffectTexture);

	float alphaControl = cColorEmission.x;
	alphaControl -= gnSwordEffectTime;

	output.EmmisiveMRT = float4(0.0f, mulColor.y, mulColor.z, cColorEmission.x);

	return output;
}