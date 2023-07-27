#include "PostEffect.hlsli"

Texture2D<float4> mainTex : register(t0);//0番スロットに設定されたテクスチャ
Texture2D<float4> subTex : register(t1);//1番スロットに設定されたテクスチャ

SamplerState smp : register(s0);//0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET{
	float4 colorMainTex = float4(0,0,0,0);
	float4 colorSubTex = float4(0,0,0,0);

	float u = 1.0f / 1280.0f;
	float v = 1.0f / 780.0f;

	float4 color = float4(0,0,0,0);

	{
		colorMainTex = mainTex.Sample(smp, input.uv);
		colorMainTex = float4(1.0f - colorMainTex.rgb, 1.0f);
	}

	{
		colorSubTex += subTex.Sample(smp, input.uv + float2(-1.0f * u, -1.0f * v));
		colorSubTex += subTex.Sample(smp, input.uv + float2(		0, -1.0f * v));
		colorSubTex += subTex.Sample(smp, input.uv + float2(+1.0f * u, -1.0f * v));

		colorSubTex += subTex.Sample(smp, input.uv + float2(-1.0f * u, 0));
		colorSubTex += subTex.Sample(smp, input.uv + float2(		0, 0));
		colorSubTex += subTex.Sample(smp, input.uv + float2(+1.0f * u, 0));

		colorSubTex += subTex.Sample(smp, input.uv + float2(-1.0f * u, +1.0f * v));
		colorSubTex += subTex.Sample(smp, input.uv + float2(		0, +1.0f * v));
		colorSubTex += subTex.Sample(smp, input.uv + float2(+1.0f * u, +1.0f * v));

		colorSubTex = float4(colorSubTex.rgb / 9.0f, 1.0f);
	}

	if (fmod(input.uv.y, 0.1f) < 0.05f) {
		color = colorMainTex;
	}
	else {
		color = colorSubTex;
	}

	return float4(color.rgb, 1.0f);
}