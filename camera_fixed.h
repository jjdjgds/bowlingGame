/*==========================================================================

Camera_fixed.h

												Author : hidetoshi muramatu
												Date   : 2025/12/16
---------------------------------------------------------------------------






=========================================================================*/

#pragma once
/*==========================================================================


Shotカメラインターフェイス[Shotcamera.h]
												Author : hidetoshi muramatu
												Date   : 2025/10/28
---------------------------------------------------------------------------






=========================================================================*/

#pragma once
#include <DirectXMath.h>
#include "camera.h"
#include "Collision.h"


class FixedCamera : public Camera
{

private:

	AABB m_aabb;
	//DirectX::XMFLOAT3 m_pos{};

	DirectX::XMFLOAT3 m_target{};
	float m_distance{};

public:
	FixedCamera(const DirectX::XMFLOAT3& position, const AABB& aabb)
		:/*m_pos(position)*/m_aabb(aabb)
	{
		SetPosition(position);
	}
	//virtual ~Camera() = default;
	//void Update(double elapsed_time)override;//キー入力待ち
	void SetMatrix() override;
	bool IsOverlap();
	void SetTarget(const DirectX::XMFLOAT3& target) {
		m_target = target;
	}
	//DirectX::XMFLOAT3 GetFront()const { return m_Front; }
};

