#include "Shaders.hlsl"

cbuffer DistortionBuffer : register(b10)
{
	float2 distortion1 : packoffset(c0);
	float2 distortion2 : packoffset(c0.z);
	float2 distortion3 : packoffset(c1);
	float distortionScale : packoffset(c1.z);
	float distortionBias : packoffset(c1.w);
};

struct VS_NOISE_INPUT
{
	float3 position : POSITION;
};

struct GS_BILLBOARD_OUTPUT
{
	float4 posH :SV_POSITION;
	float3 posW :POSITION;
	float2 tex : TEXCOORD0;
	float2 tex1 : TEXCOORD1;
	float2 tex2 : TEXCOORD2;
	float2 tex3 : TEXCOORD3;
	float4 vPorjPos : TEXCOORD4;
};

struct GS_NOISE_INPUT
{
	float3 positionW : POSITION;
};


GS_NOISE_INPUT NoiseVertexShader(VS_NOISE_INPUT input)
{
	GS_NOISE_INPUT output;

	output.positionW = input.position;

	return output;
}


[maxvertexcount(4)]
void GS(point GS_NOISE_INPUT input[1], inout TriangleStream<GS_BILLBOARD_OUTPUT> outStream)
{
	float3 vUP = float3(0.0f, 1.0f, 0.0f);
	float3 vLook = gvCameraPosition.xyz - input[0].positionW;
	vLook = normalize(vLook);
	float3 vRight = cross(vUP, vLook);

	float fHalfW = 50 * 0.5f;
	float fHalfH = 50 * 0.5f;

	float4 pVertices[4];
	pVertices[0] = float4(input[0].positionW + fHalfW * vRight - fHalfH * vUP, 1.0f);
	pVertices[1] = float4(input[0].positionW + fHalfW * vRight + fHalfH * vUP, 1.0f);
	pVertices[2] = float4(input[0].positionW - fHalfW * vRight - fHalfH * vUP, 1.0f);
	pVertices[3] = float4(input[0].positionW - fHalfW * vRight + fHalfH * vUP, 1.0f);

	float2 pUVs[4] = { float2 (0.0f, 1.0f), float2(0.0f, 0.0f), float2(1.0f, 1.0f), float2(1.0f, 0.0f) };

	GS_BILLBOARD_OUTPUT output;

	output.posW = pVertices[0].xyz;
	output.posH = mul(mul(pVertices[0], gmtxView), gmtxProjection);
	output.vPorjPos = output.posH;

	output.tex = pUVs[0];

	output.tex1 = output.tex2 = output.tex3 = output.tex;
	output.tex1 = (output.tex * scales.x);
	output.tex1.y = output.tex1.y + (frameTime * scrollSpeeds.x);

	output.tex2 = (output.tex * scales.y);
	output.tex2.y = output.tex2.y + (frameTime * scrollSpeeds.y);

	output.tex3 = (output.tex * scales.z);
	output.tex3.y = output.tex3.y + (frameTime * scrollSpeeds.z); 

	outStream.Append(output);

	output.posW = pVertices[1].xyz;
	output.posH = mul(mul(pVertices[1], gmtxView), gmtxProjection);
	output.vPorjPos = output.posH;

	output.tex = pUVs[1];

	output.tex1 = output.tex2 = output.tex3 = output.tex;
	output.tex1 = (output.tex * scales.x);
	output.tex1.y = output.tex1.y + (frameTime * scrollSpeeds.x);

	output.tex2 = (output.tex * scales.y);
	output.tex2.y = output.tex2.y + (frameTime * scrollSpeeds.y);

	output.tex3 = (output.tex * scales.z);
	output.tex3.y = output.tex3.y + (frameTime * scrollSpeeds.z); 

	outStream.Append(output);

	output.posW = pVertices[2].xyz;
	output.posH = mul(mul(pVertices[2], gmtxView), gmtxProjection);
	output.vPorjPos = output.posH;

	output.tex = pUVs[2];

	output.tex1 = output.tex2 = output.tex3 = output.tex;
	output.tex1 = (output.tex * scales.x);
	output.tex1.y = output.tex1.y + (frameTime * scrollSpeeds.x);

	output.tex2 = (output.tex * scales.y);
	output.tex2.y = output.tex2.y + (frameTime * scrollSpeeds.y);

	output.tex3 = (output.tex * scales.z);
	output.tex3.y = output.tex3.y + (frameTime * scrollSpeeds.z); 

	outStream.Append(output);

	output.posW = pVertices[3].xyz;
	output.posH = mul(mul(pVertices[3], gmtxView), gmtxProjection);
	output.vPorjPos = output.posH;

	output.tex = pUVs[3];

	output.tex1 = output.tex2 = output.tex3 = output.tex;
	output.tex1 = (output.tex * scales.x);
	output.tex1.y = output.tex1.y + (frameTime * scrollSpeeds.x);

	output.tex2 = (output.tex * scales.y);
	output.tex2.y = output.tex2.y + (frameTime * scrollSpeeds.y);

	output.tex3 = (output.tex * scales.z);
	output.tex3.y = output.tex3.y + (frameTime * scrollSpeeds.z); 

	outStream.Append(output);


}

PS_EMMISIVE_MRT_OUTPUT NoisePixelShader(GS_BILLBOARD_OUTPUT input)
{

	PS_EMMISIVE_MRT_OUTPUT output;

	float4 noise1 = gtxtNoiseTex.Sample(gSamplerState, input.tex1);
	float4 noise2 = gtxtNoiseTex.Sample(gSamplerState, input.tex2);
	float4 noise3 = gtxtNoiseTex.Sample(gSamplerState, input.tex3);

	noise1 = (noise1 - 0.5f) * 6.0f;
	noise2 = (noise2 - 0.5f) * 6.0f;
	noise3 = (noise3 - 0.5f) * 6.0f;

	noise1.xy = noise1.xy * distortion1.xy;
	noise2.xy = noise2.xy * distortion2.xy;
	noise3.xy = noise3.xy * distortion3.xy;

	float4 finalNoise = noise1 + noise2 + noise3;

	float perturb = ((1.0f - input.tex.y) * distortionScale) + distortionBias;

	float2 noiseCoords;
	noiseCoords.xy = (finalNoise.xy * perturb) + input.tex.xy;

	float4 fireColor = gtxtBaseColorNoiseTex.Sample(gSamplerClamp, noiseCoords.xy);
	float4 alphaColor = gtxtAlphaNoiseTex.Sample(gSamplerClamp, noiseCoords.xy);
	fireColor.a = alphaColor;
	output.EmmisiveMRT = fireColor;

	float2 f2pos = float2(input.posH.x / (gtx_Buffer_width / 2.0f) /2.0f - 1,
		input.posH.y / (gtx_Buffer_height / 2.0f) /2.0f - 1);

	float myDepth = input.vPorjPos.z;
	float4 fSceneDepth = gtxtSceneDepthTexture.Sample(gSamplerState, f2pos);

	float fSceenW = fSceneDepth.g * (gtx_Buffer_width / 2.0f);
	float fSceenZ = fSceneDepth.r * fSceenW;

	float fDepthDistance = fSceenZ - myDepth;
	if (/*(fDepthDistance < 80.f) &&*/ (fDepthDistance > 0.0f))
	{
		fDepthDistance = saturate(1 - (fDepthDistance / myDepth / 10.f)); // 0.7 //0.8 //0.9 
		//fDepthDistance *= 100; //0.3  //0.2 // 0.1 ..
		//fDepthDistance/= 
		output.EmmisiveMRT.a -= fDepthDistance;
	}
	
	
	
	return output;
}


struct VS_BASIC_INPUT
{
	float3 position : POSITION;
	float2 tex : TEXCOORD;
};

struct VS_BASIC_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
};

VS_BASIC_OUTPUT BasicShaderVS (VS_BASIC_INPUT input)
{
	VS_BASIC_OUTPUT output;

	float4 pos = mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(pos, gmtxView), gmtxProjection);

	output.tex = input.tex;

	return output;
}

PS_EMMISIVE_MRT_OUTPUT BasicShaderPS(VS_BASIC_OUTPUT input)
{
	PS_EMMISIVE_MRT_OUTPUT output;
	output.EmmisiveMRT = float4(0.1, 0.6, 0.9, 1);
	return output; 
}


PS_EMMISIVE_MRT_OUTPUT BasicTextureShaderPS(VS_BASIC_OUTPUT input)
{
	PS_EMMISIVE_MRT_OUTPUT output;
	output.EmmisiveMRT = gtxtWaterNormal.Sample(gSamplerClamp, input.tex);;
	return output;
}