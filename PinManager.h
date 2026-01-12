#pragma once
#include <vector>
#include "pins.h"
#include "bowlingBall.h"
class PinManager
{
public:
    void Initialize();
    void Update(float dt, BowlingBall& ball);
    void Draw();
    void SetupBowlingPins(const DirectX::XMFLOAT3& headPinPos);
    void ResetPins();
    int GetDownPinCount() const;
    void AddPin(const DirectX::XMFLOAT3& pos);
    void ResolveBallPinHit(BowlingBall& ball, Pins& pin);
    int GetRemainingPinCount() const;
    int GetInitialPinCount() const;
    void RemoveDeadPins();

    // 新: 全ピンを破棄して再配置（"1フレーム終わるごとに" 呼ぶ目的）
    void DestroyAndRecreatePins();

private:
    void ResolvePinPin(Pins& a, Pins& b);

    std::vector<Pins> m_pins;
};
