#include "Shaders.hlsl"

 //=====================================================================================

struct VS_BILLBOARD_INPUT
{
	float3 positionW : POSITION; 
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

	output.centerW = input.positionW;
	output.sizeW = input.sizeW;
	return(output);
}

[maxvertexcount(4)]
void GS(point VS_BILLBOARD_OUTPUT input[1], 
	inout TriangleStream<GS_BILLBOARD_OUTPUT> outStream)
{
	float3 vUP = float3(0.0f, 1.0f, 0.0f);
	float3 vLook = gvCameraPosition.xyz - input[0].centerW;
	vLook = normalize(vLook);
	float3 vRight = cross(vUP, vLook);

	float fHalfW = input[0].sizeW.x * 0.5f;
	float fHalfH = input[0].sizeW.y * 0.5f;

	float4 pVertices[4];
	pVertices[0] = float4(input[0].centerW + fHalfW * vRight - fHalfH * vUP, 1.0f);
	pVertices[1] = float4(input[0].centerW + fHalfW * vRight + fHalfH * vUP, 1.0f);
	pVertices[2] = float4(input[0].centerW - fHalfW * vRight - fHalfH * vUP, 1.0f);
	pVertices[3] = float4(input[0].centerW - fHalfW * vRight + fHalfH * vUP, 1.0f);

	float2 pUVs[4] = { float2 (0.0f, 1.0f), float2(0.0f, 0.0f), 
		float2(1.0f, 1.0f), float2(1.0f, 0.0f) };

	GS_BILLBOARD_OUTPUT output;

	for (int i = 0; i < 4; ++i)
	{
		output.posW = pVertices[i].xyz;
		output.posH = mul(mul(pVertices[i], gmtxView), gmtxProjection);
		output.UV = pUVs[i];
		outStream.Append(output);
	}
}

float4 PSBillboard(GS_BILLBOARD_OUTPUT input) : SV_TARGET
{
	float3 uvw = float3 (input.UV,0.0f);
	float4 cColor = gtxCloudTextures.Sample(gSamplerState, uvw);
	clip(cColor.a - 0.1f);
	return(cColor);
}