
/*==========================================================================

É{Å[ÉãÇÃêßå‰[ball.h]

												Author : hidetoshi muramatu
												Date   : 2025/11/20
---------------------------------------------------------------------------






=========================================================================*/
#pragma once
#include "DirectXMath.h"
#include "Collision.h"

void Ball_Initialize(const DirectX::XMFLOAT3& position);
void Ball_Finalize();
void Ball_Update(double et);
void Ball_Draw();
bool Ball_IsStationary();
DirectX::XMFLOAT3 Ball_GetPosition();
void BallShot(const DirectX::XMFLOAT3& velocity);
const AABB& Ball_GetAABB();