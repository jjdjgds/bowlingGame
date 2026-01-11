#include "Shot.h"
#include "model.h"
#include "mouse.h"
#include "keylogger.h"
#include "Mouselogger.h"
#include "debug_ostream.h"
#include "sprite.h"
#include "texture.h"

using namespace DirectX;

static XMFLOAT3 g_position;
static constexpr float OFFSET_LENGTH = 0.5f;

static XMFLOAT3 g_Front{ 0,0,1 };   // 単位方向
static MODEL* g_Model{};

static float g_Power = 0.0f;

// ===== マウススイング用 =====
static POINT g_PrevMousePos{};
static bool  g_IsSwinging = false;
static float g_SwingAccum = 0.0f;
// 定数
static constexpr float DIR_SMOOTH = 0.15f;
static float g_ChargeTimer = 0.0f;
static int   g_HitCount = 0;

static constexpr float CHARGE_LIMIT = 3.0f; // 2秒

static float g_AimTime = 0.0f;
static constexpr float SWING_SPEED = 3.0f;   // 揺れ速度
static constexpr float MAX_ANGLE = XM_PIDIV4; // ±45°
static float g_AimAngle = 0.0f;
static constexpr float BALL_RADIUS = -0.2f;
static bool g_IsCharging = false;
static int g_PowerBarTex = -1; // 白1pxテクスチャなど
static constexpr int   MAX_HIT_COUNT = 10;   // ★ 旧: 実質3～4 → 明示的に増やす
static constexpr float MAX_POWER = 100.0f;
static constexpr float POWER_PER_HIT = MAX_POWER / MAX_HIT_COUNT;

float Shot_GetPower()
{
    return g_Power;
}
bool Shot_IsFired()
{
    return g_State == ShotState::Fired;
}

XMFLOAT3 Shot_GetShotVelocity()
{
    return {
        g_Front.x * g_Power,
        g_Front.y * g_Power,
        g_Front.z * g_Power
    };
}

void Shot_ResetPower()
{
    g_Power = 0.0f;
    g_HitCount = 0;
    g_ChargeTimer = 0.0f;
    g_AimTime = 0.0f;
    g_Front = { 0,0,1 };
    g_IsCharging = false;   // ★追加
    g_State = ShotState::Charge;
}



void Shot_Initialize(const XMFLOAT3& position, const XMFLOAT3& front)
{
    g_position = position;
    XMStoreFloat3(&g_Front, XMVector3Normalize(XMLoadFloat3(&front)));
    g_Model = ModelLoad("rom\\Model\\yajirushi.fbx", 0.1f);
    g_PowerBarTex = Texture_Load(L"rom/white_8x8.png");

    GetCursorPos(&g_PrevMousePos);
}

void Shot_Finalize()
{
    ModelRelease(g_Model);
}

void Shot_Update(double et)
{
    POINT cur;
    GetCursorPos(&cur);

    float dx = float(cur.x - g_PrevMousePos.x);
    float dy = float(cur.y - g_PrevMousePos.y);
    g_PrevMousePos = cur;

    if (g_State == ShotState::Charge)
    {
        // 最初のPキーでチャージ開始
        if (KeyLogger_IsTrigger(KK_P))
        {
            if (!g_IsCharging)
            {
                g_IsCharging = true;
                g_ChargeTimer = 0.0f;   // 念のため
                g_HitCount = 0;
            }

            g_HitCount++;
        }

        // チャージ中だけ時間を進める
        if (g_IsCharging)
        {
            g_ChargeTimer += (float)et;

            if (g_ChargeTimer >= CHARGE_LIMIT)
            {
                g_Power = Clamp(
                    g_HitCount * POWER_PER_HIT,
                    5.0f,
                    MAX_POWER
                );

                g_IsCharging = false;
                g_State = ShotState::Aim;
            }

        }
    }

    if (g_State == ShotState::Aim)
    {
        g_AimTime += (float)et;

        g_AimAngle = sinf(g_AimTime * SWING_SPEED) * MAX_ANGLE;

        XMMATRIX rotY = XMMatrixRotationY(g_AimAngle);
        XMVECTOR base = XMVectorSet(0, 0, 1, 0);

        XMVECTOR dir = XMVector3TransformNormal(base, rotY);
        XMStoreFloat3(&g_Front, XMVector3Normalize(dir));

        if (KeyLogger_IsTrigger(KK_O))
        {
            g_State = ShotState::Fired;
        }
    }



}

float Clamp(float v, float minV, float maxV)
{
    if (v < minV) return minV;
    if (v > maxV) return maxV;
    return v;
}
void Shot_Draw()
{
    if (g_State != ShotState::Aim)
        return;

    // 向き（Yaw回転のみ）
    XMMATRIX rot = XMMatrixRotationY(g_AimAngle);

    // ローカル前方にオフセット
    XMVECTOR offset =
        XMVector3TransformCoord(
            XMVectorSet(0, 0, OFFSET_LENGTH, 0),
            rot
        );

    XMFLOAT3 ofs;
    XMStoreFloat3(&ofs, offset);

    // ワールド行列
    XMMATRIX world =
        rot *
        XMMatrixTranslation(
            g_position.x + ofs.x,
            g_position.y - BALL_RADIUS,
            g_position.z + ofs.z
        );


    ModelDraw(g_Model, world);
}



const XMFLOAT3& Shot_GetVelocity()
{
    return g_Front;
}

void Shot_SetPosition(const XMFLOAT3& position)
{
    g_position = position;
}

float Shot_GetChargeRatio()
{
    if (!g_IsCharging) return 0.0f;

    return Clamp(
        (float)g_HitCount / (float)MAX_HIT_COUNT,
        0.0f, 1.0f
    );
}

bool Shot_IsCharging()
{
    return g_IsCharging;
}

void Shot_DrawUI()
{
    if (!Shot_IsCharging())
        return;

    float ratio = Shot_GetChargeRatio();

    // ===== 円形ゲージ設定 =====
    const float cx = 150.0f;
    const float cy = 600.0f;
    const float radius = 60.0f;
    const float thickness = 14.0f;   // ★ 太さ
    const int   SEGMENTS = 36;        // 円の分割数

    int drawCount = (int)(SEGMENTS * ratio);

    for (int i = 0; i < drawCount; ++i)
    {
        float t = (float)i / (float)SEGMENTS;
        float angle = t * XM_2PI - XM_PIDIV2; // 上から開始

        float x = cx + cosf(angle) * radius;
        float y = cy + sinf(angle) * radius;

        Sprite_Draw(
            g_PowerBarTex,
            angle,
            x, y,
            thickness,  // 幅
            thickness * 2.5f, // ★ 高さ（＝太さ感）
            0, 0, 8, 8,
            {
                1.0f,
                0.3f + 0.7f * ratio, // チャージで色変化
                0.1f,
                1.0f
            }
        );
    }

    // ---- 背景円（薄く） ----
    for (int i = drawCount; i < SEGMENTS; ++i)
    {
        float t = (float)i / (float)SEGMENTS;
        float angle = t * XM_2PI - XM_PIDIV2;

        float x = cx + cosf(angle) * radius;
        float y = cy + sinf(angle) * radius;

        Sprite_Draw(
            g_PowerBarTex,
            angle,
            x, y,
            thickness,
            thickness * 2.5f,
            0, 0, 8, 8,
            { 0.2f, 0.2f, 0.2f, 0.4f }
        );
    }
}
