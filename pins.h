/*==========================================================================
ボーリングのピンを定義するヘッダファイル[pins.h]

    Author : hidetoshi muramatu
    Date   : 2026/01/11
---------------------------------------------------------------------------

==========================================================================*/

#pragma once
#include <DirectXMath.h>
#include "model.h"
#include "Collision.h"
class Pins
{
public:
    Pins();
    ~Pins() {};

    // 初期化
    void Initialize(const DirectX::XMFLOAT3& pos);

    // 更新
    void Update(float deltaTime);

    // 描画
    void Draw() ;

    // ボール衝突
    void Hit(const DirectX::XMFLOAT3& impulse, const DirectX::XMFLOAT3& hitPoint);

    // 状態取得
    bool IsDown() const;

    // 位置取得
    const DirectX::XMFLOAT3& GetPosition() const { return m_position; }
    const DirectX::XMFLOAT3& GetVelocity() const { return m_velocity; }

private:
    // 位置・回転
    DirectX::XMFLOAT3 m_position;
    DirectX::XMFLOAT3 m_rotation;
    DirectX::XMFLOAT3 m_scale;
    DirectX::XMFLOAT3 m_velocity;
    DirectX::XMFLOAT3 m_angularVelocity;
    // 倒れ判定
    bool  m_isDown;
    float m_mass;
    AABB m_Aabb;
    MODEL* m_model{ nullptr };
    DirectX::XMFLOAT3 m_baseHalf;   // ← モデル基準の当たり判定サイズ
public:
    
  
    const AABB& GetAABB() const { return m_Aabb; }

};
