// ScoreBoard.cpp
#include "ScoreBoard.h"
#include "debug_text.h"
#include "debug_ostream.h"
#include "sprite.h"
#include "texture.h"
#include "direct3d.h"

static int g_ScoreBoardTexId = -1;
static int g_NumberTexId = -1;
static constexpr int FRAME_COUNT = 4;
static constexpr float DIGIT_W = 64.0f;
static constexpr float DIGIT_H = 64.0f;
static constexpr int DIGIT_COLS = 5;
static constexpr float CELL_W = 200.0f;
static constexpr float CELL_H_NUM = 180.0f;   // 0〜9
static constexpr float CELL_H_SYM = 185.0f;   // X / -
constexpr float SCALE_THROW = 0.45f; // 上段（投球）
constexpr float SCALE_SUM = 0.55f; // 下段（合計）

static constexpr int SHEET_COLS = 5;
constexpr float startX = 0.0f;
constexpr float startY = 230.0f;
constexpr float frameW = 430.0f / FRAME_COUNT;
constexpr float throwSpacing = CELL_W * SCALE_THROW + 6.0f;
constexpr float lowerRowHeight = CELL_H_NUM * SCALE_THROW + 10.0f;

//constexpr float lowerRowHeight = 64.0f;

enum DigitSymbol
{
    DIGIT_0 = 0,
    DIGIT_1,
    DIGIT_2,
    DIGIT_3,
    DIGIT_4,
    DIGIT_5,
    DIGIT_6,
    DIGIT_7,
    DIGIT_8,
    DIGIT_9,
    SYMBOL_X,     // 10
    SYMBOL_SPARE, // 11
    SYMBOL_NONE,  // 12（空白）
};

struct Frame
{
    int throw1 = -1;
    int throw2 = -1;
    int frameScore = 0;
    
};

static Frame g_Frames[FRAME_COUNT];
static int g_CurrentFrame = 0;
static int g_CurrentThrow = 0;

void ScoreBoard_Initialize()
{
    g_ScoreBoardTexId = Texture_Load(L"rom\\Texture\\bord.jpg");
    g_NumberTexId     = Texture_Load(L"rom\\Texture\\Number.png");
    for (int i = 0; i < FRAME_COUNT; ++i)
    {
        g_Frames[i] = {};
    }
    g_CurrentFrame = 0;
    g_CurrentThrow = 0;

   

}

float GetSymbolSrcY(int symbol)
{
    if (symbol <= DIGIT_9)      return 0.0f;            // 0–9
    else if (symbol <= SYMBOL_SPARE) return CELL_H_NUM * 2; // X /
    else                         return CELL_H_NUM * 2; // 空白
}
float GetSymbolHeight(int symbol)
{
    if (symbol <= DIGIT_9) return CELL_H_NUM;
    else                  return CELL_H_SYM;
}


void DrawSymbol(
    int texId,
    int symbol,
    float x,
    float y,
    float scale
)
{
    if (symbol < 0) return;

    int col = symbol % SHEET_COLS;

    float srcY = GetSymbolSrcY(symbol);
    float srcH = GetSymbolHeight(symbol);

    Sprite_Draw(
        texId,
        x,
        y,
        CELL_W * scale,
        srcH * scale,
        col * CELL_W,
        srcY,
        CELL_W,
        srcH,
        {0,0,0,1}
    );
}


void Score_AddThrow(int pins)
{
    if (g_CurrentFrame >= FRAME_COUNT)
        return;

    Frame& f = g_Frames[g_CurrentFrame];

    // 1投目
    if (g_CurrentThrow == 0)
    {
        f.throw1 = pins;

        // ストライクなら次フレーム
        if (pins == 10)
        {
            g_CurrentFrame++;
            g_CurrentThrow = 0;
        }
        else
        {
            g_CurrentThrow = 1;
        }
    }
    // 2投目
    else
    {
        f.throw2 = pins;
        g_CurrentFrame++;
        g_CurrentThrow = 0;
    }
}

void Score_Recalculate()
{
}

void ScoreBoard_Draw()
{
    Sprite_Draw(
        g_ScoreBoardTexId,
        0.0f,
        230.0f, 145.0f,
        430.0f, 145.0f,
        0.0f, 0.0f,
        430.0f, 145.0f);

    constexpr float startX = 0.0f;
    constexpr float startY = 100.0f;
    constexpr float frameW = 430.0f / FRAME_COUNT;
    constexpr float throwSpacing = 11.0f;
    constexpr float lowerRowHeight = 64.0f;
    constexpr float THROW_SCALE = 0.3f;
    float throwDigitW = CELL_W * THROW_SCALE;

    for (int i = 0; i < FRAME_COUNT; ++i)
    {
        const Frame& f = g_Frames[i];
        float frameX = startX + frameW * i;

        // --- 1投目 ---
        if (f.throw1 >= 0)
        {
            int symbol =
                (f.throw1 == 10) ? SYMBOL_X :
                f.throw1;

            DrawSymbol(
                g_NumberTexId,
                symbol,
                frameX + 16.0f,
                startY,
                0.3f
            );
        }

        // --- 2投目 ---
        if (f.throw2 >= 0)
        {
            int symbol =
                (f.throw1 < 10 && f.throw1 + f.throw2 == 10)
                ? SYMBOL_SPARE
                : f.throw2;

            DrawSymbol(
                g_NumberTexId,
                symbol,
                frameX + 5.0f + throwDigitW + 6.0f,
                startY,
                0.3f
            );
        }

        // --- フレーム合計 ---
        if (f.throw1 >= 0)
        {
            int sum = f.throw1 + (f.throw2 >= 0 ? f.throw2 : 0);

            constexpr float SUM_SCALE = 0.32f;
            float digitW = CELL_W * SUM_SCALE;

            // 10の位
            DrawSymbol(
                g_NumberTexId,
                sum / 10,
                frameX + 24.0f,
                startY + lowerRowHeight,
                SUM_SCALE
            );

            // 1の位（← 正しい間隔）
            DrawSymbol(
                g_NumberTexId,
                sum % 10,
                frameX + 24.0f + digitW-30,
                startY + lowerRowHeight,
                SUM_SCALE
            );
        }

    }
}
