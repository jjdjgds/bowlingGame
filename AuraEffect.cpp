/*==========================================================================
オーラエフェクト
                                               Author : hidetoshi muramatu
                                               Date   : 2025/12/04
---------------------------------------------------------------------------
ビルボードを使用した多層オーラエフェクトシステム
=========================================================================*/

#include "AuraEffect.h"
#include "BillBord.h"
#include "direct3d.h"
#include <cmath>
#include <algorithm>

// エフェクト用の時間管理
static double g_time = 0.0;

// ブレンドステート保存用
static ID3D11BlendState* g_pBlendStateAdd = nullptr;
static ID3D11BlendState* g_pOriginalBlendState = nullptr;
static float g_originalBlendFactor[4] = { 0, 0, 0, 0 };
static UINT g_originalSampleMask = 0;

// オーラレイヤー構造体
struct AuraLayer
{
    float scale;           // スケール
    float rotationSpeed;   // 回転速度
    float pulseSpeed;      // 脈動速度
    float pulseAmount;     // 脈動量
    float alphaBase;       // 基本アルファ値
    float offsetY;         // Y軸オフセット
};

// 多層オーラの設定（内側から外側へ）
static const AuraLayer g_auraLayers[] =
{
    // 内側の明るいコア
    { 0.3f,  2.0f, 4.0f, 0.2f, 0.8f, 0.0f },
    { 0.5f,  1.5f, 3.0f, 0.15f, 0.6f, 0.05f },

    // 中間層
    { 0.8f, -1.0f, 2.5f, 0.25f, 0.4f, 0.1f },
    { 1.1f, -1.5f, 2.0f, 0.3f, 0.3f, 0.15f },

    // 外側の柔らかい層
    { 1.5f,  0.5f, 1.5f, 0.4f, 0.2f, 0.2f },
    { 2.0f, -0.3f, 1.0f, 0.5f, 0.1f, 0.25f },
};

static constexpr int NUM_LAYERS = sizeof(g_auraLayers) / sizeof(AuraLayer);

void AuraEffect_Initialize()
{
    g_time = 0.0;

    //// ブレンドステートを作成
    //D3D11_BLEND_DESC blendDesc = {};
    //blendDesc.AlphaToCoverageEnable = FALSE;
    //blendDesc.IndependentBlendEnable = FALSE;
    //blendDesc.RenderTarget[0].BlendEnable = TRUE;

    //// 注意：
    //// - テクスチャが「プレマルチ（RGBはalphaで乗算済）」なら Src = ONE, Dest = INV_SRC_ALPHA を使う
    //// - テクスチャが「ストレートアルファ（通常のRGBA）」なら Src = SRC_ALPHA, Dest = INV_SRC_ALPHA を使う
    ////
    //// 以下はストレートアルファ用に設定（通常のPNG/アルファをそのまま使う場合はこちらに戻してください）。
    //blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    //blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    //blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    //blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    //blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    //blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    //blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    //Direct3D_GetDevice()->CreateBlendState(&blendDesc, &g_pBlendStateAdd);
}

void AuraEffect_Finalize()
{
    if (g_pBlendStateAdd)
    {
        g_pBlendStateAdd->Release();
        g_pBlendStateAdd = nullptr;
    }
}

void AuraEffect_Update(double elapsedTime)
{
    g_time += elapsedTime;
}

void AuraEffect_SetViewMatrix(const DirectX::XMMATRIX& mtxView)
{
    // ビルボードシステムにビューマトリックスを設定
    Billboard_SetViewMatrix(mtxView);
}

