/*==========================================================================

オーラエフェクト実装

                                                Author : hidetoshi muramatu
                                                Date   : 2025/01/11
---------------------------------------------------------------------------

=========================================================================*/

#include "AuraEffect.h"
#include "BillBord.h"
#include "texture.h"
#include <cmath>

using namespace DirectX;

static double g_totalTime = 0.0;

// デフォルトパラメータ
AuraEffectParams AuraEffect_GetDefaultParams()
{
    AuraEffectParams params{};
    params.textureId = Texture_Load(L"rom\\Texture\\gra_effect_lightA.png");
    params.innerColor = { 1.0f, 1.0f, 1.0f, 0.8f };
    params.outerColor = { 0.5f, 0.5f, 1.0f, 0.3f };
    params.baseScale = 5.f;
    params.pulseSpeed = 2.0f;
    params.pulseAmount = 0.2f;
    params.rotationSpeed = 0.5f;
    params.layerCount = 2;
    params.layerScaleOffset = 0.3f;
    params.layerAlphaFalloff = 0.5f;
    params.useAdditiveBlending = true;

    // パーティクルモードのデフォルト
    params.useParticleMode = false;
    params.particleCount = 20;
    params.particleRadius = 1.0f;
    params.particleSpeed = 1.0f;
    params.particleScale = 0.5f;
    params.particleRandomness = 0.3f;
    params.particleVerticalSpeed = 1.0f;
    params.particleVerticalRange = 2.0f;
    params.particleRotationSpeed = 2.0f;

    return params;
}

void AuraEffect_Initialize()
{
    g_totalTime = 0.0;
}

void AuraEffect_Finalize()
{
}

void AuraEffect_Update(double elapsedTime)
{
    g_totalTime += elapsedTime;
}

void AuraEffect_Draw(
    const AuraEffectParams& params,
    const DirectX::XMMATRIX& mtxWorld)
{
    using namespace DirectX;

    // NOTE: Billboard_SetViewMatrix()は呼び出し側で事前に設定しておく必要があります

    // 複数レイヤーの描画
    for (int i = 0; i < params.layerCount; ++i)
    {
        float layerProgress = static_cast<float>(i) / static_cast<float>(params.layerCount);

        // 脈動アニメーション
        float pulsePhase = static_cast<float>(g_totalTime * params.pulseSpeed + i * 0.5);
        float pulse = 1.0f + std::sinf(pulsePhase) * params.pulseAmount;

        // 回転アニメーション
        float rotation = static_cast<float>(g_totalTime * params.rotationSpeed * (i % 2 == 0 ? 1.0f : -1.0f));

        // スケール計算
        float scale = params.baseScale * (1.0f + layerProgress * params.layerScaleOffset) * pulse;

        // 色の補間
        XMFLOAT4 layerColor;
        layerColor.x = params.innerColor.x + (params.outerColor.x - params.innerColor.x) * layerProgress;
        layerColor.y = params.innerColor.y + (params.outerColor.y - params.innerColor.y) * layerProgress;
        layerColor.z = params.innerColor.z + (params.outerColor.z - params.innerColor.z) * layerProgress;
        layerColor.w = (params.innerColor.w + (params.outerColor.w - params.innerColor.w) * layerProgress)
            * std::powf(params.layerAlphaFalloff, static_cast<float>(i));

        // 回転行列を適用したワールドマトリックス
        XMMATRIX mtxRotation = XMMatrixRotationZ(rotation);
        XMMATRIX mtxWorldWithRotation = mtxRotation * mtxWorld;

        // テクスチャ全体を使用するため、オフセット(0,0)とサイズ(テクスチャ全体)を指定
        // ピボットは中央(0,0)で回転
        Billboard_Draw(
            params.textureId,
            mtxWorldWithRotation,
            XMFLOAT2(0.0f, 0.0f),           // UV offset
            XMFLOAT2(static_cast<float>(Texture_GetWidht(params.textureId)),
                static_cast<float>(Texture_GetHeight(params.textureId))), // UV size
            XMFLOAT2(scale, scale),          // scale
            layerColor,                      // color
            XMFLOAT2(0.0f, 0.0f)            // pivot
        );
    }
}

// プリセット: 炎のオーラ
AuraEffectParams AuraEffect_PresetFire()
{
    AuraEffectParams params = AuraEffect_GetDefaultParams();
    params.innerColor = { 1.0f, 0.9f, 0.3f, 0.9f };    // 明るい黄色
    params.outerColor = { 1.0f, 0.2f, 0.0f, 0.2f };    // 赤
    params.baseScale = 1.5f;
    params.pulseSpeed = 3.0f;
    params.pulseAmount = 0.3f;
    params.rotationSpeed = 0.8f;
    params.layerCount = 3;
    params.layerScaleOffset = 0.4f;
    params.layerAlphaFalloff = 0.6f;
    params.useAdditiveBlending = true;
    return params;
}

// プリセット: 氷のオーラ
AuraEffectParams AuraEffect_PresetIce()
{
    AuraEffectParams params = AuraEffect_GetDefaultParams();
    params.innerColor = { 0.7f, 0.9f, 1.0f, 0.8f };    // 明るい青
    params.outerColor = { 0.2f, 0.4f, 0.8f, 0.3f };    // 濃い青
    params.baseScale = 1.3f;
    params.pulseSpeed = 1.5f;
    params.pulseAmount = 0.15f;
    params.rotationSpeed = -0.3f;
    params.layerCount = 2;
    params.layerScaleOffset = 0.25f;
    params.layerAlphaFalloff = 0.7f;
    params.useAdditiveBlending = true;
    return params;
}

