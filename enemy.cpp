//=============================================================================
//
// エネミーモデル処理 [enemy.cpp]
// Author : 梅津　修也
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "enemy.h"
#include "shadow.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ENEMY			"data/MODEL/toge.obj"		// 読み込むモデル名

#define	VALUE_MOVE			(5.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

#define ENEMY_SHADOW_SIZE	(0.4f)						// 影の大きさ
#define ENEMY_OFFSET_Y		(10.0f)						// エネミーの足元をあわせる


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];				// エネミー


int g_Enemy_load = 0;


//static INTERPOLATION_DATA move_tbl[] = {	// pos, rot, scl, frame
//	{ XMFLOAT3(   0.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*2 },
//	{ XMFLOAT3(-200.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*1 },
//	{ XMFLOAT3(-200.0f, ENEMY_OFFSET_Y, 200.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*0.5f },
//	{ XMFLOAT3(   0.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*2 },
//
//};

static int		g_Count;					// カウント

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		LoadModel(MODEL_ENEMY, &g_Enemy[i].model);
		g_Enemy[i].load = true;

		g_Enemy[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_Enemy[i].spd = 0.0f;			// 移動スピードクリア
		g_Enemy[i].size = ENEMY_SIZE;	// 当たり判定の大きさ

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Enemy[0].model, &g_Enemy[0].diffuse[0]);

		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

		g_Enemy[i].move_time = 0.0f;	// 線形補間用のタイマーをクリア
		g_Enemy[i].tbl_adr = NULL;		// 再生するアニメデータの先頭アドレスをセット
		g_Enemy[i].tbl_size = 0;		// 再生するアニメデータのレコード数をセット

		g_Enemy[i].use = false;			// true:生きてる

	}


	//// 0番だけ線形補間で動かしてみる
	//g_Enemy[0].move_time = 0.0f;		// 線形補間用のタイマーをクリア
	//g_Enemy[0].tbl_adr = move_tbl;		// 再生するアニメデータの先頭アドレスをセット
	//g_Enemy[0].tbl_size = sizeof(move_tbl) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
{

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model);
			g_Enemy[i].load = false;
		}

	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{
	// エネミーを動かく場合は、影も合わせて動かす事を忘れないようにね！
	for (int i = 0; i < MAX_ENEMY; i++)
	{

		if (g_Count < 600)
		{

			int E_flug = rand() % 10000;

			if (E_flug == 1)
			{
				SetEnemy(XMFLOAT3(-20.0f, ENEMY_OFFSET_Y, 400.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
			}
			if (E_flug == 2)
			{
				SetEnemy(XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 400.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
			}

			if (E_flug == 3)
			{
				SetEnemy(XMFLOAT3(20.0f, ENEMY_OFFSET_Y, 400.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
			}

		}

		if (g_Count > 600 && g_Count < 1800)
		{

			int E_flug = rand() % 10000;

			if (E_flug > 0 && E_flug < 3)
			{
				SetEnemy(XMFLOAT3(-20.0f, ENEMY_OFFSET_Y, 400.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
			}
			if (E_flug > 3 && E_flug < 6)
			{
				SetEnemy(XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 400.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
			}

			if (E_flug > 6 && E_flug < 9)
			{
				SetEnemy(XMFLOAT3(20.0f, ENEMY_OFFSET_Y, 400.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
			}

		}

		if (g_Count > 1800)
		{

			int E_flug = rand() % 10000;

			if (E_flug > 0 && E_flug < 4)
			{
				SetEnemy(XMFLOAT3(-20.0f, ENEMY_OFFSET_Y, 400.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
			}
			if (E_flug > 4 && E_flug <8)
			{
				SetEnemy(XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 400.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
			}

			if (E_flug > 8 && E_flug < 12)
			{
				SetEnemy(XMFLOAT3(20.0f, ENEMY_OFFSET_Y, 400.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
			}

		}

	

		if (g_Enemy[i].use == true)			// このエネミーが使われている？
		{	
			/*int gap = g_Enemy[i].pos.z - g_Enemy[i].pos.z;

			if (fabsf(gap) < 1)
			{
				g_Enemy[i].use = false;
			}*/

			g_Enemy[i].pos.z -= 5.0f;

			// Yes
			//if (g_Enemy[i].tbl_adr != NULL)	// 線形補間を実行する？
			//{								// 線形補間の処理
				//// 移動処理
				//int		index = (int)g_Enemy[i].move_time;
				//float	time = g_Enemy[i].move_time - index;
				//int		size = g_Enemy[i].tbl_size;

				//float dt = 1.0f / g_Enemy[i].tbl_adr[index].frame;	// 1フレームで進める時間
				//g_Enemy[i].move_time += dt;							// アニメーションの合計時間に足す

				//if (index > (size - 2))	// ゴールをオーバーしていたら、最初へ戻す
				//{
				//	g_Enemy[i].move_time = 0.0f;
				//	index = 0;
				//}

				//// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
				//XMVECTOR p1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].pos);	// 次の場所
				//XMVECTOR p0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].pos);	// 現在の場所
				//XMVECTOR vec = p1 - p0;
				//XMStoreFloat3(&g_Enemy[i].pos, p0 + vec * time);

				//// 回転を求める	R = StartX + (EndX - StartX) * 今の時間
				//XMVECTOR r1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].rot);	// 次の角度
				//XMVECTOR r0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].rot);	// 現在の角度
				//XMVECTOR rot = r1 - r0;
				//XMStoreFloat3(&g_Enemy[i].rot, r0 + rot * time);

				//// scaleを求める S = StartX + (EndX - StartX) * 今の時間
				//XMVECTOR s1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].scl);	// 次のScale
				//XMVECTOR s0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].scl);	// 現在のScale
				//XMVECTOR scl = s1 - s0;
				//XMStoreFloat3(&g_Enemy[i].scl, s0 + scl * time);

			//}

			// 影もプレイヤーの位置に合わせる
			XMFLOAT3 pos = g_Enemy[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(g_Enemy[i].shadowIdx, pos);

			if (g_Enemy[i].pos.z < -80)
			{
				g_Enemy[i].use = false;

			}

		}

		if (g_Enemy[i].use == false)
		{
			ReleaseShadow(g_Enemy[i].shadowIdx);
		}
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == false) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_Enemy[i].model);
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// エネミーの取得
//=============================================================================
ENEMY *GetEnemy(void)
{
	return &g_Enemy[0];
}

//=============================================================================
// エネミーのパラメータをセット
//=============================================================================
int SetEnemy(XMFLOAT3 pos, XMFLOAT3 rot)
{
	int nIdxEnemy = -1;

	for (int nCntEnemy = 0; nCntEnemy < MAX_ENEMY; nCntEnemy++)
	{
		if (!g_Enemy[nCntEnemy].use)
		{
			g_Enemy[nCntEnemy].pos = pos;
			g_Enemy[nCntEnemy].rot = rot;
			g_Enemy[nCntEnemy].scl = { 0.8f, 0.8f, 0.8f };
			g_Enemy[nCntEnemy].use = true;

			nIdxEnemy = nCntEnemy;

			//	PlaySound(SOUND_LABEL_SE_shot000);

			break;
		}
	}

	return nIdxEnemy;
}


//=============================================================================
// カウントを加算する
// 引数:add :追加する点数。マイナスも可能
//=============================================================================
void AddCount(int add)
{
	g_Count += add;
}
