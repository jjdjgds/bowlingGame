/*==========================================================================

オーラエフェクト

                                                Author : hidetoshi muramatu
                                                Date   : 2025/01/11
---------------------------------------------------------------------------
ビルボードを使用したオーラエフェクトシステム
- 多重レイヤー対応
- アニメーション機能
- カラーグラデーション
- スケールアニメーション

=========================================================================*/

#pragma once

#include <DirectXMath.h>

// オーラエフェクトの設定
struct AuraEffectParams
{
    int textureId;                      // 使用するテクスチャID
    DirectX::XMFLOAT4 innerColor;       // 内側の色
    DirectX::XMFLOAT4 outerColor;       // 外側の色
    float baseScale;                    // 基本スケール
    float pulseSpeed;                   // 脈動速度
    float pulseAmount;                  // 脈動の強さ (0.0~1.0)
    float rotationSpeed;                // 回転速度 (ラジアン/秒)
    int layerCount;                     // レイヤー数 (1~4)
    float layerScaleOffset;             // レイヤー間のスケール差
    float layerAlphaFalloff;            // レイヤー間の透明度減衰
    bool useAdditiveBlending;           // 加算合成を使用するか

    // パーティクルモード用パラメータ
    bool useParticleMode;               // パーティクルモードを使用するか
    int particleCount;                  // パーティクル数 (1~100)
    float particleRadius;               // パーティクルの配置半径
    float particleSpeed;                // パーティクルの周回速度
    float particleScale;                // 各パーティクルのスケール
    float particleRandomness;           // ランダム性 (0.0~1.0)
    float particleVerticalSpeed;        // 上下移動速度
    float particleVerticalRange;        // 上下移動範囲
    float particleRotationSpeed;        // パーティクル個別の回転速度
};

// デフォルト設定を返す
AuraEffectParams AuraEffect_GetDefaultParams();

// 初期化・終了処理
void AuraEffect_Initialize();
void AuraEffect_Finalize();

// 更新処理
void AuraEffect_Update(double elapsedTime);

// 描画処理
void AuraEffect_Draw(
    const AuraEffectParams& params,
    const DirectX::XMMATRIX& mtxWorld
);

// プリセット設定
AuraEffectParams AuraEffect_PresetFire();      // 炎のオーラ
AuraEffectParams AuraEffect_PresetIce();       // 氷のオーラ
AuraEffectParams AuraEffect_PresetElectric();  // 電気のオーラ
AuraEffectParams AuraEffect_PresetHoly();      // 神聖なオーラ
AuraEffectParams AuraEffect_PresetDark();      // 暗黒のオーラ

// パーティクルモードプリセット
AuraEffectParams AuraEffect_PresetFireParticles();     // 炎の粒子群
AuraEffectParams AuraEffect_PresetStarfield();         // 星空エフェクト
AuraEffectParams AuraEffect_PresetEnergyOrb();         // エネルギー球体
AuraEffectParams AuraEffect_PresetMagicCircle();       // 魔法陣風