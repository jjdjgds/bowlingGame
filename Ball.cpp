
/*==========================================================================
ボールの制御[ball.cpp]


												Author : hidetoshi muramatu
												Date   : 2025/11/20
---------------------------------------------------------------------------






=========================================================================*/

#include "Ball.h"
#include "model.h"
#include "direct3d.h"
#include "DirectXMath.h"
#include "shader3d.h"
#include "Map.h"
#include "keylogger.h"


using namespace DirectX;
static XMFLOAT3 g_PrevPosition{};
static MODEL* g_pBall{};
static XMFLOAT3 g_Pos{};
static XMFLOAT3 g_Vel{};
static AABB g_Aabb{};

static void BallHitChack();

void Ball_Initialize(const DirectX::XMFLOAT3& position)
{
	g_Pos = position;
	g_Vel = { 0.0f,0.0f,0.0f };
	
	g_pBall = ModelLoad("rom\\Model\\ball.fbx",0.1) ;
	g_Aabb = g_pBall->Aabb;
	g_Pos = g_PrevPosition = position;


	// ★コレだけで十分
	g_Aabb = AABB::Make(position, { 0.3f,0.3f,0.3f });
	//g_Aabb.Move(position);
}

void Ball_Finalize()
{

	ModelRelease(g_pBall);

}

void Ball_Update(double et)
{

	XMVECTOR position = XMLoadFloat3(&g_Pos);
	XMVECTOR velocity = XMLoadFloat3(&g_Vel);

	XMVECTOR dir{};
	/*if (KeyLogger_IsPressed(KK_I)) {
		dir += {0.0f, 0.0f, 1.0f};
	}
	if (KeyLogger_IsPressed(KK_J)) {
		dir += {-1.0f, 0.0f, 0.0f};

	}
	if (KeyLogger_IsPressed(KK_K)) {
		dir += {0.0f, 0.0f, -1.0f};

	}
	if (KeyLogger_IsPressed(KK_L)) {
		dir += {1.0f, 0.0f, 0.0f};

	}*/

	if (XMVectorGetX(XMVector3LengthSq(dir)) > 0.0f) {
		dir = XMVector3Normalize(dir) * 5.0f * (float)et;
		velocity += dir * 5.0f * (float)et; // 速度に加算
	}

	//重力によって落ちる
	const XMVECTOR G{ 0.0f,-9.8f,0.0f };
	velocity += G * 1.0f * (float)et;
	position += velocity * (float)et;
	g_Aabb.Move(position);

	for (int i = 0; i < MapGetBlockCount(); i++) {
		Hit hit =GetCollision(i).IsHit(g_Aabb);
		if (hit.IsHit()) {
			if (hit.GetNormal().y > 0.0f)
			{
				position = XMVectorSetY(position, GetCollision(i).GetMax().y + g_Aabb.GetHalfSize().y);
				g_Aabb.Move(position);
				velocity *= {1.0f, -0.8f, 1.0f}; //ボールの動きを止める バウンドも出来る　当たったブロックのタイプによって変えられる
			}
		}
	}
	//横に当たったときの当たり判定
	for (int i = 0; i < MapGetBlockCount(); i++) {
		Hit hit = GetCollision(i).IsHit(g_Aabb);
		if (hit.IsHit()) {
			if (hit.GetNormal().x < 0.0f)
			{
				position = XMVectorSetX(position, GetCollision(i).GetMin().x - g_Aabb.GetHalfSize().x);
				velocity *= {0.0f, 1.0f, 1.0f};
			}
			else if (hit.GetNormal().x > 0.0f) {
				position = XMVectorSetX(position, GetCollision(i).GetMin().x + g_Aabb.GetHalfSize().x);
				velocity *= {0.0f, 1.0f, 1.0f};
			}
			else if (hit.GetNormal().y < 0.0f) {
				position = XMVectorSetY(position, GetCollision(i).GetMin().y - g_Aabb.GetHalfSize().y);
				velocity *= {1.0f, 0.0f, 1.0f};
			}
			else if (hit.GetNormal().z < 0.0f) {
				position = XMVectorSetZ(position, GetCollision(i).GetMin().z - g_Aabb.GetHalfSize().z);
				velocity *= {1.0f, 1.0f, 0.0f};
			}
			else if (hit.GetNormal().z > 0.0f) {
				position = XMVectorSetZ(position, GetCollision(i).GetMin().z + g_Aabb.GetHalfSize().z);
				velocity *= {1.0f, 1.0f, 0.0f};
			}

			g_Aabb.Move(position);
		}
	}

	BallHitChack();
	g_PrevPosition = g_Pos;
	XMStoreFloat3(&g_Pos, position);
	XMStoreFloat3(&g_Vel, velocity);
}

void Ball_Draw()
{
	Shader3d_Begin();
	XMMATRIX mtxWorld = XMMatrixTranslation(g_Pos.x,g_Pos.y,g_Pos.z);
	ModelDraw(g_pBall, mtxWorld);
	

}

bool Ball_IsStationary()
{

	static constexpr float STATIONARY_THRESHOLD = 1e-4f;

	return XMVectorGetX(XMVector3Length(XMLoadFloat3(&g_PrevPosition) - XMLoadFloat3(&g_PrevPosition))) < STATIONARY_THRESHOLD;

}

DirectX::XMFLOAT3 Ball_GetPosition()
{
	return g_Pos;
}

void BallShot(const DirectX::XMFLOAT3& velocity)
{

	g_Vel.x += velocity.x;
	g_Vel.y += velocity.y*5;
	g_Vel.z += velocity.z;


}

const AABB& Ball_GetAABB()
{
	return g_Aabb;
	// TODO: return ステートメントをここに挿入します
}

void BallHitChack()
{

}
