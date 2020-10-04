#include "Shaders.hlsl"

float4 VSPostProcessing(uint nVertexID : SV_VertexID) : SV_POSITION
{
	if (nVertexID == 0) return(float4(-1.0f, +1.0f, 0.0f, 1.0f));
	if (nVertexID == 1) return(float4(+1.0f, +1.0f, 0.0f, 1.0f));
	if (nVertexID == 2) return(float4(+1.0f, -1.0f, 0.0f, 1.0f));
	if (nVertexID == 3) return(float4(-1.0f, +1.0f, 0.0f, 1.0f));
	if (nVertexID == 4) return(float4(+1.0f, -1.0f, 0.0f, 1.0f));
	if (nVertexID == 5) return(float4(-1.0f, -1.0f, 0.0f, 1.0f));

	return(float4(0, 0, 0, 0));
}

float4 PSPostProcessing(float4 position : SV_POSITION) : SV_Target
{
	float4 cColor = gtxtScene[int2(position.xy)];
	float4 fLighted = gtxtLight[int2(position.xy)];
	float4 cNonLight = gtxtNonLightNoise[int2(position.xy)];
	float fDepth = gtxtDepth[int2(position.xy)].g;


	int2 dir = int2(1, 1);
	const int offset[] = { 0, 1, 2, 3, 4, 5 };
	const float weight[] = { 0.3270270270, 0.2945945946, 0.1216216216, 0.0540540541, 0.0162162162 };
	float4 FragmentColor = float4(0, 0, 0, 0);

	int hstep = dir.x;
	int vstep = dir.y;

	for (int i = 1; i < 5; i++) {
		FragmentColor += gtxtEmmisive[int2(position.xy) + int2(hstep * offset[i], vstep * offset[i])] * weight[i] +
			gtxtEmmisive[int2(position.xy) - int2(hstep * offset[i], vstep * offset[i])] * weight[i];
	}
	float4 cEmmisive = FragmentColor;
	
	fLighted = min(fLighted * 2.6f + 0.3f, 2);
	cColor = cColor * fLighted ;

	float fog = length(cNonLight.rgb) ;

	float4 fogColor = float4(0.6f,0.6f,0.6f,1);
	if (fog > 0)
		fogColor.g *= cNonLight.g / cNonLight.r * 0.9f ;
	cColor = lerp(cColor, fogColor,fog );
	cColor += cEmmisive ;

	//cColor = lerp(cColor,fLighted,0.6f);

	return(cColor);
}

float4 PSIndoorPostProcessing(float4 position : SV_POSITION) : SV_Target
{
	float4 cColor = gtxtScene[int2(position.xy)];
	float4 fLighted = gtxtLight[int2(position.xy)];
	float4 cNonLight = gtxtNonLightNoise[int2(position.xy)];
	float fDepth = gtxtDepth[int2(position.xy)].g;


	int2 dir = int2(1, 1);
	const int offset[] = { 0, 1, 2, 3, 4, 5 };
	const float weight[] = { 0.3270270270, 0.2945945946, 0.1216216216, 0.0540540541, 0.0162162162 };
	float4 FragmentColor = float4(0, 0, 0, 0);

	int hstep = dir.x;
	int vstep = dir.y;

	for (int i = 1; i < 5; i++) {
		FragmentColor += gtxtEmmisive[int2(position.xy) + int2(hstep * offset[i], vstep * offset[i])] * weight[i] +
			gtxtEmmisive[int2(position.xy) - int2(hstep * offset[i], vstep * offset[i])] * weight[i];
	}
	float4 cEmmisive = FragmentColor;

	fLighted = min(fLighted * 2.6f, 2);
	cColor = cColor * fLighted;

	float fog = length(cNonLight.rgb);

	//float4 fogColor = float4(0.6f,0.6f,0.6f,1);
	//if (fog > 0)
		//fogColor.g *= cNonLight.g / cNonLight.r * 0.9f;
	cColor += cNonLight;
	cColor += cEmmisive;

	//cColor = lerp(cColor,fLighted,0.6f);

	return(cColor);
}

static float gfLaplacians[9] = { -1.0f, -1.0f, -1.0f, -1.0f, 8.0f, -1.0f, -1.0f, -1.0f, -1.0f };

float4 PSPostProcessingByLaplacianEdge(float4 position : SV_POSITION) : SV_Target //backbufferm
{
	float fEdgeness = 0.0f;
	float fDepthEdgeness = 0.0f;
	float3 cEdgeness = float3(0.0f, 0.0f, 0.0f);
	if ((uint(position.x) >= 1) || (uint(position.y) >= 1) 
		|| (uint(position.x) <= gtxtNormal.Length.x - 2) 
		|| (uint(position.y) <= gtxtNormal.Length.y - 2))
	{
		for (int i = 0; i < 9; i++)
		{
			float3 vNormal = gtxtNormal[int2(position.xy) + gnOffsets[i]].xyz;
			vNormal = vNormal * 2.0f - 1.0f;
			cEdgeness += gfLaplacians[i] * vNormal;
		}
		fEdgeness = 1 - (cEdgeness.r * 0.3f + cEdgeness.g * 0.59f + cEdgeness.b * 0.11f);
		cEdgeness = float3(fEdgeness, fEdgeness, fEdgeness) ;
	}

	float3 cColor = gtxtScene[int2(position.xy)].rgb;
	

	cColor = (fEdgeness > 0.85f) ? cColor : ((fEdgeness > 0.35f) ? (cColor * cEdgeness) : cEdgeness);


	return(float4(cColor, 1.0f));

}


struct VS_TEXTURED_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

struct VS_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VS_TEXTURED_OUTPUT VSUI(VS_TEXTURED_INPUT input)  /// 직교투영
{
	VS_TEXTURED_OUTPUT output;
	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxOrthoView), gmtxOrtho);
	output.uv = input.uv;

	return(output);
}

float4 PSUI(VS_TEXTURED_OUTPUT input) :SV_TARGET //backbuffer
{
	float4 cColor = float4 (1.0f, 0.0f, 0.0f,0.0f);

	if (gnObjectID == 0)  cColor = gtxtNormal.Sample(gSamplerState, input.uv);
	if (gnObjectID == 1)  cColor = gtxtScene.Sample(gSamplerState, input.uv);
	if (gnObjectID == 2)  cColor = gtxtDepth.Sample(gSamplerState, input.uv);
	if (gnObjectID == 3)  cColor = gtxtNonLightNoise.Sample(gSamplerState, input.uv);
	if (gnObjectID == 4)  cColor = gtxtCopySunTex.Sample(gSamplerState, input.uv);
	if (gnObjectID == 5)  cColor = gtxtLight.Sample(gSamplerState, input.uv);
	if (gnObjectID == 6)  cColor = gtxtEmmisive.Sample(gSamplerState, input.uv);

	return cColor;
}	