// プリセット: 電気のオーラ
AuraEffectParams AuraEffect_PresetElectric()
{
    AuraEffectParams params = AuraEffect_GetDefaultParams();
    params.innerColor = { 1.0f, 1.0f, 1.0f, 1.0f };    // 白
    params.outerColor = { 0.3f, 0.5f, 1.0f, 0.4f };    // 青紫
    params.baseScale = 1.2f;
    params.pulseSpeed = 5.0f;
    params.pulseAmount = 0.4f;
    params.rotationSpeed = 1.5f;
    params.layerCount = 4;
    params.layerScaleOffset = 0.2f;
    params.layerAlphaFalloff = 0.5f;
    params.useAdditiveBlending = true;
    return params;
}

// プリセット: 神聖なオーラ
AuraEffectParams AuraEffect_PresetHoly()
{
    AuraEffectParams params = AuraEffect_GetDefaultParams();
    params.innerColor = { 1.0f, 1.0f, 0.9f, 0.9f };    // 温かい白
    params.outerColor = { 1.0f, 0.9f, 0.5f, 0.3f };    // ゴールド
    params.baseScale = 1.4f;
    params.pulseSpeed = 1.0f;
    params.pulseAmount = 0.2f;
    params.rotationSpeed = 0.2f;
    params.layerCount = 3;
    params.layerScaleOffset = 0.3f;
    params.layerAlphaFalloff = 0.65f;
    params.useAdditiveBlending = true;
    return params;
}

// プリセット: 暗黒のオーラ
AuraEffectParams AuraEffect_PresetDark()
{
    AuraEffectParams params = AuraEffect_GetDefaultParams();
    params.innerColor = { 0.5f, 0.0f, 0.5f, 0.8f };    // 紫
    params.outerColor = { 0.1f, 0.0f, 0.2f, 0.4f };    // 濃い紫
    params.baseScale = 1.6f;
    params.pulseSpeed = 2.5f;
    params.pulseAmount = 0.35f;
    params.rotationSpeed = -0.6f;
    params.layerCount = 3;
    params.layerScaleOffset = 0.35f;
    params.layerAlphaFalloff = 0.55f;
    params.useAdditiveBlending = false;
    return params;
}

// ========== パーティクルモードプリセット ==========

// 炎の粒子群
AuraEffectParams AuraEffect_PresetFireParticles()
{
    AuraEffectParams params = AuraEffect_GetDefaultParams();
    params.innerColor = { 1.0f, 0.9f, 0.3f, 1.0f };
    params.outerColor = { 1.0f, 0.2f, 0.0f, 0.3f };
    params.useAdditiveBlending = true;

    // パーティクルモード設定
    params.useParticleMode = true;
    params.particleCount = 40;
    params.particleRadius = 1.5f;
    params.particleSpeed = 2.0f;
    params.particleScale = 0.4f;
    params.particleRandomness = 0.5f;
    params.pulseSpeed = 4.0f;
    params.pulseAmount = 0.3f;

    return params;
}

// 星空エフェクト
AuraEffectParams AuraEffect_PresetStarfield()
{
    AuraEffectParams params = AuraEffect_GetDefaultParams();
    params.innerColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    params.outerColor = { 0.7f, 0.8f, 1.0f, 0.5f };
    params.useAdditiveBlending = true;

    params.useParticleMode = true;
    params.particleCount = 30;
    params.particleRadius = 2.0f;
    params.particleSpeed = 0.3f;
    params.particleScale = 0.2f;
    params.particleRandomness = 0.8f;
    params.pulseSpeed = 3.0f;
    params.pulseAmount = 0.5f;

    return params;
}

// エネルギー球体
AuraEffectParams AuraEffect_PresetEnergyOrb()
{
    AuraEffectParams params = AuraEffect_GetDefaultParams();
    params.innerColor = { 0.3f, 0.8f, 1.0f, 0.9f };
    params.outerColor = { 0.1f, 0.3f, 1.0f, 0.4f };
    params.useAdditiveBlending = true;

    params.useParticleMode = true;
    params.particleCount = 50;
    params.particleRadius = 1.2f;
    params.particleSpeed = 1.5f;
    params.particleScale = 0.3f;
    params.particleRandomness = 0.2f;
    params.pulseSpeed = 2.0f;
    params.pulseAmount = 0.2f;

    return params;
}

// 魔法陣風
AuraEffectParams AuraEffect_PresetMagicCircle()
{
    AuraEffectParams params = AuraEffect_GetDefaultParams();
    params.innerColor = { 0.8f, 0.2f, 1.0f, 1.0f };
    params.outerColor = { 0.4f, 0.1f, 0.8f, 0.3f };
    params.useAdditiveBlending = true;

    params.useParticleMode = true;
    params.particleCount = 60;
    params.particleRadius = 2.5f;
    params.particleSpeed = 0.8f;
    params.particleScale = 0.25f;
    params.particleRandomness = 0.1f;  // 整列させる
    params.pulseSpeed = 1.5f;
    params.pulseAmount = 0.15f;

    return params;
}