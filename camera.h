/*==========================================================================


カメラインターフェイス[camera.h]
												Author : hidetoshi muramatu
												Date   : 2025/10/28
---------------------------------------------------------------------------






=========================================================================*/

#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
class Camera
{
private:
	static ID3D11Buffer* m_pVSConstantBuffer1;
	static ID3D11Buffer* m_pVSConstantBuffer2;
	static int m_ReferenceCount;
	DirectX::XMMATRIX m_viewMatrix{};
	DirectX::XMFLOAT3 m_Pos{};
public:
	Camera();
	virtual ~Camera();

	virtual void Update(double elapsed_time){};
	virtual void SetMatrix() = 0;
	
	const DirectX::XMMATRIX& GetViewMatrix() const { return m_viewMatrix; }
	const DirectX::XMFLOAT3& GetPosition()const { return m_Pos; }
protected:
	DirectX::XMFLOAT3& GetPositionReference() { return m_Pos; }
	void SetProjectionMatrix(const DirectX::XMMATRIX& matrix);
	void SetViewMatrix(const DirectX::XMMATRIX& matrix);
	void SetPosition(const DirectX::XMFLOAT3& position) { m_Pos = position; };
};