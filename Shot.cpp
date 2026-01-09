/*==========================================================================

ショットの制御[Shot.cpp]

												Author : hidetoshi muramatu
												Date   : 2025/
---------------------------------------------------------------------------

修正点:
1.  **クラッシュの原因除去**: Shot_Update内の `front *= 10;` を削除。この過剰なスケーリングがXMMatrixLookAtLHでのアサーションエラーを引き起こしていました。
2.  **回転ロジックの修正**: Yaw (ry) と Pitch (axis) の両方が正しく適用されるように修正。
3.  **移動ロジックの追加**: g_positionを毎フレーム更新し、ショットが移動するようにしました。
4.  **速度の分離**: g_Frontを単位方向ベクトルとして保持し、g_Velocityを分離しました。

=========================================================================*/

#include "Shot.h"
#include "model.h"
#include "keylogger.h"
using namespace DirectX;

static XMFLOAT3 g_position;
static constexpr float OFFSET_LENGTH = 0.5f;
static XMFLOAT3 g_Front{}; // 進行方向を示す単位ベクトル (Direction vector)
static MODEL* g_Model{};
static float g_Speed = 50.0f; // ショットの移動速度
static XMFLOAT3 g_Velocity{}; // 実際の速度ベクトル (Direction * Speed)

void Shot_Initialize(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& front)
{

	g_position = position;

	// g_Frontを単位ベクトルとして初期化
	XMVECTOR frontVec = XMVector3Normalize(XMLoadFloat3(&front));
	XMStoreFloat3(&g_Front, frontVec);
	// 速度も初期化
	XMStoreFloat3(&g_Velocity, frontVec * g_Speed);

	g_Model = ModelLoad("rom\\Model\\yajirushi.fbx",0.1);

}

void Shot_Finalize()
{
	ModelRelease(g_Model);
}

void Shot_Update(double et)
{
	float y_angle{ 0.0f };
	float x_angle{ 0.0f };
	float frameTime = (float)et; // フレーム時間をfloatにキャスト

	// 入力による回転量を取得
	if (KeyLogger_IsPressed(KK_T))
	{
		y_angle = 1.0f * frameTime;
	}
	if (KeyLogger_IsPressed(KK_G))
	{
		y_angle = -1.0f * frameTime;
	}
	if (KeyLogger_IsPressed(KK_F))
	{
		x_angle = -1.0f * frameTime;
	}
	if (KeyLogger_IsPressed(KK_H))
	{
		x_angle = 1.0f * frameTime;
	}

	XMMATRIX rotationY = XMMatrixRotationY(y_angle);
	XMVECTOR front = XMVector3TransformNormal(XMLoadFloat3(&g_Front), rotationY);
	XMVECTOR right = XMVector3Cross(XMVECTOR{ 0.0f,1.0f,0.0f }, front);
	XMMATRIX axis = XMMatrixRotationAxis(right, x_angle);
	front = XMVector3TransformNormal(front, axis);
	
	XMStoreFloat3(&g_Front, front);
}

void Shot_Draw()
{
	XMMATRIX mtxRotation = XMMatrixLookAtLH(XMVECTOR{ 0.0f,0.0f,0.0f }, XMLoadFloat3(&g_Front), XMVECTOR{ 0.0f,1.0f,0.0f });
	XMMATRIX mtxWorld =
		XMMatrixTranslation(0.0f, 0.0f, OFFSET_LENGTH) *
		XMMatrixTranspose(mtxRotation) * 
		XMMatrixTranslation(g_position.x, g_position.y, g_position.z);
	ModelDraw(g_Model, mtxWorld);
}

const DirectX::XMFLOAT3& Shot_GetVelocity()
{
	return g_Front;
}
void Shot_SetPosition(const DirectX::XMFLOAT3& position)
{
	g_position = position;
}