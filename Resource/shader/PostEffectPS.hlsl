#include "PostEffect.hlsli"

Texture2D<float4> mainTex : register(t0);//0番スロットに設定されたテクスチャ
Texture2D<float4> subTex : register(t1);//1番スロットに設定されたテクスチャ

SamplerState smp : register(s0);//0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET {
	{
	//float4 texColor = tex.Sample(smp, input.uv);
	//return float4(texColor.rgb,1);
	}

	{
		float4 colorMainTex = mainTex.Sample(smp,input.uv);
		float4 colorSubTex = subTex.Sample(smp,input.uv);
	
		float4 color = colorMainTex;
		if (fmod(input.uv.y, 0.1f) < 0.05f) {
			color = colorSubTex;
		}

		return float4(color.rgb, 1);
	}
}