void AuraEffect_Draw(
    int texId,
    const DirectX::XMMATRIX& mtxWorld,
    float baseScale,
    float intensity,
    const DirectX::XMFLOAT3& color)
{
    using namespace DirectX;

    // 元のブレンドステートを保存
    Direct3D_GetContext()->OMGetBlendState(&g_pOriginalBlendState, g_originalBlendFactor, &g_originalSampleMask);

    // アルファブレンドステートを設定
    float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    Direct3D_GetContext()->OMSetBlendState(g_pBlendStateAdd, blendFactor, 0xffffffff);

    // 各レイヤーを描画
    for (int i = 0; i < NUM_LAYERS; ++i)
    {
        const AuraLayer& layer = g_auraLayers[i];

        // 脈動計算
        float pulse = 1.0f + sinf((float)g_time * layer.pulseSpeed) * layer.pulseAmount;
        float currentScale = layer.scale * baseScale * pulse;

        // 回転計算（ビルボードなので実際にはあまり意味がないが、テクスチャ回転として使える）
        float rotation = (float)g_time * layer.rotationSpeed;

        // Y軸オフセット（浮遊感）
        float floatOffset = sinf((float)g_time * 1.5f + i * 0.5f) * layer.offsetY;
        XMMATRIX mtxOffset = XMMatrixTranslation(0.0f, floatOffset, 0.0f);

        // ワールド行列を合成（オフセットのみ適用）
        XMMATRIX mtxLayerWorld = mtxOffset * mtxWorld;

        // アルファ値計算（時間による変動を追加）
        float alphaVariation = sinf((float)g_time * 2.0f + i * 0.8f) * 0.1f;
        float alpha = (layer.alphaBase + alphaVariation) * intensity;
        alpha = std::max(0.0f, std::min(1.0f, alpha)); // クランプ

        // 外側のレイヤーほど少し明るく
        float brightness = 1.0f + (float)i * 0.1f;

        // 色設定（alpha をここで必ずセットする）
        XMFLOAT4 layerColor = {
            color.x * brightness,  // RGB値
            color.y * brightness,
            color.z * brightness,
            alpha  // 透過度（0.0=完全透明、1.0=不透明）
        };

        // ビルボード描画（スケールとカラーのみ指定）
       // ビルボード描画（スケールとカラーのみ指定）
        Billboard_Draw(
            texId,
            mtxLayerWorld,
            { currentScale, currentScale },
            { 0.0f, 0.0f }, // ピボット中心
            layerColor
        );
    }

    // 元のブレンドステートに戻す
    Direct3D_GetContext()->OMSetBlendState(g_pOriginalBlendState, g_originalBlendFactor, g_originalSampleMask);

    if (g_pOriginalBlendState)
    {
        g_pOriginalBlendState->Release();
        g_pOriginalBlendState = nullptr;
    }
}

// パワーアップエフェクト（金色のオーラ）
void AuraEffect_DrawPowerUp(int texId, const DirectX::XMMATRIX& mtxWorld, float scale)
{
    float intensity = 0.8f + sinf((float)g_time * 3.0f) * 0.2f; // 激しく脈動
    DirectX::XMFLOAT3 goldColor = { 1.0f, 0.9f, 0.3f }; // ゴールド

    AuraEffect_Draw(texId, mtxWorld, scale, intensity, goldColor);
}

// エネルギーエフェクト（青白いオーラ）
void AuraEffect_DrawEnergy(int texId, const DirectX::XMMATRIX& mtxWorld, float scale)
{
    float intensity = 0.6f + sinf((float)g_time * 2.0f) * 0.15f;
    DirectX::XMFLOAT3 energyColor = { 0.3f, 0.7f, 1.0f }; // 電撃のような青

    AuraEffect_Draw(texId, mtxWorld, scale, intensity, energyColor);
}

// 回復エフェクト（緑のオーラ）
void AuraEffect_DrawHeal(int texId, const DirectX::XMMATRIX& mtxWorld, float scale)
{
    float intensity = 0.5f + sinf((float)g_time * 1.5f) * 0.2f; // ゆっくり脈動
    DirectX::XMFLOAT3 healColor = { 0.3f, 1.0f, 0.5f }; // 優しい緑

    AuraEffect_Draw(texId, mtxWorld, scale, intensity, healColor);
}

// 炎エフェクト（赤橙のオーラ）
void AuraEffect_DrawFire(int texId, const DirectX::XMMATRIX& mtxWorld, float scale)
{
    float intensity = 0.7f + sinf((float)g_time * 4.0f) * 0.3f; // 激しく揺らぐ

    // 時間で色を変化させる（赤→オレンジ）
    float colorShift = sinf((float)g_time * 3.0f) * 0.3f;
    DirectX::XMFLOAT3 fireColor = {
        1.0f,
        0.3f + colorShift,
        0.1f
    };

    AuraEffect_Draw(texId, mtxWorld, scale, intensity, fireColor);
}