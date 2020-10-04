//--------------------------------------------------------------------------------------
#define MAX_LIGHTS			20
#define MAX_MATERIALS		512 

#define POINT_LIGHT			1
#define SPOT_LIGHT			2
#define DIRECTIONAL_LIGHT	3

#define _WITH_LOCAL_VIEWER_HIGHLIGHTING
#define _WITH_THETA_PHI_CONES
//#define _WITH_REFLECT

struct LIGHT
{
	float4					m_cAmbient;
	float4					m_cDiffuse;
	float4					m_cSpecular;
	float3					m_vPosition;
	float 					m_fFalloff;
	float3					m_vDirection;
	float 					m_fTheta; //cos(m_fTheta)
	float3					m_vAttenuation;
	float					m_fPhi; //cos(m_fPhi)
	bool					m_bEnable;
	int 					m_nType;
	float					m_fRange;
	float					padding;
};

cbuffer cbLights : register(b3)
{
	float4					gcGlobalAmbientLight;
	LIGHT					gLights[MAX_LIGHTS];
};

// Schlick gives an approximation to Fresnel reflectance (see pg. 233 "Real-Time Rendering 3rd Ed.").
// R0 = ( (n-1)/(n+1) )^2, where n is the index of refraction.

float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVec)
{
	float cosIncidentAngle = saturate(dot(normal, lightVec));

	float f0 = 1.0f - cosIncidentAngle;
	float3 reflectPercent = R0 + (1.0f - R0) * (f0 * f0 * f0 * f0 * f0);

	return reflectPercent;
}

float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye)
{
	lightVec = -lightVec;
	const float m = 0.04f * 256.0f;
	float3 halfVec = normalize(-toEye + lightVec);
	float fDiffuseFactor = dot(lightVec, normal);

	float roughnessFactor =(m + 3.0f) * pow(max(dot(halfVec, normal), 0.0f), m) / 8.0f;
	float3 fresnelFactor = SchlickFresnel(float3(1.0f,1.0f,1.0f), halfVec, lightVec);

	float3 specAlbedo = fresnelFactor * roughnessFactor;

	return (float3(0.6f,0.6f,0.6f) * specAlbedo) + float3(0.3f,0.3f,0.3f) * fDiffuseFactor;
}

float4 NormalLight(float3 vNormal, float3 vToCamera)
{
	float3 vToLight = -float3(1.0f, -1.0f, 1.0f);
	float fDiffuseFactor = dot(vToLight, vNormal);
	float fSpecularFactor = 0.0f;

	if (fDiffuseFactor > 0.0f)
	{
		float3 vHalf = normalize(float3(vToCamera + vToLight));
		fSpecularFactor = pow(max(dot(vHalf, vNormal), 0.0f), 10.f);
	}

	return((float4(1.0f, 1.0f, 1.0f, 1.0f) * fDiffuseFactor) * float4(0.4f, 0.4f, 0.4f, 1.f) +
		fSpecularFactor  * float4(0.4f, 0.4f, 0.4f, 1.f));
}


float4 DirectionalLight(int nIndex, float3 vNormal, float3 vToCamera)
{

	float3 lightVec = -gLights[nIndex].m_vDirection;
	const float m = 0.06f * 256.0f;
	float3 halfVec = normalize(vToCamera + lightVec); 
	float fDiffuseFactor = dot(lightVec, vNormal) / 1.2f ;

	float4 diffuse = max(fDiffuseFactor * gLights[nIndex].m_cDiffuse,-0.15f);
	float4 spec = float4(0, 0, 0, 0);
	if (fDiffuseFactor > 0.0f)
	{
		float roughnessFactor = (m + 3.0f) * pow(max(dot(halfVec, vNormal), 0.0f), m) /15.0f;
		float3 fresnelFactor = SchlickFresnel(gLights[nIndex].m_cSpecular.xyz, halfVec, lightVec) / 2.5f;
		spec = float4 (fresnelFactor * roughnessFactor,1);
	}

	return (spec) + diffuse;
}

