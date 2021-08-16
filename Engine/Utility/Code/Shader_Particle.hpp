#include "Shader_Base.hpp"

struct VS_IN_PARTICLE
{
	float3		vPosition   : POSITION;
	float		fPSize		: PSIZE;
	float2		vTexUV		: TEXCOORD0;

	float4		vRight		: TEXCOORD1;
	float4		vUp			: TEXCOORD2;
	float4		vLook		: TEXCOORD3;
	float4		vPos		: TEXCOORD4;
};

struct VS_OUT_PARTICLE
{
	float4		vPosition : POSITION;
	float		fPSize : PSIZE;
	float2		vTexUV : TEXCOORD0;
};

VS_OUT_PARTICLE	VS_MAIN(VS_IN_PARTICLE In)
{
	VS_OUT_PARTICLE	Out = (VS_OUT_PARTICLE)0;
	
	matrix matWorld = float4x4(In.vRight, In.vUp, In.vLook, In.vPos);
	matrix matWV, matWVP;

	matWV	= mul(matWorld, g_matView);
	matWVP	= mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition.xyz, 1.f), matWVP);
	
	// 화면 해상도 높이값(필요에 따라 변경 해야 함)
	Out.fPSize = (600.f * In.fPSize) * sqrt(1.f / pow(Out.vPosition.w, 2.f));
	
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN_PARTICLE
{
	float4			vPosition	: POSITION;
	float			fPSize		: PSIZE;
	float2			vTexUV		: TEXCOORD0;
};

struct PS_OUT_PARTICLE
{
	vector			vColor		: COLOR0;
};

PS_OUT_PARTICLE		PS_MAIN(PS_IN_PARTICLE In)
{
	PS_OUT_PARTICLE		Out = (PS_OUT_PARTICLE)0;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	return Out;
}

technique Default_Device
{
	pass PointSprite
	{
		pointspriteenable = true;
		zwriteenable = false;

		alphatestenable = true;
		alpharef = 0;
		alphafunc = greater;

		vertexshader = compile vs_3_0	VS_MAIN();
		pixelshader = compile ps_3_0	PS_MAIN();

	}
};