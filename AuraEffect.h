/*==========================================================================
オーラエフェクト
                                               Author : hidetoshi muramatu
                                               Date   : 2025/12/04
---------------------------------------------------------------------------
ビルボードを使用した多層オーラエフェクトシステム
=========================================================================*/
#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

// オーラエフェクトの初期化
void AuraEffect_Initialize();

// オーラエフェクトの終了処理
void AuraEffect_Finalize();

// オーラエフェクトの更新
void AuraEffect_Update(double elapsedTime);

// ビューマトリックスを設定（描画前に一度呼ぶ）
void AuraEffect_SetViewMatrix(const DirectX::XMMATRIX& mtxView);

// オーラエフェクトの描画
// texId: オーラテクスチャID（あなたの球体画像）
// mtxWorld: ワールド変換行列（キャラクターの位置など）
// baseScale: 基本スケール
// intensity: エフェクトの強度 (0.0~1.0)
// color: オーラの色
void AuraEffect_Draw(
    int texId,
    const DirectX::XMMATRIX& mtxWorld,
    float baseScale = 1.0f,
    float intensity = 1.0f,
    const DirectX::XMFLOAT3& color = { 0.3f, 0.7f, 1.0f }  // デフォルトは青白い色
);

// プリセットエフェクト
void AuraEffect_DrawPowerUp(int texId, const DirectX::XMMATRIX& mtxWorld, float scale = 1.0f);
void AuraEffect_DrawEnergy(int texId, const DirectX::XMMATRIX& mtxWorld, float scale = 1.0f);
void AuraEffect_DrawHeal(int texId, const DirectX::XMMATRIX& mtxWorld, float scale = 1.0f);
void AuraEffect_DrawFire(int texId, const DirectX::XMMATRIX& mtxWorld, float scale = 1.0f);