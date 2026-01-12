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
