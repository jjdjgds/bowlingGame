// Stage_SwapAdjacent.h
#pragma once

#include "stage.h"

// 入れ替え方向
enum SwapDir {
    SWAP_UP,
    SWAP_DOWN,
    SWAP_LEFT,
    SWAP_RIGHT
};

// ステージ上のブロックを指定位置 (h,v) で隣接ブロックと入れ替え
// 成功した場合は true を返す。失敗（範囲外など）なら false
bool Stage_SwapBlock(int h, int v, SwapDir dir);
