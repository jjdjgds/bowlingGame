/*==========================================================================


cubeÇÃï`âÊ[cube.h]
												Author : hidetoshi muramatu
												Date   : 2025/10/16
---------------------------------------------------------------------------






=========================================================================*/




#pragma once
#include <d3d11.h>
#include <DirectXMath.h>  // Å© ïKÇ∏í«â¡ÅI
using namespace DirectX;

void Cube_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Cube_Finalize();
void Cube_Update(double elapsedTime);
void Cube_Draw(const XMMATRIX& mtxWorld,int texid);

//Camera* GetCamera();