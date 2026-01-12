// ScoreBoard.cpp
#include "ScoreBoard.h"
#include "debug_text.h"
#include "debug_ostream.h"
#include "sprite.h"
#include "texture.h"
#include "direct3d.h"

static int g_ScoreBoardTexId = -1;
static constexpr int FRAME_COUNT = 4;
static hal::DebugText* g_DebugText = nullptr;
static hal::DebugText* g_DebugText1 = nullptr;
static hal::DebugText* g_DebugText2 = nullptr;

struct Frame
{
    int throw1 = -1;
    int throw2 = -1;
};

static Frame g_Frames[FRAME_COUNT];
static int g_CurrentFrame = 0;
static int g_CurrentThrow = 0;

void ScoreBoard_Initialize()
{
    g_ScoreBoardTexId = Texture_Load(L"rom\\Texture\\bord.jpg");

    for (int i = 0; i < FRAME_COUNT; ++i)
    {
        g_Frames[i] = {};
    }
    g_CurrentFrame = 0;
    g_CurrentThrow = 0;

    // --- DebugText 作成 ---
    //delete g_DebugText;
    g_DebugText = new hal::DebugText(
        Direct3D_GetDevice(),
        Direct3D_GetContext(),
        L"rom/consolab_ascii_512.png",
        (float)Direct3D_GetBackBufferWidth(),
        (float)Direct3D_GetBackBufferHeight(),
        0.0f,
        0.0f
    );
    //delete g_DebugText1;
    g_DebugText1 = new hal::DebugText(
        Direct3D_GetDevice(),
        Direct3D_GetContext(),
        L"rom/consolab_ascii_512.png",
        (float)Direct3D_GetBackBufferWidth(),
        (float)Direct3D_GetBackBufferHeight(),
        0.0f,
        0.0f
    );
    //delete g_DebugText2;
    g_DebugText2 = new hal::DebugText(
        Direct3D_GetDevice(),
        Direct3D_GetContext(),
        L"rom/consolab_ascii_512.png",
        (float)Direct3D_GetBackBufferWidth(),
        (float)Direct3D_GetBackBufferHeight(),
        0.0f,
        0.0f
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

    if (!g_DebugText)
        return;

    // ★ 毎フレーム必須 - テキストをクリア
    g_DebugText->Clear();
    g_DebugText->SetScale(1.4f);
    g_DebugText1->Clear();
    g_DebugText1->SetScale(1.4f);
    g_DebugText2->Clear();
    g_DebugText2->SetScale(1.4f);
    // スコアボードのレイアウトに合わせた配置
    constexpr float startX = 0.0f;      // 左端の開始位置
    constexpr float startY = 100.0f;     // 上段の開始位置
    constexpr float frameW = 107.5f;     // フレーム間隔 (430 / 4)
    constexpr float throwSpacing = 50.0f; // 1投目と2投目の間隔
    constexpr float upperRowHeight = 20.0f;  // 上段（投球結果）
    constexpr float lowerRowHeight = 70.0f; // 下段（合計スコア）

    // ★ 各フレームを横に並べて個別に描画
    for (int i = 0; i < FRAME_COUNT; ++i)
    {
        const Frame& f = g_Frames[i];
        float frameX = startX + i * frameW;

        // --- 上段: 1投目表示（左側の小さいセル） ---
        if (f.throw1 >= 0)
        {
            char throw1Str[8];
            if (f.throw1 == 10)
                strcpy_s(throw1Str, "X");
            else
                sprintf_s(throw1Str, "%d", f.throw1);

            g_DebugText->SetOffset(frameX + 20.0f, startY );
            g_DebugText->SetText(throw1Str, { 0, 0, 0, 1 });
            g_DebugText->Draw();
        }

        // --- 上段: 2投目表示（右側の小さいセル） ---
        if (f.throw2 >= 0)
        {
            char throw2Str[8];
            // スペア判定: 1投目が10未満で、合計が10
            if (f.throw1 < 10 && f.throw1 + f.throw2 == 10)
                strcpy_s(throw2Str, "/");
            else
                sprintf_s(throw2Str, "%d", f.throw2);

            g_DebugText1->SetOffset(frameX + 20.0f + throwSpacing, startY + upperRowHeight);
            g_DebugText1->SetText(throw2Str, { 0, 0, 0, 1 });
            g_DebugText1->Draw();
        }

        // --- 下段: フレーム合計表示（大きいセル） ---
        if (f.throw1 >= 0)
        {
            int sum = f.throw1 + (f.throw2 >= 0 ? f.throw2 : 0);
            char sumStr[8];
            sprintf_s(sumStr, "%d", sum);

            g_DebugText2->SetOffset(frameX + 30.0f, startY + lowerRowHeight);
            g_DebugText2->SetText(sumStr, { 1, 0, 0, 1 });
            g_DebugText2->Draw();
          
        }
    }

    // ★ 最後に1回だけDrawを呼ぶ
   
   
    
}