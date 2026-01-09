#include "game.h"
#include "game_bg.h"
#include "grid.h"
#include "game_score.h"
#include "lighth.h"
#include "keylogger.h"
#include "Map.h"
#include "debug.h"
#include "camera.h"
#include "gameeffect.h"
#include "texture.h"
#include "player.h"
#include "mouse.h"
#include "direct3d.h"
#include "scene.h"
#include "Timer.h"
#include "Ball.h"
#include "sprite_anim.h"
//#include "cube.h"
#include "model.h";
#include "Shot.h"
#include "BillBord.h"
#include "sprite_anim.h"
#include "ShotCamera.h"
#include "camera_fixed.h"
#include "trail.h"

using namespace DirectX;


static Camera* g_a[2];
static bool g_Start = false;
static int g_BgmId = -1; // BGMのID
static bool g_NextChlick = false;
static AnimPattern* g_pTestAnim = nullptr;
static float g_SelectedX = 0.0f;
static float g_SelectedY = 0.0f;
static float g_LimitTime = 0.0f;
static int Cube01tex = -1;
static AnimPatternPlayer* g_pAnimPlayer{ nullptr };
//ポインタ Newすれば作ったタイミングで生成される
//deleteすれば消える
static DebugCamera* g_pDebugCamera{}; 
static FixedCamera* g_FixedCameras[3];
static int g_FixedCameraIndex{0};


static MODEL* g_pKirby;
static MODEL* g_pPenis;
static AnimPattern* g_pTestAnim2 = nullptr;
static AnimPatternPlayer* g_pAnimPlayer2{ nullptr };
static ShotCamera* g_pShotCamera{ nullptr };

static int g_texid = -1;
static int g_texid2 = -1;

static AABB g_GoalAABB;
static bool g_IsGoal = false;

void Game_Initialize()
{
	g_GoalAABB = AABB::Make({ float(1.0f), 0.9f, float(1.0f) }, { 1,1,1 });

	g_pDebugCamera = new DebugCamera({ 0.0f,1.0f,-5.0f }, { 0.0f,0.0f,0.0f });
	g_pShotCamera = new ShotCamera({ 0.0f,0.0f,50.0f }, {2.0f,1.0f,2.0f},2.0f);
	g_FixedCameras[0] = new FixedCamera({ -5.0f, 10.0f, 10.0f }, AABB::Make({ 1.0f,5.00f,1.0f }, {5.0f,5.0f,5.0f}));
	g_FixedCameras[1] = new FixedCamera({5.0f, 10.0f, 10.0f}, AABB::Make({ 6.0f,6.0f,6.0f }, { 20.0f,10.0f,20.0f }));

//	Cube01tex = Texture_Load(L"rom\\saikoro_image.png");
	Billboard_Initialize();
	Gulid_Initialize(10,10,1);
	Map_Initialize();
	Light_Initialize();
	Ball_Initialize({3,5,2});
	Shot_Initialize({ 3,5,2 },g_pDebugCamera->GetFront());
	Score_Initialize(100,100,2);
	Trail_Initialize();
	//df.fbx
	//g_pPenis = ModelLoad("rom\\Model\\yajirusi.fbx",0.1);
	g_texid = Texture_Load(L"rom\\runningman003.png");
	g_pKirby = ModelLoad("rom\\Model\\kirby.fbx",0.1);
	g_texid2 = Texture_Load(L"rom\\bomb3.png");



	g_pTestAnim = new AnimPattern(g_texid, 10, 5, 0.1, { 0,0 }, { 140,200});
	g_pAnimPlayer = new AnimPatternPlayer(g_pTestAnim);


	/*g_pTestAnim2 = new AnimPattern(g_texid2, 10, 5, 0.1, { 0,0 }, { 140,200 }, false);
	g_pAnimPlayer2 = new AnimPatternPlayer(g_pTestAnim2);*/

}
static double g_time{ 0.0 };
void Game_Update(double elapsed_time)
{
	g_time += elapsed_time;
	g_pDebugCamera->Update(elapsed_time);
	// ココを追加
	//Cube_Update(elapsed_time);
	Shot_Update(elapsed_time);
	Ball_Update(elapsed_time);
	Trail_AddPosition(Ball_GetPosition());
	Billboard_Update(elapsed_time);
	for (int i = 0; i < 2; i++)
	{
		g_FixedCameras[i]->SetTarget(Ball_GetPosition());
		if (g_FixedCameras[i]->IsOverlap())
		{
			g_FixedCameraIndex = i;
			break;
		}
	}
	g_FixedCameras[g_FixedCameraIndex]->SetMatrix();
	Billboard_SetViewMatrix(g_FixedCameras[g_FixedCameraIndex]->GetViewMatrix());

	Trail_SetCameraPosition(g_FixedCameras[g_FixedCameraIndex]->GetPosition());
	if(Ball_IsStationary())
	{
		//g_pShotCamera->SetTarget(Ball_GetPosition());
		Shot_SetPosition(Ball_GetPosition());
		//g_pDebugCamera->SetMatrix();

		if (KeyLogger_IsTrigger(KK_ENTER))
		{
			/*XMFLOAT3 front = g_DebugCamera.GetFront();
			front.y = 2.0f;
			XMVECTOR velo = XMVector3Normalize(XMLoadFloat3(&front)) * 15.0f;
			XMStoreFloat3(&front, velo);*/
			if(!g_pAnimPlayer2)

			{
				g_pTestAnim2 = new AnimPattern(g_texid2, 8, 4, 0.1, { 0,0 }, { 156,185 }, false);
				g_pAnimPlayer2 = new AnimPatternPlayer(g_pTestAnim2);
				
			}
			BallShot(Shot_GetVelocity());
			Score_AddScore(1);

		}
	}
	Score_Update();
	
	Hit hit = g_GoalAABB.IsHit(Ball_GetAABB());
	//上向き法線がTrueだったらのを取ればまぁいいよね
	if (hit.IsHit()) {

		//ゴール演出

		g_IsGoal = true;

	}

	g_pAnimPlayer->Update(elapsed_time);

	if(g_pAnimPlayer2)
	{
		g_pAnimPlayer2->Update(elapsed_time);
		if (g_pAnimPlayer2->IsAnimStopeed())
		{
			delete g_pAnimPlayer2;
			delete g_pTestAnim2;
			g_pAnimPlayer2 = nullptr;
		}
	}


	
	
}

