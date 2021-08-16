texture				g_AlbedoTexture;

sampler AlbedoSampler = sampler_state
{
	texture = g_AlbedoTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};
texture			g_ShadeTexture;

sampler ShadeSampler = sampler_state
{
	texture = g_ShadeTexture;
};
texture g_LightTexture;

sampler LightSampler = sampler_state
{
	texture = g_LightTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture g_SpecularTexture;

sampler SpecularSampler = sampler_state
{
	texture = g_SpecularTexture;

	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture		g_DepthTexture;

sampler DepthSampler = sampler_state
{
	texture = g_DepthTexture;
minfilter = linear;
magfilter = linear;
mipfilter = none;
AddressU = clamp;
AddressV = clamp;
};


texture		g_ShadowDepthTexture;

sampler ShadowDepthSampler = sampler_state
{
	texture = g_ShadowDepthTexture;
minfilter = linear;
magfilter = linear;
mipfilter = none;
AddressU = clamp;
AddressV = clamp;
};


matrix		g_matProj;
matrix		g_matViewInv;
matrix		g_matProjInv;

matrix		g_matLightView;

texture		g_DiffuseTexture;

vector		g_vLightDir;



struct PS_IN
{
	float2			vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector			vColor : COLOR0;
};

PS_OUT		PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vAlbedo = tex2D(AlbedoSampler, In.vTexUV);
	vector		vShade = tex2D(ShadeSampler, In.vTexUV);
	vector		vSpecular = tex2D(SpecularSampler, In.vTexUV);

	Out.vColor = vAlbedo  * vShade + vSpecular;

	vector		vDepthInfo = tex2D(DepthSampler, In.vTexUV);
	float		fViewZ = vDepthInfo.y * 1000.0f;

	vector		vPosition;

	vPosition.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vPosition.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	vPosition.z = vDepthInfo.x * fViewZ;
	vPosition.w = fViewZ;

	// ºä »ó
	vPosition = mul(vPosition, g_matProjInv);

	// ¿ùµå »ó
	vPosition = mul(vPosition, g_matViewInv);

	vPosition = mul(vPosition, g_matLightView);

	vector		vUVPos = mul(vPosition, g_matProj);
	float2		vNewUV;

	vNewUV.x = (vUVPos.x / vUVPos.w) * 0.5f + 0.5f;
	vNewUV.y = (vUVPos.y / vUVPos.w) * -0.5f + 0.5f;

	vector		vShadowDepthInfo = tex2D(ShadowDepthSampler, vNewUV);

	if (vPosition.z - 0.1f > vShadowDepthInfo.r * 1000.0f)
		Out.vColor *= 0.75f;
	//Out.vColor = vector(0.f, 0.f, 0.f, 1.f);

	return Out;
}

technique Default_Device
{
	pass Blend
	{
		zenable = false;
		ZWriteEnable = false;
		/*alphatestenable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;*/

		alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;


		vertexShader = NULL;
		pixelshader = compile ps_3_0 PS_MAIN();
	}

};