#include "Shaders.hlsl"

cbuffer cbEffectCompoment : register(b12)
{
	int row : packoffset(c0);
	int col : packoffset(c0.y);
	float fime : packoffset(c0.z);
	float fLifetime : packoffset(c0.w);
}

struct VS_EFFECT_INPUT
{
	float3 position : POSITION;
};

struct GS_EFFECT_OUTPUT
{
	float4 posH :SV_POSITION;
	float3 posW :POSITION;
	float2 UV : TEXCOORD0;
};

struct GS_EFFECT_INPUT
{
	float3 positionW : POSITION;
};

GS_EFFECT_INPUT EffectVertexShader(VS_EFFECT_INPUT input)
{
	GS_EFFECT_INPUT output;

	output.positionW = input.position;

	return output;
}


[maxvertexcount(4)]
void EffectGS(point GS_EFFECT_INPUT input[1], inout TriangleStream<GS_EFFECT_OUTPUT> outStream)
{
	float upX = cos(fLifetime);
	float upY = sin(fLifetime);

	float3 vUP = normalize(float3(0, upX, upY));
	float3 vLook = gvCameraPosition.xyz - input[0].positionW;
	vLook = normalize(vLook);
	float3 vRight = cross(vUP, vLook);

	float fHalfW = 20 * 0.5f;
	float fHalfH = 20 * 0.5f;

	float4 pVertices[4];
	pVertices[0] = float4(input[0].positionW + fHalfW * vRight - fHalfH * vUP, 1.0f);
	pVertices[1] = float4(input[0].positionW + fHalfW * vRight + fHalfH * vUP, 1.0f);
	pVertices[2] = float4(input[0].positionW - fHalfW * vRight - fHalfH * vUP, 1.0f);
	pVertices[3] = float4(input[0].positionW - fHalfW * vRight + fHalfH * vUP, 1.0f);

	float2 pUVs[4] = { float2 (0.0f, 1.0f), float2(0.0f, 0.0f), float2(1.0f, 1.0f), float2(1.0f, 0.0f) };

	GS_EFFECT_OUTPUT output;

	for (int i = 0; i < 4; ++i)
	{
		output.posW = pVertices[i].xyz;
		output.posH = mul(mul(pVertices[i], gmtxView), gmtxProjection); 
		output.UV = pUVs[i]; 
		outStream.Append(output);
	}
}

PS_EMMISIVE_MRT_OUTPUT EffectPixelShader(GS_EFFECT_OUTPUT input)
{

	PS_EMMISIVE_MRT_OUTPUT output;

	float3 uvw = float3 (input.UV, 0.0f);
	float4 cColor = gtxtRealEffectTexture.Sample(gSamplerState, uvw);
	output.EmmisiveMRT = cColor;
	//output.NonLight = float4(1,1,1,1);
	return(output);
}

[maxvertexcount(4)] //±âÇÏ½¦ÀÌ´õ ´¯È÷±â ...
void EffectSPTGS(point GS_EFFECT_INPUT input[1], inout TriangleStream<GS_EFFECT_OUTPUT> outStream)
{
	float3 vUP = float3(0.0f, 1.0f, 0.0f);
	float3 vLook = gvCameraPosition.xyz - input[0].positionW;
	vLook = normalize(vLook);
	float3 vRight = cross(vUP, vLook);

	float fHalfW = 50 * 0.5f;
	float fHalfH = 50 * 0.5f;

	float4 pVertices[4];
	pVertices[0] = float4(input[0].positionW.x + fHalfW, input[0].positionW.y, input[0].positionW.z + fHalfW ,1.0f);
	pVertices[1] = float4(input[0].positionW.x + fHalfW, input[0].positionW.y, input[0].positionW.z - fHalfW, 1.0f);
	pVertices[2] = float4(input[0].positionW.x - fHalfW, input[0].positionW.y, input[0].positionW.z + fHalfW, 1.0f);
	pVertices[3] = float4(input[0].positionW.x - fHalfW, input[0].positionW.y, input[0].positionW.z - fHalfW, 1.0f);

	float2 pUVs[4] = { float2 (0.0f, 1.0f), float2(0.0f, 0.0f), float2(1.0f, 1.0f), float2(1.0f, 0.0f) };

	GS_EFFECT_OUTPUT output;

	for (int i = 0; i < 4; ++i)
	{
		output.posW = pVertices[i].xyz;
		output.posH = mul(mul(pVertices[i], gmtxView), gmtxProjection);
		output.UV = pUVs[i];
		outStream.Append(output);
	}
}

[maxvertexcount(4)] //±âÇÏ½¦ÀÌ´õ ¼¼¿ì±â 
void EffectSPTYZGS(point GS_EFFECT_INPUT input[1], inout TriangleStream<GS_EFFECT_OUTPUT> outStream)
{
	float3 vUP = float3(0.0f, 1.0f, 0.0f);
	float3 vLook = gvCameraPosition.xyz - input[0].positionW;
	vLook = normalize(vLook);
	float3 vRight = cross(vUP, vLook);

	float fHalfW = 10 * 0.5f;
	float fHalfH = 7 * 0.5f;

	float4 pVertices[4];
	pVertices[0] = float4(input[0].positionW.x - fHalfW, input[0].positionW.y - fHalfH, input[0].positionW.z, 1.0f);
	pVertices[1] = float4(input[0].positionW.x - fHalfW, input[0].positionW.y + fHalfH, input[0].positionW.z, 1.0f);
	pVertices[2] = float4(input[0].positionW.x + fHalfW, input[0].positionW.y - fHalfH, input[0].positionW.z, 1.0f);
	pVertices[3] = float4(input[0].positionW.x + fHalfW, input[0].positionW.y + fHalfH, input[0].positionW.z , 1.0f);

	float2 pUVs[4] = { float2 (0.0f, 1.0f), float2(0.0f, 0.0f), float2(1.0f, 1.0f), float2(1.0f, 0.0f) };

	GS_EFFECT_OUTPUT output;

	for (int i = 0; i < 4; ++i)
	{
		output.posW = pVertices[i].xyz;
		output.posH = mul(mul(pVertices[i], gmtxView), gmtxProjection);
		output.UV = pUVs[i];
		outStream.Append(output);
	}
}

PS_EMMISIVE_MRT_OUTPUT EffectSPTPixelShader(GS_EFFECT_OUTPUT input)
{

	PS_EMMISIVE_MRT_OUTPUT output;

	input.UV.x /= 8;
	input.UV.x += (1.f / 8.f) * row;

	input.UV.y /= 8;
	input.UV.y += (1.f / 8.f) * col;


	float3 uvw = float3 (input.UV, 0.0f);
	float4 cColor = gtxtRealEffectTexture.Sample(gSamplerState, uvw);
	output.EmmisiveMRT = cColor;
	//output.NonLight = float4(1,1,1,1);
	return(output);
}
