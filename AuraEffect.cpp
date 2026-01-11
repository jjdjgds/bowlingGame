/*==========================================================================

オーラエフェクト実装

                                                Author : hidetoshi muramatu
                                                Date   : 2025/01/11
---------------------------------------------------------------------------

=========================================================================*/

#include "AuraEffect.h"
#include "BillBord.h"
#include "ShaderBillBoard.h"
#include "texture.h"
#include "direct3d.h"
#include <cmath>

static double g_totalTime = 0.0;

// ブレンドステート
static ID3D11BlendState* g_pBlendAdd = nullptr;
static ID3D11BlendState* g_pBlendAlpha = nullptr;

AuraEffectParams AuraEffect_GetDefaultParams()
{
    AuraEffectParams params{};
    params.textureId = Texture_Load(L"rom\\Texture\\gra_effect_lightA.png");
    params.innerColor = { 1.0f, 1.0f, 1.0f, 0.8f };
    params.outerColor = { 0.5f, 0.5f, 1.0f, 0.3f };
    params.baseScale = 1.0f;
    params.pulseSpeed = 2.0f;
    params.pulseAmount = 0.2f;
    params.rotationSpeed = 0.5f;
    params.layerCount = 2;
    params.layerScaleOffset = 0.3f;
    params.layerAlphaFalloff = 0.5f;
    params.useAdditiveBlending = true;
    return params;
}

void AuraEffect_Initialize()
{
    g_totalTime = 0.0;

    ID3D11Device* dev = Direct3D_GetDevice();
    if (!dev) return;

    // 加算ブレンド
    D3D11_BLEND_DESC descAdd = {};
    descAdd.AlphaToCoverageEnable = FALSE;
    descAdd.IndependentBlendEnable = FALSE;
    descAdd.RenderTarget[0].BlendEnable = TRUE;
    descAdd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    descAdd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    descAdd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    descAdd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    descAdd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    descAdd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    descAdd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    dev->CreateBlendState(&descAdd, &g_pBlendAdd);

    // ストレートアルファブレンド
    D3D11_BLEND_DESC descAlpha = {};
    descAlpha.AlphaToCoverageEnable = FALSE;
    descAlpha.IndependentBlendEnable = FALSE;
    descAlpha.RenderTarget[0].BlendEnable = TRUE;
    descAlpha.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    descAlpha.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    descAlpha.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    descAlpha.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    descAlpha.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    descAlpha.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    descAlpha.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    dev->CreateBlendState(&descAlpha, &g_pBlendAlpha);
}

void AuraEffect_Finalize()
{
    if (g_pBlendAdd) { g_pBlendAdd->Release(); g_pBlendAdd = nullptr; }
    if (g_pBlendAlpha) { g_pBlendAlpha->Release(); g_pBlendAlpha = nullptr; }
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

    ID3D11DeviceContext* ctx = Direct3D_GetContext();
    if (!ctx) return;

    // 元のブレンドステート保存
    ID3D11BlendState* prevBlend = nullptr;
    FLOAT prevFactor[4] = { 0,0,0,0 };
    UINT prevMask = 0xffffffff;
    ctx->OMGetBlendState(&prevBlend, prevFactor, &prevMask);

    // 適切なブレンドを設定
    if (params.useAdditiveBlending && g_pBlendAdd) {
        FLOAT bf[4] = { 1,1,1,1 };
        ctx->OMSetBlendState(g_pBlendAdd, bf, 0xffffffff);
    }
    else if (g_pBlendAlpha) {
        FLOAT bf[4] = { 1,1,1,1 };
        ctx->OMSetBlendState(g_pBlendAlpha, bf, 0xffffffff);
    }

    // 複数レイヤーの描画
    for (int i = 0; i < params.layerCount; ++i)
    {
        float layerProgress = static_cast<float>(i) / static_cast<float>(params.layerCount);

        // 脈動アニメーション
        float pulsePhase = static_cast<float>(g_totalTime * params.pulseSpeed + i * 0.5f);
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

        // ビルボード描画（UV全体、ピボット中心）
        Billboard_Draw(
            params.textureId,
            mtxWorldWithRotation,
            XMFLOAT2(0.0f, 0.0f),           // UV offset
            XMFLOAT2(static_cast<float>(Texture_GetWidht(params.textureId)),
                static_cast<float>(Texture_GetHeight(params.textureId))), // UV size
            XMFLOAT2(scale, scale),          // scale
            layerColor,                      // color
            XMFLOAT2(0.0f, 0.0f)             // pivot
        );
    }

    // 元のブレンドステートに戻す
    ctx->OMSetBlendState(prevBlend, prevFactor, prevMask);
    if (prevBlend) prevBlend->Release();
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