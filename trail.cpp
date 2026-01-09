#include "trail.h"
#include <d3d11.h>
#include "direct3d.h"
#include "ShaderBillBoard.h"
using namespace DirectX;

static constexpr int POSITION_COUNT_MAX = 50;
static constexpr float POLIGON_WIDTH = 0.1f;
static ID3D11Buffer* g_pVertexBuffer = NULL;
static int g_Texture = -1;
static XMFLOAT3 g_PositionBuffer[POSITION_COUNT_MAX]{};
static XMFLOAT3 g_CameraPosition{};


// 頂点構造体
struct VertexTrail
{
	XMFLOAT3 position; // 頂点座標
	XMFLOAT2 texcoord; // テクスチャ座標（必要なら追加）

};

void Trail_Initialize()
{

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VertexTrail) * POSITION_COUNT_MAX*2;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	Direct3D_GetDevice()->CreateBuffer(&bd, NULL, &g_pVertexBuffer);
}

void Trail_Finalize()
{

	SAFE_RELEASE(g_pVertexBuffer);
}

void Trail_Draw()
{
	D3D11_MAPPED_SUBRESOURCE msr;
	Direct3D_GetContext()->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	VertexTrail* v = (VertexTrail*)msr.pData;
	for (int i = 0; i < POSITION_COUNT_MAX - 1; i++)
	{
		XMVECTOR vecEye = XMLoadFloat3(&g_PositionBuffer[i]) - XMLoadFloat3(&g_CameraPosition);
		XMVECTOR vecTrail = XMLoadFloat3(&g_PositionBuffer[i]) - XMLoadFloat3(&g_PositionBuffer[i + 1]);
		XMVECTOR vecCross = XMVector3Cross(vecEye,vecTrail);
		vecCross = XMVector3Normalize(vecCross);
		XMStoreFloat3(&v[i*2].position, XMLoadFloat3(&g_PositionBuffer[i])+ vecCross * POLIGON_WIDTH);
		v[i * 2].texcoord = XMFLOAT2(0.5f, 0.0);


		XMStoreFloat3(&v[i*2+1].position, XMLoadFloat3(&g_PositionBuffer[i]) -vecCross * POLIGON_WIDTH);
		v[i * 2 + 1].texcoord = XMFLOAT2(0.5f, 1.0f);


	}
	v[POSITION_COUNT_MAX * 2 - 2].position = g_PositionBuffer[POSITION_COUNT_MAX - 1];
	v[POSITION_COUNT_MAX * 2 - 2].texcoord = XMFLOAT2(0.5f, 0.0f);
	v[POSITION_COUNT_MAX * 2 - 1].position = g_PositionBuffer[POSITION_COUNT_MAX - 1];
	v[POSITION_COUNT_MAX * 2 - 1].texcoord = XMFLOAT2(0.5f, 1.0f);



	Direct3D_GetContext()->Unmap(g_pVertexBuffer, 0);
	ShaderBillboard_Begin();
	ShaderBillboard_UV({ 0.0f,0.0f }, { 1.f,1.f });

	UINT stride = sizeof(VertexTrail);
	UINT offset = 0;
	Direct3D_GetContext()->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	// ★ここが重要: mtxViewInverseを含める
	//XMMATRIX mtxWorldFinal = mtxScale * mtxPivot * mtxViewInverse * mtxWorld;

	ShaderBillboard_SetWorldMatrix(XMMatrixIdentity());
	// プリミティブトポロジ設定
	Direct3D_GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//Tecture_SetTexture(texId);
	ShaderBillboard_SetColor({1.0f,1.f,1.0f,1.0f});

	Direct3D_GetContext()->Draw(POSITION_COUNT_MAX*2, 0);
}

void Trail_AddPosition(const DirectX::XMFLOAT3& position)
{
	for (int i = 0; i < POSITION_COUNT_MAX - 1; i++)
	{
		g_PositionBuffer[i] = g_PositionBuffer[i + 1];
	}
	g_PositionBuffer[POSITION_COUNT_MAX - 1] = position;

}

void Trail_SetCameraPosition(const DirectX::XMFLOAT3& position)
{

	g_CameraPosition = position;
}
