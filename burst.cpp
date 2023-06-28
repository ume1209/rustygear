//=============================================================================
//
// エフェクト処理 [burst.cpp]
// Author : 梅津　修也
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "burst.h"
#include "enemy.h"
#include "meshfield.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_BURST		"data/MODEL/burst.obj"			// 読み込むモデル名



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************



//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BURST		g_Burst[MAX_BURST];						// プレイヤー

static XMFLOAT3					g_posB;				// エフェクト発生位置

static BOOL			g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBurst(void)
{
	for (int effectIndex = 0; effectIndex < MAX_BURST; effectIndex++)
	{
		LoadModel(MODEL_BURST, &g_Burst[effectIndex].model);

		g_Burst[effectIndex].pos  = { 0.0f, 0.0f, 0.0f };
		g_Burst[effectIndex].rot  = { 0.0f, 0.0f, 0.0f };
		g_Burst[effectIndex].scl  = { 1.0f, 1.0f, 1.0f };
		g_Burst[effectIndex].move = { 0.0f, 0.0f, 0.0f };
		g_Burst[effectIndex].accel= { 0.0f, -0.5f, -1.0f };

		g_Burst[effectIndex].use = true;
	}

	return S_OK;
}


//=============================================================================
// 終了処理
//=============================================================================
void UninitBurst(void)
{
	for (int effectIndex = 0; effectIndex < MAX_BURST; effectIndex++)
	{
		// モデルの解放処理
		if (g_Burst[effectIndex].ismodelLoaded)
		{
			UnloadModel(&g_Burst[effectIndex].model);
			g_Burst[effectIndex].ismodelLoaded = false;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateBurst(void)
{
	//lifetimeは16フレーム
	ENEMY* pEnemy = GetEnemy();
	g_posB = pEnemy->pos;


	for (int effectIndex = 0; effectIndex < MAX_BURST; effectIndex++)
	{
		if (!g_Burst[effectIndex].use)
			continue;
		// 使用中
		g_Burst[effectIndex].lifetimeFrame--;

		XMVECTOR pos   = XMLoadFloat3(&g_Burst[effectIndex].pos);
		XMVECTOR move  = XMLoadFloat3(&g_Burst[effectIndex].move);
		XMVECTOR accel = XMLoadFloat3(&g_Burst[effectIndex].accel);

		pos += move;
		move += accel;

		XMStoreFloat3(&g_Burst[effectIndex].pos, pos);
		XMStoreFloat3(&g_Burst[effectIndex].move, move);

		XMFLOAT3 hitPosition;
		XMFLOAT3 normal;
		RayHitField(g_Burst[effectIndex].pos, &hitPosition, &normal);

		if (g_Burst[effectIndex].pos.y < hitPosition.y)
		{
			g_Burst[effectIndex].pos.y = hitPosition.y;
			g_Burst[effectIndex].move.y *= -0.5f;
		}


		g_Burst[effectIndex].rot.z += XM_PI / 20;

		if (g_Burst[effectIndex].lifetimeFrame < 0)
		{
			g_Burst[effectIndex].use = false;
			continue;
		}


		//if (g_Burst[effectIndex].lifetimeFrame > 32)
		//	continue;

		//if (g_Burst[effectIndex].lifetimeFrame < 16)
		//{
		//	g_Burst[effectIndex].scl.x = (float)(g_Burst[effectIndex].lifetimeFrame) / 16 * 1.0f;
		//	g_Burst[effectIndex].scl.y = (float)(g_Burst[effectIndex].lifetimeFrame) / 16 * 1.0f;
		//	g_Burst[effectIndex].scl.z = (float)(g_Burst[effectIndex].lifetimeFrame) / 16 * 1.0f;
		//	continue;
		//}

		//g_Burst[effectIndex].scl.x = (float)(32 - g_Burst[effectIndex].lifetimeFrame) / 16 * 1.0f;
		//g_Burst[effectIndex].scl.y = (float)(32 - g_Burst[effectIndex].lifetimeFrame) / 16 * 1.0f;
		//g_Burst[effectIndex].scl.z = (float)(32 - g_Burst[effectIndex].lifetimeFrame) / 16 * 1.0f;

	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBurst(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);
	for (int effectIndex = 0; effectIndex < MAX_BURST; effectIndex++)
	{

		if (!g_Burst[effectIndex].use)
			continue;
		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Burst[effectIndex].scl.x, g_Burst[effectIndex].scl.y, g_Burst[effectIndex].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Burst[effectIndex].rot.x, g_Burst[effectIndex].rot.y + XM_PI, g_Burst[effectIndex].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Burst[effectIndex].pos.x, g_Burst[effectIndex].pos.y, g_Burst[effectIndex].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		// 自分(プレイヤー)のmtxWorldを保存している。パーツをくっつけるのが理由になる
		XMStoreFloat4x4(&g_Burst[effectIndex].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_Burst[effectIndex].model);

	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);

}

//=============================================================================
// エフェクトの発生処理
//=============================================================================
void SetBurst(XMFLOAT3 pos, int lifetimeFrame)
{
	int nidxBurst = -1;

	for (int effectIndex = 0; effectIndex < MAX_BURST; effectIndex++)
	{
		if (!g_Burst[effectIndex].use)
		{
			g_Burst[effectIndex].pos = pos;
			g_Burst[effectIndex].rot = { 0.0f,0.0f,0.0f };
			g_Burst[effectIndex].scl = { 1.0f,1.0f,1.0f };
			g_Burst[effectIndex].lifetimeFrame = lifetimeFrame;
			g_Burst[effectIndex].use = true;
			float dir = (rand()%17) / 16.0f * (2*XM_PI);
			float force = rand() % 5;
			g_Burst[effectIndex].move = { sinf(dir) * force,4.0f,cosf(dir) * force };
			


			nidxBurst = effectIndex;

			break;
		}

	}

}
