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

    void AddPin(const DirectX::XMFLOAT3& pos);
    void ResolveBallPinHit(BowlingBall& ball, Pins& pin);
private:
    void ResolvePinPin(Pins& a, Pins& b);

    std::vector<Pins> m_pins;
};
