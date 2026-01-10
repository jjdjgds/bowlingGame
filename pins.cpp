#include "pins.h"
#include "cube.h"
#include "DebugDraw.h"
using namespace DirectX;

static constexpr float GRAVITY = -9.8f;
static constexpr float FLOOR_Y = 0.0f;

Pins::Pins()
    : m_position{ 0,0,0 }
    , m_rotation{ 0,0,0 }
    , m_velocity{ 0,0,0 }
    , m_angularVelocity{ 0,0,0 }
    , m_isDown(false)
    , m_mass(1.0f)
{
	//m_Aabb = AABB::Make(m_position, { 0.5f,0.5f,0.5f });
}

void Pins::Initialize(const XMFLOAT3& pos)
{
    m_position = pos;
    m_rotation = { 0,0,0 };
    m_velocity = { 0,0,0 };
    m_scale = { 1,1,1 };
    m_angularVelocity = { 0,0,0 };
    m_isDown = false;

    m_model = ModelLoad("rom\\Model\\Pins.fbx");

    // ★ モデル基準サイズ（スケール1.0時）
    m_baseHalf = { 1.5f, 3.f, 1.5f };

    // 初期AABB生成
    m_Aabb = AABB::Make(m_position, m_baseHalf);
}



void Pins::Update(float deltaTime)
{
    // ===== 重力 =====
    m_velocity.y += GRAVITY * deltaTime;

    // ===== 位置更新 =====
    m_position.x += m_velocity.x * deltaTime;
    m_position.y += m_velocity.y * deltaTime;
    m_position.z += m_velocity.z * deltaTime;

    // ===== 回転更新 =====
    m_rotation.x += m_angularVelocity.x * deltaTime;
    m_rotation.y += m_angularVelocity.y * deltaTime;
    m_rotation.z += m_angularVelocity.z * deltaTime;

    // ===== 床 =====
    if (m_position.y < FLOOR_Y)
    {
        m_position.y = FLOOR_Y;
        m_velocity.y *= -0.2f;
        m_velocity.x *= 0.9f;
        m_velocity.z *= 0.9f;

        m_angularVelocity.x *= 0.9f;
        m_angularVelocity.z *= 0.9f;
    }

    // ===== 倒れ判定 =====
    if (fabsf(m_rotation.x) > XM_PIDIV4 || fabsf(m_rotation.z) > XM_PIDIV4)
    {
        m_isDown = true;
    }

    // ===== ★ AABBサイズ更新 =====
    XMFLOAT3 half =
    {
        m_baseHalf.x * m_scale.x,
        m_baseHalf.y * m_scale.y,
        m_baseHalf.z * m_scale.z
    };

    // 倒れたら横に広げる
    if (m_isDown)
    {
        half = { half.y, half.x, half.y };
    }

    // ★ AABB中心を下にずらす
    XMFLOAT3 aabbCenter = m_position;
    aabbCenter.y += half.y;   // ← ここが修正点

    m_Aabb.SetCenter(half);
    m_Aabb.Move(aabbCenter);

}

void Pins::Draw()
{
    // ★ 描画用オフセット（底合わせ）
    float drawOffsetY = m_baseHalf.y * m_scale.y;

    XMMATRIX mtxworld =
        XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) *
        XMMatrixRotationX(m_rotation.x) *
        XMMatrixRotationY(m_rotation.y) *
        XMMatrixRotationZ(m_rotation.z) *
        XMMatrixTranslation(
            m_position.x,
            m_position.y + drawOffsetY, // ← ここ重要
            m_position.z
        );

    Cube_Draw(mtxworld, 0);
    DebugDraw_AddAABB(m_Aabb, { 1,1,0,1 });
}

void Pins::Hit(const XMFLOAT3& impulse, const XMFLOAT3& hitPoint)
{
    // 線形インパルス
    m_velocity.x += impulse.x / m_mass;
    m_velocity.y += impulse.y / m_mass;
    m_velocity.z += impulse.z / m_mass;

    // 回転インパルス（簡易）
    XMVECTOR r = XMLoadFloat3(&hitPoint) - XMLoadFloat3(&m_position);
    XMVECTOR f = XMLoadFloat3(&impulse);

    XMVECTOR torque = XMVector3Cross(r, f);

    XMFLOAT3 t;
    XMStoreFloat3(&t, torque);

    m_angularVelocity.x += t.x * 0.05f;
    m_angularVelocity.y += t.y * 0.05f;
    m_angularVelocity.z += t.z * 0.05f;
}

bool Pins::IsDown() const
{
    return m_isDown;
}
