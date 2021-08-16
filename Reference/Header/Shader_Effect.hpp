#include "Shader_Base.hpp"

vector          g_vEffectRGB;

texture			g_DepthTexture;

sampler DepthSampler = sampler_state
{
	texture = g_DepthTexture;
};

struct VS_IN
{
	vector				vPosition	: POSITION;
	float2				vTexUV		: TEXCOORD0;
};

struct VS_OUT
{
	vector				vPosition	: POSITION;
	float2				vTexUV		: TEXCOORD0;
	vector				vProjPos	: TEXCOORD1;

};

VS_OUT	VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV  = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition.xyz, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct  PS_IN
{
	float2			vTexUV : TEXCOORD0;
	vector			vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	vector			vColor : COLOR0;
};

PS_OUT		PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);
	if (g_vEffectRGB.a == 0)
	{
		Out.vColor.r *= g_vEffectRGB.r;
		Out.vColor.g *= g_vEffectRGB.g;
		Out.vColor.b *= g_vEffectRGB.b;
	}
	Out.vColor.a = Out.vColor.rgb;
	//float2		vDepthUV = (float2)0.f;

	//// -1 -> 0, 1 -> 1 (투영좌표인 x값이 텍스처 uv로 좌표 변환)

	//vDepthUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;

	//// 1 -> 0, -1 -> 1(투영좌표인 y값이 텍스처 uv로 좌표 변환)
	//vDepthUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	//float	vViewZ = tex2D(DepthSampler, vDepthUV).y * 1000.f;

	//Out.vColor.a = Out.vColor.a * saturate(vViewZ - In.vProjPos.w);

	return Out;
}

struct VS_IN_PARTICLE
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;

	float4		vRight : TEXCOORD1;
	float4		vUp : TEXCOORD2;
	float4		vLook : TEXCOORD3;
	float4		vPos : TEXCOORD4;
};

struct VS_OUT_PARTICLE
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

VS_OUT_PARTICLE	VS_INSTANCE_MAIN(VS_IN_PARTICLE In)
{
	VS_OUT_PARTICLE	Out = (VS_OUT_PARTICLE)0;

	matrix matWorld = float4x4(In.vRight, In.vUp, In.vLook, In.vPos);
	matrix matWV, matWVP;

	matWV = mul(matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition.xyz, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN_PARTICLE
{
	float4			vPosition : POSITION;
	float2			vTexUV : TEXCOORD0;
};

struct PS_OUT_PARTICLE
{
	vector			vColor : COLOR0;
};

PS_OUT_PARTICLE		PS_INSTANCE_MAIN(PS_IN_PARTICLE In)
{
	PS_OUT_PARTICLE		Out = (PS_OUT_PARTICLE)0;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	return Out;
}

technique Default_Device
{
	pass
	{
		alphablendenable = true;
		srcblend = srcalpha;	
		destblend = invsrcalpha;

		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader = compile ps_3_0 PS_MAIN();
	}

	pass Instancing
	{
		alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		zwriteenable = false;
		cullmode = none;

		vertexshader = compile vs_3_0	VS_INSTANCE_MAIN();
		pixelshader = compile ps_3_0	PS_INSTANCE_MAIN();

	}

};