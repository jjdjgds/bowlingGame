/*==========================================================================

ビルボードの表示

												Author : hidetoshi muramatu
												Date   : 2025/12/04
---------------------------------------------------------------------------*/

#include "billBord.h"
#include "shaderBillBoard.h"
#include <DirectXMath.h>
#include "direct3d.h"
#include "texture.h"
using namespace DirectX;


static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ
static ID3D11Buffer* g_pIndexBuffer = nullptr; // 頂点バッファ
static ID3D11DepthStencilState* g_pDepthNoWrite = nullptr; // 深度書き込み無効ステート

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

	Direct3D_GetDevice()->CreateBuffer(&bd, &sd, &g_pVertexBuffer);

	// 深度書き込み無効ステート作成（透明描画用）
	D3D11_DEPTH_STENCIL_DESC dsdesc{};
	dsdesc.DepthEnable = TRUE;
	dsdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 書き込み無効
	dsdesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsdesc.StencilEnable = FALSE;
	Direct3D_GetDevice()->CreateDepthStencilState(&dsdesc, &g_pDepthNoWrite);
}

void Billboard_Finalize()
{
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pDepthNoWrite);
}

void Billboard_Update(double elapsedTime)
{}

void Billboard_SetViewMatrix(const DirectX::XMMATRIX& mtxWorld)
{
	g_mtxView = mtxWorld;
}

static void SaveCommonPipelineState(
	ID3D11DeviceContext* ctx,
	ID3D11VertexShader** outVS,
	ID3D11PixelShader** outPS,
	ID3D11InputLayout** outIL,
	ID3D11Buffer* outVSCB[3],
	ID3D11Buffer* outPSCB[3],
	ID3D11ShaderResourceView* outSRV[1],
	ID3D11SamplerState* outSamp[1])
{
	*outVS = nullptr;
	*outPS = nullptr;
	ctx->VSGetShader(outVS, nullptr, nullptr);
	ctx->PSGetShader(outPS, nullptr, nullptr);

	*outIL = nullptr;
	ctx->IAGetInputLayout(outIL);

	for (int i = 0; i < 3; ++i) { outVSCB[i] = nullptr; outPSCB[i] = nullptr; }
	ctx->VSGetConstantBuffers(0, 3, outVSCB);
	ctx->PSGetConstantBuffers(0, 3, outPSCB);

	outSRV[0] = nullptr;
	ctx->PSGetShaderResources(0, 1, outSRV);

	outSamp[0] = nullptr;
	ctx->PSGetSamplers(0, 1, outSamp);
}

static void RestoreCommonPipelineState(
	ID3D11DeviceContext* ctx,
	ID3D11VertexShader* prevVS,
	ID3D11PixelShader* prevPS,
	ID3D11InputLayout* prevIL,
	ID3D11Buffer* prevVSCB[3],
	ID3D11Buffer* prevPSCB[3],
	ID3D11ShaderResourceView* prevSRV[1],
	ID3D11SamplerState* prevSamp[1])
{
	ctx->VSSetShader(prevVS, nullptr, 0);
	ctx->PSSetShader(prevPS, nullptr, 0);
	ctx->IASetInputLayout(prevIL);
	ctx->VSSetConstantBuffers(0, 3, prevVSCB);
	ctx->PSSetConstantBuffers(0, 3, prevPSCB);
	ctx->PSSetShaderResources(0, 1, prevSRV);
	ctx->PSSetSamplers(0, 1, prevSamp);

	if (prevVS) prevVS->Release();
	if (prevPS) prevPS->Release();
	if (prevIL) prevIL->Release();
	for (int i = 0; i < 3; ++i) { if (prevVSCB[i]) prevVSCB[i]->Release(); if (prevPSCB[i]) prevPSCB[i]->Release(); }
	if (prevSRV[0]) prevSRV[0]->Release();
	if (prevSamp[0]) prevSamp[0]->Release();
}

