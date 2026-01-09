#include "Camera.h"
#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;

// ★ static メンバの定義（絶対必要）
ID3D11Buffer* Camera::m_pVSConstantBuffer1 = nullptr;
ID3D11Buffer* Camera::m_pVSConstantBuffer2 = nullptr;
int Camera::m_ReferenceCount = 0;


Camera::Camera()
{
	if(!m_pVSConstantBuffer1)
	{
		// 頂点シェーダー用定数バッファの作成
		D3D11_BUFFER_DESC buffer_desc{};
		buffer_desc.ByteWidth = sizeof(XMFLOAT4X4); // バッファのサイズ
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // バインドフラグ

		Direct3D_GetDevice()->CreateBuffer(&buffer_desc, nullptr, &m_pVSConstantBuffer1);
		Direct3D_GetDevice()->CreateBuffer(&buffer_desc, nullptr, &m_pVSConstantBuffer2);
	}

	m_ReferenceCount++;

}

Camera::~Camera()
{
	m_ReferenceCount--;



	if(m_ReferenceCount ==0)
	{
		SAFE_RELEASE(m_pVSConstantBuffer2);
		SAFE_RELEASE(m_pVSConstantBuffer1);
	}

	
}


void Camera::SetProjectionMatrix(const DirectX::XMMATRIX& matrix)
{
	// 定数バッファ格納用行列の構造体を定義
	XMFLOAT4X4 transpose;

	// 行列を転置して定数バッファ格納用行列に変換
	//転置してる
	XMStoreFloat4x4(&transpose, XMMatrixTranspose(matrix));

	// 定数バッファに行列をセット
	Direct3D_GetContext()->UpdateSubresource(m_pVSConstantBuffer2, 0, nullptr, &transpose, 0, 0);
	Direct3D_GetContext()->VSSetConstantBuffers(2, 1, &m_pVSConstantBuffer2);
}

void Camera::SetViewMatrix(const DirectX::XMMATRIX& matrix)
{

	m_viewMatrix = matrix;

	// 定数バッファ格納用行列の構造体を定義
	XMFLOAT4X4 transpose;

	// 行列を転置して定数バッファ格納用行列に変換
	XMStoreFloat4x4(&transpose, XMMatrixTranspose(matrix));

	// 定数バッファに行列をセット
	Direct3D_GetContext()->UpdateSubresource(m_pVSConstantBuffer1, 0, nullptr, &transpose, 0, 0);
	Direct3D_GetContext()->VSSetConstantBuffers(1, 1, &m_pVSConstantBuffer1);


}

