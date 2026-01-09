/*==========================================================================

トレイル(ポリゴン)の表示[trail.h]

												Author : hidetoshi muramatu
												Date   : 2025/12/18
---------------------------------------------------------------------------






=========================================================================*/

#pragma once
#include <DirectXMath.h>

void Trail_Initialize();
void Trail_Finalize();
void Trail_Draw();
void Trail_AddPosition(const DirectX::XMFLOAT3& position);
void Trail_SetCameraPosition(const DirectX::XMFLOAT3& position);
/*Tailを描画するしないの関数を追加して判断*/

