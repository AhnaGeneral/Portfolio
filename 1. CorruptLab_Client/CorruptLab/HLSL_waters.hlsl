#include "HLSL_Noise.hlsl"


struct VS_WATER_INPUT
{
	float3 position : POSITION;
	float2 tex : TEXCOORD;
};


struct PS_WATER_INPUT
{
	float3 positionW : TEXCOORD4;
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float2 tex1 : TEXCOORD1;
	float2 tex2 : TEXCOORD2;
	float2 tex3 : TEXCOORD3;
	float4 posj : TEXCOORD5;
	float3 tangent : TANGENT;
	float3 normal : NORMAL;
	float3 bitangent : BITANGENT;
};

PS_WATER_INPUT WaterVertexShader(VS_WATER_INPUT input)
{
	PS_WATER_INPUT output;

	float4 pos = mul(float4(input.position, 1.0f), gmtxGameObject);
	output.positionW = pos.xyz;
	output.position = mul(mul(pos, gmtxView), gmtxProjection);

	output.tex = input.tex;

	output.tex1 = (input.tex * scales.y);

	output.tex2 = (input.tex * scales.z);
	output.tex2.x = output.tex2.x + (frameTime * (-scrollSpeeds.z));
	output.tex2.y = output.tex2.y + (frameTime * (0.04));


	output.tex3 = (input.tex * scales.z);
	output.tex3.x = output.tex3.x + (frameTime * scrollSpeeds.z);
	output.tex3.y = output.tex3.y + (frameTime * 0.08);


	output.normal = float3(0.0f, 1.0f, 0.0f);
	output.tangent = float3 (0.0f, 0.0f, 1.0f);
	output.bitangent = float3 (1.0f, 0.0f, 0.0f);
	output.posj = output.position;
	return output;
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT WaterPixelShader(PS_WATER_INPUT input) : SV_TARGET
{

	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	float4 noise3 = gtxtWaterNormal.Sample(gSamplerState, input.tex3);
	float4 noise2 = gtxtWaterNormal.Sample(gSamplerState, input.tex2);
	float4 stopnoise = gtxtWaterNormal.Sample(gSamplerState, input.tex1); 

	float3 normalizenoise2 = normalize(noise2.rgb * 2.0f - 1.0f);
	float3 normalizenoise3 = normalize(noise3.rgb * 2.0f - 1.0f);
	float3 normalizestop = normalize(stopnoise.rgb * 2.0f - 1.0f);

	float3x3 TBN = float3x3((input.tangent), (input.bitangent), (input.normal));
	float3 finalNormal = normalize(mul( normalizenoise2 + normalizenoise3 + normalizestop,TBN));

	float3 toCamera = normalize(gvCameraPosition - input.positionW);
	float3 cColor = lerp(float4(0.3f, 0.6f, 0.85f, 1.f), 
		BlinnPhong(float3(0.9f, 0.9f, 0.9f),
			normalize(float3(1, -0.9f, 1)),
			finalNormal, toCamera),0.5f);
	//float3 cColor = BlinnPhong(float3(0.5f, 0.5f, 0.5f), float3(1, -1, 1), finalNormal, toCamera);
	float fDepth = input.posj.z / (gtx_Buffer_width / 2.0f);
	if (fDepth > 0.4f)
		cColor = lerp(cColor, float4(1, 1, 1, 1), fDepth - 0.4f);


	output.color = float4(cColor, 1);

	output.depth = float4(input.posj.z / input.posj.w, 
		input.posj.w / (gtx_Buffer_width / 2.0f), 0, 1);
	//output.NonLight = float4 (1.0f, 0.0f, 0.0f, 1.0f);
	return output;
}