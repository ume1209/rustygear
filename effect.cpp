//=============================================================================
//
// エフェクト処理 [effect.cpp]
// Author : 梅津　修也
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "effect.h"
#include "enemy.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_EFFECT		"data/MODEL/kirakira.obj"			// 読み込むモデル名



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************



//*****************************************************************************
// グローバル変数
//*****************************************************************************
static EFFECT		g_Effect[MAX_EFFECT];						// プレイヤー

static XMFLOAT3					g_posB;				// エフェクト発生位置

static BOOL			g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEffect(void)
{
	for (int effectIndex = 0; effectIndex < MAX_EFFECT; effectIndex++)
	{
		LoadModel(MODEL_EFFECT, &g_Effect[effectIndex].model);

		g_Effect[effectIndex].pos = { 0.0f, 0.0f, 0.0f };
		g_Effect[effectIndex].rot = { 0.0f, 0.0f, 0.0f };
		g_Effect[effectIndex].scl = { 1.0f, 1.0f, 1.0f };
		g_Effect[effectIndex].move = { 0.0f, 1.0f, 0.0f };


		g_Effect[effectIndex].use = true;
	}

	return S_OK;
}


//=============================================================================
// 終了処理
//=============================================================================
void UninitEffect(void)
{
	for (int effectIndex = 0; effectIndex < MAX_EFFECT; effectIndex++)
	{
		// モデルの解放処理
		if (g_Effect[effectIndex].ismodelLoaded)
		{
			UnloadModel(&g_Effect[effectIndex].model);
			g_Effect[effectIndex].ismodelLoaded = false;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEffect(void)
{
	//lifetimeは16フレーム
	ENEMY* pEnemy = GetEnemy();
	g_posB = pEnemy->pos;


	for (int effectIndex = 0; effectIndex < MAX_EFFECT; effectIndex++)
	{
		if (!g_Effect[effectIndex].use)
			continue;
		// 使用中
		g_Effect[effectIndex].lifetimeFrame--;

		g_Effect[effectIndex].pos.x += g_Effect[effectIndex].move.x;
		g_Effect[effectIndex].pos.z += g_Effect[effectIndex].move.z;
		g_Effect[effectIndex].pos.y += g_Effect[effectIndex].move.y;

		g_Effect[effectIndex].rot.y += XM_PI / 5;

		if (g_Effect[effectIndex].lifetimeFrame < 0)
		{
			g_Effect[effectIndex].use = false;
			continue;
		}

		if (g_Effect[effectIndex].lifetimeFrame > 32)
			continue;

		if (g_Effect[effectIndex].lifetimeFrame < 16)
		{
			g_Effect[effectIndex].scl.x = (float)(g_Effect[effectIndex].lifetimeFrame) / 16 * 0.5f;
			g_Effect[effectIndex].scl.y = (float)(g_Effect[effectIndex].lifetimeFrame) / 16 * 0.5f;
			g_Effect[effectIndex].scl.z = (float)(g_Effect[effectIndex].lifetimeFrame) / 16 * 0.5f;
			continue;
		}

		g_Effect[effectIndex].scl.x = (float)(32 - g_Effect[effectIndex].lifetimeFrame) / 16 * 0.5f;
		g_Effect[effectIndex].scl.y = (float)(32 - g_Effect[effectIndex].lifetimeFrame) / 16 * 0.5f;
		g_Effect[effectIndex].scl.z = (float)(32 - g_Effect[effectIndex].lifetimeFrame) / 16 * 0.5f;

	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEffect(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);
	for (int effectIndex = 0; effectIndex < MAX_EFFECT; effectIndex++)
	{

		if (!g_Effect[effectIndex].use)
			continue;
		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Effect[effectIndex].scl.x, g_Effect[effectIndex].scl.y, g_Effect[effectIndex].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Effect[effectIndex].rot.x, g_Effect[effectIndex].rot.y + XM_PI, g_Effect[effectIndex].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Effect[effectIndex].pos.x, g_Effect[effectIndex].pos.y, g_Effect[effectIndex].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		// 自分(プレイヤー)のmtxWorldを保存している。パーツをくっつけるのが理由になる
		XMStoreFloat4x4(&g_Effect[effectIndex].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_Effect[effectIndex].model);

	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);

}

//=============================================================================
// エフェクトの発生処理
//=============================================================================
void SetEffect(XMFLOAT3 pos, int lifetimeFrame)
{
	int nidxEffect = -1;

	for (int effectIndex = 0; effectIndex < MAX_EFFECT; effectIndex++)
	{
		if (!g_Effect[effectIndex].use)
		{
			g_Effect[effectIndex].pos = pos;
			g_Effect[effectIndex].rot = { 0.0f,0.0f,0.0f };
			g_Effect[effectIndex].scl = { 0.1f,0.1f,0.1f };
			g_Effect[effectIndex].lifetimeFrame = lifetimeFrame;
			g_Effect[effectIndex].use = true;

			nidxEffect = effectIndex;

			break;
		}

	}

}

