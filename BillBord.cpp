/*==========================================================================

ビルボードの表示

												Author : hidetoshi muramatu
												Date   : 2025/12/04
---------------------------------------------------------------------------






=========================================================================*/


#include "BillBord.h"
#include "ShaderBillBoard.h"
#include <DirectXMath.h>
#include "direct3d.h"
#include "texture.h"
using namespace DirectX;


static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ
static ID3D11Buffer* g_pIndexBuffer = nullptr; // 頂点バッファ

static constexpr int NUM_VERTEX = 4; // 頂点数
static XMMATRIX g_mtxView{};

// 頂点構造体
struct VertexBillboard
{
	XMFLOAT3 position; // 頂点座標
	XMFLOAT2 texcoord; // テクスチャ座標（必要なら追加）

};
static VertexBillboard g_BillboardVertex[] =
{
	// 前面 (Z+)
	{{-0.5f,  0.5f,   0.0f},  {0.0f, 0.0f}},
	{{0.5f, 0.5f,     0.0f},   {1.0f, 0.0f}},
	{{ -0.5f, -0.5f,  0.0f}, {0.0f, 1.0f}},
	{{ 0.5f,  -0.5f,  0.0f}, {1.0f, 1.0f}},
};

void Billboard_Initialize()
{
	

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd{};
	
	bd.Usage = D3D11_USAGE_DYNAMIC;;
	bd.ByteWidth = sizeof(VertexBillboard) * NUM_VERTEX;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA sd;
	sd.pSysMem = g_BillboardVertex;

	//頂点バッファへ流し込むデータの設計

	Direct3D_GetDevice()->CreateBuffer(&bd, &sd, &g_pVertexBuffer);






}

void Billboard_Finalize()
{
	SAFE_RELEASE(g_pVertexBuffer);

}

void Billboard_Update(double elapsedTime)
{


}

void Billboard_SetViewMatrix(const DirectX::XMMATRIX& mtxWorld)
{

	g_mtxView = mtxWorld;

}

void Billboard_Draw(
	int texid, const DirectX::XMMATRIX& mtxWorld,
	const DirectX::XMFLOAT2& scale,
	const DirectX::XMFLOAT2& pivot,
	const DirectX::XMFLOAT4& color)

{
	//// シェーダーを描画パイプラインに設定
	//ShaderBillboard_Begin();
	//ShaderBillboard_UV({ 0.0f,0.0f }, {1.0,1.0});
	//UINT stride = sizeof(VertexBillboard);
	//UINT offset = 0;
	//Direct3D_GetContext()->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	//XMFLOAT4X4 mtxView{};
	//XMStoreFloat4x4(&mtxView, g_mtxView);
	//mtxView._41 = mtxView._42 = mtxView._43 = 0.0f;
	//// 回転成分のみの行列から逆行列を計算
	//XMMATRIX mtxViewRotationOnly = XMLoadFloat4x4(&mtxView);
	//XMMATRIX mtxViewInverse = XMMatrixInverse(nullptr, mtxViewRotationOnly);
	//XMMATRIX mtxScale = XMMatrixScaling(scal.x, scal.y, 1.0f);
	//XMMATRIX mtxPivot = XMMatrixTranslation(-pivot.x, -pivot.y, 0.0f);
	//// ★ここが重要: mtxViewInverseを含める
	//XMMATRIX mtxWorldFinal = mtxScale * mtxPivot * mtxViewInverse * mtxWorld;
	//ShaderBillboard_SetWorldMatrix(mtxWorldFinal);
	//// プリミティブトポロジ設定
	//Direct3D_GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//Tecture_SetTexture(texid);
	//ShaderBillboard_SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	//Direct3D_GetContext()->Draw(NUM_VERTEX, 0);


	Billboard_Draw(texid, mtxWorld, { 0.0f,0.0f }, { (float)Texture_GetWidht(texid),(float)Texture_GetHeight(texid) }, scale, color, pivot);

}

void Billboard_Draw(
	int texId,
	const DirectX::XMMATRIX& mtxWorld,
	const DirectX::XMFLOAT2& uv_offset,
	const DirectX::XMFLOAT2& uv_size,
	const DirectX::XMFLOAT2& scale,
	const DirectX::XMFLOAT4& color,
	const DirectX::XMFLOAT2& pivot

)

{

	// シェーダーを描画パイプラインに設定
	ShaderBillboard_Begin();

	float x = uv_offset.x  / Texture_GetWidht(texId);
	float y = uv_offset.y / Texture_GetHeight(texId);
	float w = uv_size.x / Texture_GetWidht(texId);
	float h = uv_size.y / Texture_GetHeight(texId);



	ShaderBillboard_UV({ x,y }, { w,h });


	UINT stride = sizeof(VertexBillboard);
	UINT offset = 0;
	Direct3D_GetContext()->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	XMFLOAT4X4 mtxView{};
	XMStoreFloat4x4(&mtxView, g_mtxView);
	mtxView._41 = mtxView._42 = mtxView._43 = 0.0f;

	// 回転成分のみの行列から逆行列を計算
	XMMATRIX mtxViewRotationOnly = XMLoadFloat4x4(&mtxView);
	XMMATRIX mtxViewInverse = XMMatrixInverse(nullptr, mtxViewRotationOnly);

	XMMATRIX mtxScale = XMMatrixScaling(scale.x, scale.y, 1.0f);
	XMMATRIX mtxPivot = XMMatrixTranslation(-pivot.x, -pivot.y, 0.0f);

	// ★ここが重要: mtxViewInverseを含める
	XMMATRIX mtxWorldFinal = mtxScale * mtxPivot * mtxViewInverse * mtxWorld;

	ShaderBillboard_SetWorldMatrix(mtxWorldFinal);

	// プリミティブトポロジ設定
	Direct3D_GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	Tecture_SetTexture(texId);
	ShaderBillboard_SetColor(color);

	Direct3D_GetContext()->Draw(NUM_VERTEX, 0);


}

void Billboard_Draw(int texid, const DirectX::XMMATRIX& mtxWorld, const DirectX::XMFLOAT2& scal, const DirectX::XMFLOAT4& color)
{


	// シェーダーを描画パイプラインに設定
	ShaderBillboard_Begin();

	UINT stride = sizeof(VertexBillboard);
	UINT offset = 0;
	Direct3D_GetContext()->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	XMFLOAT4X4 mtxView{};
	XMStoreFloat4x4(&mtxView, g_mtxView);
	mtxView._41 = mtxView._42 = mtxView._43 = 0.0f;



	//Direct3D_GetContext()->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	XMMATRIX mtxViewInverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(&mtxView));

	XMMATRIX mtxScale = XMMatrixScaling(scal.x, scal.y, 1.0);


	ShaderBillboard_SetWorldMatrix(mtxScale * mtxViewInverse*mtxWorld); //合成したい順番で掛け算する




	// プリミティブトポロジ設定
	Direct3D_GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	Tecture_SetTexture(texid); // テクスチャの設定
	ShaderBillboard_SetColor({ 1.0,1.0,1.0 ,1.0 });

	// ポリゴン描画命令発行
	//g_pContext->Draw(NUM_VERTEX, 0);
	Direct3D_GetContext()->Draw(NUM_VERTEX, 0);

}

void Billboard_Draw(int texid, const DirectX::XMMATRIX& mtxWorld, const DirectX::XMFLOAT4& color)
{
	

}
