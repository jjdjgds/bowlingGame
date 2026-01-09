#include "Stage_SwapAdjacent.h"
#include "stage.h"
#include <algorithm>  // Å© Ç±ÇÍÇí«â¡

#include "stage.h"
#include "Stage_SwapAdjacent.h"

bool Stage_SwapBlock(int h, int v, SwapDir dir)
{
    int h2 = h, v2 = v;

    switch (dir) {
    case SWAP_UP:    v2 = v - 1; break;
    case SWAP_DOWN:  v2 = v + 1; break;
    case SWAP_LEFT:  h2 = h - 1; break;
    case SWAP_RIGHT: h2 = h + 1; break;
    }

    return Stage_SwapBlockAt(h, v, h2, v2);
}

