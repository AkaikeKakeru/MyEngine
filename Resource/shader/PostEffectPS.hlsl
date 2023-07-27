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

	if (fmod(input.uv.y, 0.1f) < 0.05f) {
		colorMainTex = mainTex.Sample(smp, input.uv);
		colorMainTex = float4(1.0f - colorMainTex.rgb, 1.0f);
	}
	else {
		const int MaxU = 3;
		const int MaxV = 3;

		for (int i = 0; i < MaxV; i++) {
			for (int j = 0; j < MaxU; j++) {
				colorSubTex += subTex.Sample(smp, input.uv + float2((j - 1) * u, (i - 1) * v));
			}
		}

		colorSubTex = float4(colorSubTex.rgb / (MaxU * MaxV), 1.0f);
	}

	color = colorMainTex + colorSubTex;

	return float4(color.rgb, 1.0f);
}