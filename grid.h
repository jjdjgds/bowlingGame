/*==========================================================================


grid‚Ì•`‰æ[grid.h]
												Author : hidetoshi muramatu
												Date   : 2025/10/21
---------------------------------------------------------------------------






=========================================================================*/


#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
void Gulid_Initialize(int x_count, int z_count, float size);
void Gulid_Finalize();
void Gulid_Update(double elapsedTime);
void Gulid_Draw();
void Gulid_Draw(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale);