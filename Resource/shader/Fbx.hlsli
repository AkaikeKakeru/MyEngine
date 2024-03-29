cbuffer cbuff0 : register(b0) {
	matrix viewproj;//ビュープロジェクション行列
	matrix world;//ワールド行列
	float3 cameraPos;//カメラ座標 (ワールド座標)
};

cbuffer cbuff1 : register(b1) {
	float3 m_ambient : packoffset(c0); //アンビエント
	float3 m_diffuse : packoffset(c1); //ディヒューズ
	float3 m_specular : packoffset(c2); //スペキュラー
	float m_alpha : packoffset(c2.w); //アルファ
}

static const uint DIR_LIGHT_NUM = 3;

struct DirLight {
	float3 lightv;//ライトへの方向の単位ベクトル
	float3 lightcolor;//ライトの色(RGB)
	uint active;
};

cbuffer cbuff2 : register(b2) {
	float3 ambientColor;
	DirLight dirLights[DIR_LIGHT_NUM];
}

//ボーンの最大値
static const int MAX_BONES = 32;

cbuffer skinning : register(b3) {
	matrix matSkinning[MAX_BONES];
}

//バーテックスバッファの入力
struct VSInput {
	float4 pos : POSITION; //位置
	float3 normal : NORMAL; //頂点法線
	float2 uv : TEXCOORD; //テクスチャ座標
	uint4 boneIndices : BONEINDICES; //ボーンの番号
	float4 boneWeights : BONEWEIGHTS; //ボーンのスキンウェイト
};

//頂点シェーダからピクセルシェーダへの出力
struct VSOutput {
	float4 svpos : SV_POSITION; //システム用頂点座標
	float4 worldpos:POSITION;//ワールド座標
	float3 normal:NORMAL;//法線
	float2 uv : TEXCOORD; //uv値
};

struct PSOutput {
	float4 target0 : SV_TARGET0;
	float4 target1 : SV_TARGET1;
};