void Billboard_Draw(
	int texid, const DirectX::XMMATRIX& mtxWorld,
	const DirectX::XMFLOAT2& scale,
	const DirectX::XMFLOAT2& pivot,
	const DirectX::XMFLOAT4& color)
{
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
	ID3D11DeviceContext* ctx = Direct3D_GetContext();

	// --- 退避 ---
	ID3D11VertexShader* prevVS = nullptr;
	ID3D11PixelShader* prevPS = nullptr;
	ID3D11InputLayout* prevIL = nullptr;
	ID3D11Buffer* prevVSCB[3] = { nullptr,nullptr,nullptr };
	ID3D11Buffer* prevPSCB[3] = { nullptr,nullptr,nullptr };
	ID3D11ShaderResourceView* prevSRV[1] = { nullptr };
	ID3D11SamplerState* prevSamp[1] = { nullptr };

	SaveCommonPipelineState(ctx, &prevVS, &prevPS, &prevIL, prevVSCB, prevPSCB, prevSRV, prevSamp);

	// DepthStencilState の退避
	ID3D11DepthStencilState* prevDSS = nullptr;
	UINT prevRef = 0;
	ctx->OMGetDepthStencilState(&prevDSS, &prevRef);

	// Billboard 用シェーダーセット
	ShaderBillboard_Begin();

	float x = uv_offset.x  / Texture_GetWidht(texId);
	float y = uv_offset.y / Texture_GetHeight(texId);
	float w = uv_size.x / Texture_GetWidht(texId);
	float h = uv_size.y / Texture_GetHeight(texId);

	ShaderBillboard_UV({ x,y }, { w,h });

	UINT stride = sizeof(VertexBillboard);
	UINT offset = 0;
	ctx->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	XMFLOAT4X4 mtxView{};
	XMStoreFloat4x4(&mtxView, g_mtxView);
	mtxView._41 = mtxView._42 = mtxView._43 = 0.0f;

	XMMATRIX mtxViewRotationOnly = XMLoadFloat4x4(&mtxView);
	XMMATRIX mtxViewInverse = XMMatrixInverse(nullptr, mtxViewRotationOnly);

	XMMATRIX mtxScale = XMMatrixScaling(scale.x, scale.y, 1.0f);
	XMMATRIX mtxPivot = XMMatrixTranslation(-pivot.x, -pivot.y, 0.0f);

	XMMATRIX mtxWorldFinal = mtxScale * mtxPivot * mtxViewInverse * mtxWorld;

	ShaderBillboard_SetWorldMatrix(mtxWorldFinal);

	// 深度書き込み無効にして描画（これでビルボードが奥のオブジェクトを消さない）
	if (g_pDepthNoWrite) {
		ctx->OMSetDepthStencilState(g_pDepthNoWrite, prevRef);
	}

	ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	Tecture_SetTexture(texId);
	ShaderBillboard_SetColor(color);

	ctx->Draw(NUM_VERTEX, 0);

	// 深度ステート復元
	ctx->OMSetDepthStencilState(prevDSS, prevRef);
	if (prevDSS) prevDSS->Release();

	// --- 復元 ---
	RestoreCommonPipelineState(ctx, prevVS, prevPS, prevIL, prevVSCB, prevPSCB, prevSRV, prevSamp);
}

void Billboard_Draw(int texid, const DirectX::XMMATRIX& mtxWorld, const DirectX::XMFLOAT2& scal, const DirectX::XMFLOAT4& color)
{
	Billboard_Draw(texid, mtxWorld, DirectX::XMFLOAT2(1.0f,1.0f), DirectX::XMFLOAT2(0.0f,0.0f), scal, color, DirectX::XMFLOAT2(0.0f,0.0f));
}

void Billboard_Draw(
	int texid,
	const DirectX::XMMATRIX& mtxWorld,
	const DirectX::XMFLOAT4& color
)
{
	Billboard_Draw(texid, mtxWorld, DirectX::XMFLOAT2(1.0f,1.0f), DirectX::XMFLOAT2(0.0f,0.0f), DirectX::XMFLOAT2(1.0f,1.0f), color, DirectX::XMFLOAT2(0.0f,0.0f));
}
