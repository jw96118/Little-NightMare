#include "Shader_Base.hpp"

struct VS_IN
{
	vector		vPosition	: POSITION;	
	vector		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;

};

struct VS_OUT
{
	vector		vPosition	: POSITION;
	vector		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;
	vector		vProjPos	: TEXCOORD1;
	vector		vWorldPos   : TEXCOORD2;

};

// 버텍스 쉐이더 함수
VS_OUT		VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition.xyz, 1.f), matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal.xyz, 0.f), g_matWorld));

	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vWorldPos = mul(vector(In.vPosition.xyz, 1.f), g_matWorld);;
	return Out;
}

struct PS_IN // 픽셀 쉐이더 구조체에서 POSITION이란 Semantic은 사용할 수 없다.
{
	vector			vNormal : NORMAL;
	float2			vTexUV : TEXCOORD0;
	vector			vProjPos : TEXCOORD1;
	vector		    vWorldPos : TEXCOORD2;
};

struct PS_OUT
{
	vector			vColor : COLOR0;
	vector			vNormal : COLOR1;
	vector			vDepth : COLOR2;
	vector		    vWorldPos : COLOR3;
};

PS_OUT		PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	
	Out.vColor = tex2D(BaseSampler, In.vTexUV);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w,	// z나누기를 수행하여 0~1사이의 투영 z값을 만들고, 이를 텍스쳐 uv좌표로 판단
						In.vProjPos.w * 0.001f,			// 뷰스페이스 상태의 z값을 텍스쳐의 uv로 변환
						0.f,
						0.f);
	Out.vWorldPos = In.vWorldPos;
	return Out;
}


PS_OUT		PS_ALPHA(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);		

	return Out;
}
////////////////////////////////////////////////
// 그림자 쉐이더

float		g_fFar = 1000.f;

struct VS_IN_SHADOW
{
	float3			vPosition : POSITION;
};

struct VS_OUT_SHADOW
{
	float4			vPosition : POSITION;
	float4			vProjPos : TEXCOORD0;
};

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN_SHADOW In)
{
	VS_OUT_SHADOW		Out = (VS_OUT_SHADOW)0;

	matrix			matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN_SHADOW
{
	float4			vPosition : POSITION;
	float4			vProjPos : TEXCOORD0;
};

struct PS_OUT_SHADOW
{
	vector			vLightDepth : COLOR0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN_SHADOW In)
{
	PS_OUT_SHADOW		Out = (PS_OUT_SHADOW)0;

	Out.vLightDepth.r = In.vProjPos.w / g_fFar;

	Out.vLightDepth.a = 1.f;

	return Out;
}

technique Default_Device
{
	pass
	{
		vertexshader = compile vs_3_0 VS_MAIN();		
		pixelshader  = compile ps_3_0 PS_MAIN();	
	}
	pass Depth
	{
		zwriteenable = false;

		vertexshader = compile vs_3_0 VS_MAIN_SHADOW();
		pixelshader = compile ps_3_0 PS_MAIN_SHADOW();
	}


	pass AlphaTesting
	{
		alphatestenable = true;
		alphafunc = greater;
		alpharef = 0xc0;
		cullmode = none;

		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader = compile ps_3_0 PS_ALPHA();
	}
};