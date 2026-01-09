/*==========================================================================

ビルボードの表示

												Author : hidetoshi muramatu
												Date   : 2025/12/04
---------------------------------------------------------------------------






=========================================================================*/

#pragma once
#include <d3d11.h>
#include <DirectXMath.h>  // ← 必ず追加！


void Billboard_Initialize();
void Billboard_Finalize();
void Billboard_Update(double elapsedTime);
void Billboard_SetViewMatrix(const DirectX::XMMATRIX& mtxWorld);

//void Billboard_Draw(
//	int texid,const DirectX::XMMATRIX& mtxWorld,
//	const DirectX::XMFLOAT2& scale,
//	const DirectX::XMFLOAT2& pivot,
//	const DirectX::XMFLOAT4& color = { 1.0,1.0,1.0,1.0 }
//	 
//);

void Billboard_Draw(
	int texId, const DirectX::XMMATRIX& mtxWorld,
	const DirectX::XMFLOAT2& scale,
	const DirectX::XMFLOAT2& pivot,
	const DirectX::XMFLOAT4& color = { 1.0,1.0,1.0,1.0 }
);

void Billboard_Draw(
	int texId,
	const DirectX::XMMATRIX& mtxWorld,
	const DirectX::XMFLOAT2& uv_offset,
	const DirectX::XMFLOAT2& uv_size,
	const DirectX::XMFLOAT2& scale,
	const DirectX::XMFLOAT4& color,
	const DirectX::XMFLOAT2& pivot
);













