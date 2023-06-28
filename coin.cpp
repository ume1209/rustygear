//=============================================================================
//
// エネミーモデル処理 [coin.cpp]
// Author : 梅津　修也
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "coin.h"
#include "shadow.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_COIN			"data/MODEL/coin.obj"		// 読み込むモデル名

#define	VALUE_MOVE			(5.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

#define COIN_SHADOW_SIZE	(0.4f)						// 影の大きさ
#define COIN_OFFSET_Y		(10.0f)						// エネミーの足元をあわせる


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static COIN			g_Coin[MAX_COIN];				// エネミー


int g_Coin_load = 0;


static INTERPOLATION_DATA move_tbl[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(   0.0f, COIN_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*2 },
	{ XMFLOAT3(-200.0f, COIN_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*1 },
	{ XMFLOAT3(-200.0f, COIN_OFFSET_Y, 200.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*0.5f },
	{ XMFLOAT3(   0.0f, COIN_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*2 },

};

static int		g_Count;					// カウント

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitCoin(void)
{
	for (int i = 0; i < MAX_COIN; i++)
	{
		LoadModel(MODEL_COIN, &g_Coin[i].model);
		g_Coin[i].load = true;

		g_Coin[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Coin[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Coin[i].scl = XMFLOAT3(0.1f, 0.1f, 0.1f);

		g_Coin[i].spd = 0.0f;			// 移動スピードクリア
		g_Coin[i].size = COIN_SIZE;	// 当たり判定の大きさ

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Coin[0].model, &g_Coin[0].diffuse[0]);

		XMFLOAT3 pos = g_Coin[i].pos;
		pos.y -= (COIN_OFFSET_Y - 0.1f);
		g_Coin[i].shadowIdx = CreateShadow(pos, COIN_SHADOW_SIZE, COIN_SHADOW_SIZE);

		g_Coin[i].move_time = 0.0f;	// 線形補間用のタイマーをクリア
		g_Coin[i].tbl_adr = NULL;		// 再生するアニメデータの先頭アドレスをセット
		g_Coin[i].tbl_size = 0;		// 再生するアニメデータのレコード数をセット

		g_Coin[i].use = false;			// true:生きてる

	}


	//// 0番だけ線形補間で動かしてみる
	//g_Coin[0].move_time = 0.0f;		// 線形補間用のタイマーをクリア
	//g_Coin[0].tbl_adr = move_tbl;		// 再生するアニメデータの先頭アドレスをセット
	//g_Coin[0].tbl_size = sizeof(move_tbl) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitCoin(void)
{

	for (int i = 0; i < MAX_COIN; i++)
	{
		if (g_Coin[i].load)
		{
			UnloadModel(&g_Coin[i].model);
			g_Coin[i].load = false;
		}

	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateCoin(void)
{
	// エネミーを動かく場合は、影も合わせて動かす事を忘れないようにね！
	for (int i = 0; i < MAX_COIN; i++)
	{
		int C_flug = rand() % 1000;

		if (C_flug == 33)
		{
			SetCoin(XMFLOAT3(-20.0f, COIN_OFFSET_Y, 400.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
		}
		if (C_flug == 66)
		{
			SetCoin(XMFLOAT3(0.0f, COIN_OFFSET_Y, 400.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
		}

		if (C_flug == 99)
		{
			SetCoin(XMFLOAT3(20.0f, COIN_OFFSET_Y, 400.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
		}
	

		if (g_Coin[i].use == true)			// このエネミーが使われている？
		{	
			g_Coin[i].pos.z -= 5.0f;
			g_Coin[i].rot.y += VALUE_ROTATE;
			// Yes
			//if (g_Coin[i].tbl_adr != NULL)	// 線形補間を実行する？
			//{								// 線形補間の処理
				//// 移動処理
				//int		index = (int)g_Coin[i].move_time;
				//float	time = g_Coin[i].move_time - index;
				//int		size = g_Coin[i].tbl_size;

				//float dt = 1.0f / g_Coin[i].tbl_adr[index].frame;	// 1フレームで進める時間
				//g_Coin[i].move_time += dt;							// アニメーションの合計時間に足す

				//if (index > (size - 2))	// ゴールをオーバーしていたら、最初へ戻す
				//{
				//	g_Coin[i].move_time = 0.0f;
				//	index = 0;
				//}

				//// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
				//XMVECTOR p1 = XMLoadFloat3(&g_Coin[i].tbl_adr[index + 1].pos);	// 次の場所
				//XMVECTOR p0 = XMLoadFloat3(&g_Coin[i].tbl_adr[index + 0].pos);	// 現在の場所
				//XMVECTOR vec = p1 - p0;
				//XMStoreFloat3(&g_Coin[i].pos, p0 + vec * time);

				//// 回転を求める	R = StartX + (EndX - StartX) * 今の時間
				//XMVECTOR r1 = XMLoadFloat3(&g_Coin[i].tbl_adr[index + 1].rot);	// 次の角度
				//XMVECTOR r0 = XMLoadFloat3(&g_Coin[i].tbl_adr[index + 0].rot);	// 現在の角度
				//XMVECTOR rot = r1 - r0;
				//XMStoreFloat3(&g_Coin[i].rot, r0 + rot * time);

				//// scaleを求める S = StartX + (EndX - StartX) * 今の時間
				//XMVECTOR s1 = XMLoadFloat3(&g_Coin[i].tbl_adr[index + 1].scl);	// 次のScale
				//XMVECTOR s0 = XMLoadFloat3(&g_Coin[i].tbl_adr[index + 0].scl);	// 現在のScale
				//XMVECTOR scl = s1 - s0;
				//XMStoreFloat3(&g_Coin[i].scl, s0 + scl * time);

			//}

			// 影もプレイヤーの位置に合わせる
			XMFLOAT3 pos = g_Coin[i].pos;
			pos.y -= (COIN_OFFSET_Y - 0.1f);
			SetPositionShadow(g_Coin[i].shadowIdx, pos);

			if (g_Coin[i].pos.z < -80)
			{
				g_Coin[i].use = false;
			}

		}

		if (g_Coin[i].use == false)
		{
			ReleaseShadow(g_Coin[i].shadowIdx);
		}
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawCoin(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_COIN; i++)
	{
		if (g_Coin[i].use == false) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Coin[i].scl.x, g_Coin[i].scl.y, g_Coin[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Coin[i].rot.x, g_Coin[i].rot.y + XM_PI, g_Coin[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Coin[i].pos.x, g_Coin[i].pos.y, g_Coin[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Coin[i].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_Coin[i].model);
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// エネミーの取得
//=============================================================================
COIN *GetCoin(void)
{
	return &g_Coin[0];
}

//=============================================================================
// 弾のパラメータをセット
//=============================================================================
int SetCoin(XMFLOAT3 pos, XMFLOAT3 rot)
{
	int nIdxCoin = -1;

	for (int nCntCoin = 0; nCntCoin < MAX_COIN; nCntCoin++)
	{
		if (!g_Coin[nCntCoin].use)
		{
			g_Coin[nCntCoin].pos = pos;
			g_Coin[nCntCoin].rot = rot;
			g_Coin[nCntCoin].scl = { 1.0f, 1.0f, 1.0f };
			g_Coin[nCntCoin].use = true;

			nIdxCoin = nCntCoin;

			//	PlaySound(SOUND_LABEL_SE_shot000);

			break;
		}
	}

	return nIdxCoin;
}