float4 PointLight(int nIndex, float3 vPosition, float3 vNormal, float3 vToCamera)
{
	float3 vToLight = gLights[nIndex].m_vPosition - vPosition;
	float fDistance = length(vToLight);
	float fresnelFactor = 0;

	if (fDistance <= gLights[nIndex].m_fRange)
	{
		float fSpecularFactor = 0.0f;
		vToLight /= fDistance;
		float fDiffuseFactor = dot(vToLight, vNormal);
		if (fDiffuseFactor > 0.0f)
		{
			float3 halfVec = normalize(-vToCamera + vToLight);
			const float m = 0.06f * 256.0f;
			float roughnessFactor = (m + 3.0f) * pow(max(dot(halfVec, vNormal), 0.0f), m) / 8.0f;
			fresnelFactor = SchlickFresnel(gLights[nIndex].m_cSpecular.xyz, halfVec, vToLight);
		}
		float fAttenuationFactor = 1.0f / dot(gLights[nIndex].m_vAttenuation, float3(1.0f, fDistance, fDistance * fDistance));
		float4 LightColor = (gLights[nIndex].m_cDiffuse * fDiffuseFactor + gLights[nIndex].m_cSpecular * fresnelFactor);
		return saturate(LightColor * fAttenuationFactor);
	}
	return(float4(0.0f, 0.0f, 0.0f, 0.0f));
}

float4 SpotLight(int nIndex, float3 vPosition, float3 vNormal, float3 vToCamera)
{
	float3 vToLight = gLights[nIndex].m_vPosition - vPosition;
	float fDistance = length(vToLight);
	float fresnelFactor = 0;

	if (fDistance <= gLights[nIndex].m_fRange)
	{
		float fSpecularFactor = 0.0f;
		vToLight /= fDistance;
		float fDiffuseFactor = dot(vToLight, vNormal);
		const float m = 0.06f * 256.0f;
		float AxisZAttenuation =0.0f; 
		if (fDiffuseFactor > 0.0f)
		{
		
				float3 vReflect = reflect(-vToLight, vNormal);
				fSpecularFactor = pow(max(dot(vReflect, vToCamera), 0.0f), m);
				fresnelFactor = saturate(SchlickFresnel(gLights[nIndex].m_cSpecular.xyz, vReflect, vToLight));
			
		}

		float fAlpha = max(dot(-vToLight, gLights[nIndex].m_vDirection), 0.0f);

		float fSpotFactor = pow(max(((fAlpha - gLights[nIndex].m_fPhi) /
			(gLights[nIndex].m_fTheta - gLights[nIndex].m_fPhi)), 0.0f), gLights[nIndex].m_fFalloff);

		fSpotFactor = max(fSpotFactor, 0);
		float fAttenuationFactor = max(1.0f / dot(gLights[nIndex].m_vAttenuation,
			                            float3(1.0f, fDistance, fDistance * fDistance)),0);

		float4 LightColor =  saturate((gLights[nIndex].m_cDiffuse * fDiffuseFactor + gLights[nIndex].m_cSpecular * fresnelFactor));

		if (fDistance / gLights[nIndex].m_fRange > 0.5f)
		{
			AxisZAttenuation = max((gLights[nIndex].m_fRange - fDistance) / (gLights[nIndex].m_fRange / 2),0);
			return(LightColor * fAttenuationFactor * fSpotFactor * AxisZAttenuation);
		}

		else
		{
			return(LightColor * fAttenuationFactor * fSpotFactor /** AxisZAttenuation*/);
		}//return LightColor * fAttenuationFactor;
	}
	return(float4(0.0f, 0.0f, 0.0f, 0.0f));
}

float4 Lighting(float3 vPosition, float3 vNormal)
{
	float3 vCameraPosition = float3(gvCameraPosition.x, gvCameraPosition.y, gvCameraPosition.z);
	float3 vToCamera = normalize(vCameraPosition - vPosition);

	float4 cColor = gcGlobalAmbientLight;
	[unroll(MAX_LIGHTS)] for (int i = 0; i < MAX_LIGHTS; i++)
	{
		if (gLights[i].m_bEnable)
		{
			if (gLights[i].m_nType == DIRECTIONAL_LIGHT)
			{
				//cColor += BlinnPhong(gLights[i].m_cDiffuse, gLights[i].m_vDirection, vNormal, vToCamera);
				cColor += DirectionalLight(i, vNormal, vToCamera);
			}
			else if (gLights[i].m_nType == POINT_LIGHT)
			{
				cColor += PointLight(i, vPosition, vNormal, vToCamera);
			}
			else if (gLights[i].m_nType == SPOT_LIGHT)
			{
				cColor += SpotLight(i, vPosition, vNormal, vToCamera);
			}
		}
	}

	return(cColor);
}

