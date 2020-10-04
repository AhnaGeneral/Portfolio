#include "HLSL_Standard.hlsl"

struct VS_STANDARD_SHADOWOUTPUT
{
	float3 positionW : POSITION;
	float4 position : SV_POSITION;
	float4 posj : TEXCOORD0;
};

VS_STANDARD_SHADOWOUTPUT VSStandardShadow(VS_TEXTURED_LIGHTING_INPUT input)
{
	VS_STANDARD_SHADOWOUTPUT output;
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), shadowgmtxView), shadowgmtxProjection);
	output.posj = output.position;
	return output;
}

VS_STANDARD_SHADOWOUTPUT VSSkinnedShadow(VS_SKINNED_STANDARD_INPUT input)
{
	VS_STANDARD_SHADOWOUTPUT output;

	output.positionW = float3(0.0f, 0.0f, 0.0f);
	matrix mtxVertexToBoneWorld;
	for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
	{
		mtxVertexToBoneWorld = mul(gpmtxBoneOffsets[input.indices[i]], gpmtxBoneTransforms[input.indices[i]]);
		output.positionW += input.weights[i] * mul(float4(input.position, 1.0f), mtxVertexToBoneWorld).xyz;
	}

	output.position = mul(mul(float4(output.positionW, 1.0f), shadowgmtxView), shadowgmtxProjection);
	output.posj = output.position;
	return(output);
}

PS_SHADOW_OUTPUT PSStandardShadow(VS_STANDARD_SHADOWOUTPUT input)
{
	PS_SHADOW_OUTPUT output;

	output.ShadowTex = float4(input.posj.z / input.posj.w, input.posj.w / (gtx_Buffer_width/2.0f), 0.0f, 1.0f);

	return output;
}
