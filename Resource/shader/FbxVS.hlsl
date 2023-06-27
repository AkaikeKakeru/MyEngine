#include "Fbx.hlsli"
　
struct SkinOutput {
	float4 pos;
	float3 normal;
};

VSOutput main(VSInput input) {
	//法線にワールド行列によるスケーリング・回転を適用
	float4 wnormal = normalize(mul(world, float4(input.normal, 0)));
	float4 wpos = mul(world, input.pos);

	//ピクセルシェーダーに渡す
	VSOutput output;

	output.svpos = mul(mul(viewproj, world), input.pos);
	output.worldpos = wpos;
	output.normal = wnormal.xyz;
	output.uv = input.uv;

	return output;
}