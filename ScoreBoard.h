// ScoreBoard.h
#pragma once
#include "debug_text.h"
#include "debug_ostream.h"
enum class ThrowMark
{
    None,
    Number,
    Spare,
    Strike
};



void Score_AddThrow(int pins);
void Score_Recalculate();
void ScoreBoard_Draw();
void ScoreBoard_Initialize();
float GetSymbolSrcY(int symbol);
float GetSymbolHeight(int symbol);
void DrawSymbol(
    int texId,
    int symbol,
    float x,
    float y,
    float scale = 1.0f
);