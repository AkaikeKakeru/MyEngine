#include "PostEffect.hlsli"

VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD) {
	VSOutput output; //ピクセルシェーダーに渡す値
	output.svpos = pos;//座標に行列を乗算
	output.uv = uv;

	return output;
}