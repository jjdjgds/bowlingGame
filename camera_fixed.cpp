
/*==========================================================================

å≈íËÉJÉÅÉâÇÃé¿ëï [camera_fixed.cpp]

												Author : hidetoshi muramatu
												Date   : 2025/12/16
---------------------------------------------------------------------------






=========================================================================*/

#include "camera_fixed.h"
#include "direct3d.h"
using namespace DirectX;
void FixedCamera::SetMatrix()
{


   
    XMMATRIX mtxView = XMMatrixLookAtLH(
        XMLoadFloat3(&GetPosition()),
        XMLoadFloat3(&m_target),
        XMVECTOR({ 0,1,0 }));

    SetViewMatrix(mtxView);

    float w = static_cast<float>(Direct3D_GetBackBufferWidth());
    float h = static_cast<float>(Direct3D_GetBackBufferHeight());

    XMMATRIX mtxProj = XMMatrixPerspectiveFovLH(1.0,w/h,0.1f,1000.0f);
    SetProjectionMatrix(mtxProj);
}

bool FixedCamera::IsOverlap()
{



    return m_aabb.IsOverlap({m_target});


}