void Game_Draw()
{

	Direct3D_SetDepthTest(true);
	//g_pShotCamera->SetMatrix();
	
	//Direct3D_SetDepthWriteDisable();
	//g_pAnimPlayer->BillboardDraw({ 3.0, 2.0f, 2.0f }, { 0.7,1 }, {0.5,0.5});


		//XMMATRIX mtx = XMMatrixScaling(100.0f, 3000.0f, 10000.0f) * XMMatrixTranslation(4.5f, 0.5f, 4.5f);
		//Cube_Draw(mtx,0);
	
	
		
	

	Gulid_Draw();
	Light_SetAmnient({0.5f,0.5f,0.6f,1.0f});
	XMFLOAT3 direction;
	XMStoreFloat3(&direction, { -1.0f,-1.5f,1.0f });
	Light_SetDiffuse({ 0.5f,0.5f,0.5f }, direction);

	//ModelDraw(g_pKirby, XMMatrixIdentity());
	Ball_Draw();
	if (Ball_IsStationary()) {
		Shot_Draw();
	}
	Trail_Draw();
	//ModelDraw(g_pPenis, XMMatrixIdentity());

	Billboard_SetViewMatrix(g_pDebugCamera->GetViewMatrix());

	
	Map_Draw();
	
	//2D描画はここに
	Direct3D_SetDepthTest(false);
	//Score_Draw();

	//Billboard_Draw(g_texid, XMMatrixTranslation(3.0, 2.0f, 2.0f), 
	//	{ 140.0f,200.0f }, { 14.0f,20.0f }); // world を Identity にするか、任意のワールド行列を渡す
	//g_pAnimPlayer->BillboardDraw({ 3.0, 2.0f, 2.0f }, { 0.14,0.2 });
	if(g_IsGoal)
	{
		XMFLOAT3 pos = g_GoalAABB.GetCenter();
		pos.y += 5.0f;
		if (g_pAnimPlayer2)
		{
			g_pAnimPlayer2->BillboardDraw({ pos }, { 1.4,2.0 }, { 0,0 });
		}
	}


}

void Game_Finalize()
{
	//delete g_pTestAnim2;
	delete g_pTestAnim;
	delete g_pDebugCamera;
	delete g_pShotCamera;
	ModelRelease(g_pKirby);
	//ModelRelease(g_pPenis);
	Ball_Finalize();
	Map_Finalize();
	Gulid_Finalize();
	Light_Finalize();
	Shot_Finalize();
	Trail_Finalize();
}



