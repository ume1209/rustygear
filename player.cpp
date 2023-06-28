//=============================================================================
//
// モデル処理 [player.cpp]
// Author : 梅津　修也
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "player.h"
#include "shadow.h"
#include "light.h"
#include "bullet.h"
#include "sound.h"
#include "sprite.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_PLAYER			"data/MODEL/AC_Body.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_PARTS_0	"data/MODEL/AC_L_Arm.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_PARTS_1	"data/MODEL/AC_R_Arm.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_PARTS_2	"data/MODEL/AC_L_UL.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_PARTS_3	"data/MODEL/AC_R_UL.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_PARTS_4	"data/MODEL/AC_L_LL.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_PARTS_5	"data/MODEL/AC_R_LL.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_PARTS_6	"data/MODEL/Rifle.obj"			// 読み込むモデル名


//#define	MODEL_PLAYER_PARTS_6	"data/MODEL/AC_R_Head.obj"			// 読み込むモデル名


#define	VALUE_MOVE			(2.0f)							// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// 回転量

#define PLAYER_SHADOW_SIZE	(1.0f)							// 影の大きさ
#define PLAYER_OFFSET_Y		(19.0f)							// プレイヤーの足元をあわせる

#define PLAYER_PARTS_MAX	(7)								// プレイヤーのパーツの数

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
enum STATE {
	taiki = 0,
	move_R,
	move_L,
	shot
};

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PLAYER		g_Player;						// プレイヤー

static PLAYER		g_Parts[PLAYER_PARTS_MAX];		// プレイヤーのパーツ用

static float		floatidx;

static STATE		g_playerState = taiki;

// プレイヤーの階層アニメーションデータ
// プレイヤーの頭を左右に動かしているアニメデータ
static PARTDATA stateListArm_R[] = {
	{XMFLOAT3(8.0f, 6.0f, 0.0f), XMFLOAT3(-30.0f * XM_PI / 180, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f)},
	{XMFLOAT3(8.0f, 6.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f)},
	{XMFLOAT3(8.0f, 6.0f, 0.0f), XMFLOAT3(-80.0f * XM_PI / 180, -40.0f * XM_PI / 180, 0.0f),XMFLOAT3(1.0f, 1.0f, 1.0f)}
};

static PARTDATA stateListArm_L[] = {
	{XMFLOAT3(-8.0f, 6.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -10.0f * XM_PI / 180), XMFLOAT3(1.0f, 1.0f, 1.0f)},
	{XMFLOAT3(-8.0f, 6.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -10.0f * XM_PI / 180), XMFLOAT3(1.0f, 1.0f, 1.0f)},
	{XMFLOAT3(-8.0f, 6.0f, 0.0f), XMFLOAT3(-80.0f * XM_PI / 180, 40.0f * XM_PI / 180, 0.0f),XMFLOAT3(1.0f, 1.0f, 1.0f)}
};

static PARTDATA stateListULeg_R[] = {
	{XMFLOAT3(3.5f, -4.0f, 0.0f), XMFLOAT3(0.0f, 0.0f,15.0f * XM_PI / 180),         XMFLOAT3(1.0f, 1.0f, 1.0f)},
	{XMFLOAT3(3.5f, -4.0f, 0.0f), XMFLOAT3(-60.0f * XM_PI / 180, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f)},
	{XMFLOAT3(3.5f, -4.0f, 0.0f), XMFLOAT3(0.0f, 0.0f,15.0f * XM_PI / 180),         XMFLOAT3(1.0f, 1.0f, 1.0f)},
};

static PARTDATA stateListULeg_L[] = {
	{XMFLOAT3(-3.5f, -4.0f, 0.0f), XMFLOAT3(0.0f, 0.0f,-15.0f * XM_PI / 180),         XMFLOAT3(1.0f, 1.0f, 1.0f)},
	{XMFLOAT3(-3.5f, -4.0f, 0.0f), XMFLOAT3(-60.0f * XM_PI / 180, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f)},
	{XMFLOAT3(-3.5f, -4.0f, 0.0f), XMFLOAT3(0.0f, 0.0f,-15.0f * XM_PI / 180),         XMFLOAT3(1.0f, 1.0f, 1.0f)},
};

static PARTDATA stateListLLeg_R[] = {
	{ XMFLOAT3(2.0f, -9.0f, 0.0f), XMFLOAT3(0.0f, 0.0f,  0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f)},
	{ XMFLOAT3(2.0f, -9.0f, 0.0f), XMFLOAT3(100.0f * XM_PI / 180, 0.0f, 0.0f),XMFLOAT3(1.0f, 1.0f, 1.0f)},
	{ XMFLOAT3(2.0f, -9.0f, 0.0f), XMFLOAT3(0.0f, 0.0f,  0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f)},
};

static PARTDATA stateListLLeg_L[] = {
	{ XMFLOAT3(-2.0f, -9.0f, 0.0f), XMFLOAT3(0.0f, 0.0f,  0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f)},
	{ XMFLOAT3(-2.0f, -9.0f, 0.0f), XMFLOAT3(100.0f * XM_PI / 180, 0.0f, 0.0f),XMFLOAT3(1.0f, 1.0f, 1.0f)},
	{ XMFLOAT3(-2.0f, -9.0f, 0.0f), XMFLOAT3(0.0f, 0.0f,  0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f)},
};

static PARTDATA stateListRIfle[] = {
	{ XMFLOAT3(2.0f, -13.0f, 3.0f), XMFLOAT3(0.0f, 90.0f * XM_PI / 180,  60.0f * XM_PI / 180),         XMFLOAT3(1.8f, 1.8f, 1.8f)},
	{ XMFLOAT3(2.0f, -13.0f, 3.0f), XMFLOAT3(0.0f, 90.0f * XM_PI / 180,  60.0f * XM_PI / 180),         XMFLOAT3(1.8f, 1.8f, 1.8f) },
	{ XMFLOAT3(2.0f, -13.0f, 3.0f), XMFLOAT3(-40.0f * XM_PI / 180, 85.0f * XM_PI / 180,  90.0f * XM_PI / 180),         XMFLOAT3(1.8f, 1.8f, 1.8f) },
};

static PARTDATA* g_Player_Part[] = {
	stateListArm_R,
	stateListArm_L,
	stateListULeg_R,
	stateListULeg_L,
	stateListLLeg_R,
	stateListLLeg_L,
	stateListRIfle
};



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{


	LoadModel(MODEL_PLAYER, &g_Player.model);
	g_Player.load = true;

	g_Player.pos = { 0.0f, PLAYER_OFFSET_Y, -10.0f };
	g_Player.rot = { 0.0f, 0.0f, 0.0f };
	g_Player.scl = { 0.6f, 0.6f, 0.6f };

	g_Player.spd = 0.0f;			// 移動スピードクリア
	g_Player.size = PLAYER_SIZE;	// 当たり判定の大きさ
	
	g_Player.hp_max = 3;
	g_Player.hp = 3;
	g_Player.use = true;

	// ここでプレイヤー用の影を作成している
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE/2, PLAYER_SHADOW_SIZE/2);
	//          ↑
	//        このメンバー変数が生成した影のIndex番号



	// 階層アニメーション用の初期化処理
	g_Player.parent = NULL;			// 本体（親）なのでNULLを入れる

	// パーツの初期化
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		g_Parts[i].use = false;

		// 位置・回転・スケールの初期設定
		g_Parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);	// 親の原点からのオフセット座標
		g_Parts[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);	// 自分の回転情報
		g_Parts[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);	// 自分の拡大縮小

		// 親子関係
		g_Parts[i].parent = &g_Player;		// ← ここに親のアドレスを入れる
		g_Parts[0].parent = &g_Player;		// 腕だったら親は本体（プレイヤー）
		g_Parts[1].parent = &g_Player;
		g_Parts[2].parent = &g_Player;
		g_Parts[3].parent = &g_Player;
		g_Parts[4].parent = &g_Parts[2];
		g_Parts[5].parent = &g_Parts[3];
		g_Parts[6].parent = &g_Player;

		//g_Parts[6].parent = &g_Player;
	//	g_Parts[手].parent= &g_Paerts[腕];	// 指が腕の子供だった場合の例


		// 階層アニメーション用のメンバー変数の初期化
		g_Parts[i].time = 0.0f;			// 線形補間用のタイマーをクリア
		g_Parts[i].tblNo = 0;			// 再生する行動データテーブルNoをセット
		g_Parts[i].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット

		// パーツの読み込みはまだしていない
		g_Parts[i].load = 0;
	}

	// 右上腕パーツの設定
	LoadModel(MODEL_PLAYER_PARTS_0, &g_Parts[0].model);
	g_Parts[0].use = true;
	g_Parts[0].load = true;
	g_Parts[0].parent = &g_Player;

	// 左上腕パーツの設定
	LoadModel(MODEL_PLAYER_PARTS_1, &g_Parts[1].model);
	g_Parts[1].use = true;
	g_Parts[1].load = true;
	g_Parts[1].parent = &g_Player;

	// 右上脚パーツの設定
	LoadModel(MODEL_PLAYER_PARTS_2, &g_Parts[2].model);
	g_Parts[2].use = true;
	g_Parts[2].load = true;
	g_Parts[2].parent = &g_Player;

	// 左上脚パーツの設定
	LoadModel(MODEL_PLAYER_PARTS_3, &g_Parts[3].model);
	g_Parts[3].use = true;
	g_Parts[3].load = true;
	g_Parts[3].parent = &g_Player;

	// 右上脚パーツの設定
	LoadModel(MODEL_PLAYER_PARTS_4, &g_Parts[4].model);
	g_Parts[4].use = true;
	g_Parts[4].load = true;
	g_Parts[4].parent = &g_Parts[2];

	// 左上脚パーツの設定
	LoadModel(MODEL_PLAYER_PARTS_5, &g_Parts[5].model);
	g_Parts[5].use = true;
	g_Parts[5].load = true;
	g_Parts[5].parent = &g_Parts[3];

	// ライフルパーツの設定
	LoadModel(MODEL_PLAYER_PARTS_6, &g_Parts[6].model);
	g_Parts[6].use = true;
	g_Parts[6].load = true;
	g_Parts[6].parent = &g_Parts[0];

	//// 頭パーツの設定
	//LoadModel(MODEL_PLAYER_PARTS_6, &g_Parts[6].model);
	//g_Parts[6].tblNo = 6;			// 再生する行動データテーブルNoをセット
	//g_Parts[6].tblMax = sizeof(move_tbl_Head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	//g_Parts[6].use = true;
	//g_Parts[6].load = true;
	//g_Parts[6].parent = &g_Player;


	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	// モデルの解放処理
	if (g_Player.load)
	{
		UnloadModel(&g_Player.model);
		g_Player.load = false;
	}

	// パーツの解放処理
	{
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	CAMERA *cam = GetCamera();
	bool isMove = false;
	int Count = 0;
	g_playerState = taiki;
	g_Player.rot.x = 15 * XM_PI / 180;
	g_Player.rot.z = 0;
	g_Player.rot.y =180 * XM_PI / 180;
	g_Player.pos.y += sinf(floatidx) * 0.1f;
	floatidx += XM_PI / 50;

	if(g_Player.hp == 1)
		SetModelDiffuse(&g_Player.model, 1, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	// 移動させちゃう
	if (GetKeyboardPress(DIK_LEFT))
	{	// 左へ移動
		g_Player.spd = 2;
		g_Player.rot.z = 15 * XM_PI / 180;
		g_Player.pos.x -= g_Player.spd;
		g_playerState = move_L;
	}
	

	if (GetKeyboardPress(DIK_RIGHT))
	{	// 右へ移動
		g_Player.spd = 2;
		g_Player.rot.z = -15 * XM_PI / 180;
		g_Player.pos.x += g_Player.spd;
		g_playerState = move_R;

	}
	

	//if (GetKeyboardPress(DIK_UP))
	//{	// 上へ移動
	//	g_Player.spd = VALUE_MOVE;
	//	g_Player.dir = XM_PI;
	//}
	//if (GetKeyboardPress(DIK_DOWN))
	//{	// 下へ移動
	//	g_Player.spd = VALUE_MOVE;
	//	g_Player.dir = 0.0f;
	//}


#ifdef _DEBUG
	/*if (GetKeyboardPress(DIK_R))
	{
		g_Player.pos.z = g_Player.pos.x = 0.0f;
		g_Player.rot.y = g_Player.dir = 0.0f;
		g_Player.spd = 0.0f;
	}*/
#endif


	//	// Key入力があったら移動処理する
	if (g_Player.spd > 1.0f)
	{
		isMove = true;

		//g_Player.rot.y = g_Player.dir + cam->rot.y;

		//// 入力のあった方向へプレイヤーを向かせて移動させる
		//g_Player.pos.x -= sinf(g_Player.rot.y) * g_Player.spd;
		//g_Player.pos.z -= cosf(g_Player.rot.y) * g_Player.spd;
	}

	// 影もプレイヤーの位置に合わせる
	XMFLOAT3 pos = g_Player.pos;
	pos.y -=
		(PLAYER_OFFSET_Y - 0.1f);
	SetPositionShadow(g_Player.shadowIdx, pos);

	// 弾発射処理
	if (GetKeyboardPress(DIK_SPACE))
	{
		g_playerState = shot;
		g_Player.rot.x = -5 * XM_PI / 180;

		SetBullet(g_Player.pos, 5.0f,5.0f,1000);
		PlaySound(SOUND_LABEL_SE_shot000);
	}

	g_Player.spd *= 0.5f;


	// 階層アニメーション
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// 使われているなら処理する
		if (g_Parts[i].use == true)
		{	
			PARTDATA* tbl = g_Player_Part[i];	// 行動テーブルのアドレスを取得
			
			switch (g_playerState)
			{
			case taiki:
				g_Parts[i].pos = tbl[taiki].pos;
				g_Parts[i].rot = tbl[taiki].rot;
				g_Parts[i].scl = tbl[taiki].scl;
				break;

			case move_R:
				if (i % 2 == 0)
				{
					g_Parts[i].pos = tbl[1].pos;
					g_Parts[i].rot = tbl[1].rot;
					g_Parts[i].scl = tbl[1].scl;
				}
				break;

			case move_L:
				if (i % 2 == 1)
				{
					g_Parts[i].pos = tbl[1].pos;
					g_Parts[i].rot = tbl[1].rot;
					g_Parts[i].scl = tbl[1].scl;
				}
				break;

			case shot:
				g_Parts[i].pos = tbl[shot - 1].pos;
				g_Parts[i].rot = tbl[shot - 1].rot;
				g_Parts[i].scl = tbl[shot - 1].scl;
				break;

			//default:
			//	g_playerState = taiki;
			}
			



			// 線形補間の処理
		//	int nowNo = (int)g_Parts[i].time;			// 整数分であるテーブル番号を取り出している
		//	int maxNo = g_Parts[i].tblMax;				// 登録テーブル数を数えている
		//	int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
		//	INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Parts[i].tblNo];	// 行動テーブルのアドレスを取得

		//	XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
		//	XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
		//	XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

		//	XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
		//	XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
		//	XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

		//	float nowTime = g_Parts[i].time - nowNo;	// 時間部分である少数を取り出している

		//	Pos *= nowTime;								// 現在の移動量を計算している
		//	Rot *= nowTime;								// 現在の回転量を計算している
		//	Scl *= nowTime;								// 現在の拡大率を計算している

		//	// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
		//	XMStoreFloat3(&g_Parts[i].pos, nowPos + Pos);

		//	// 計算して求めた回転量を現在の移動テーブルに足している
		//	XMStoreFloat3(&g_Parts[i].rot, nowRot + Rot);

		//	// 計算して求めた拡大率を現在の移動テーブルに足している
		//	XMStoreFloat3(&g_Parts[i].scl, nowScl + Scl);

		//	// frameを使て時間経過処理をする
		//	if (isMove == true)
		//	{
		//		g_Parts[i].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
		//	}
		//	//else
		//	//{
		//	//	g_Parts[i].time = 0.0f;	// 時間を進めている
		//	//}

		//	if ((int)g_Parts[i].time >= maxNo)			// 登録テーブル最後まで移動したか？
		//	{
		//		g_Parts[i].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
		//	}

		}

	}



	{	// ポイントライトのテスト
		LIGHT *light = GetLightData(1);
		XMFLOAT3 pos = g_Player.pos;
		pos.y += 20.0f;

		light->Position = pos;
		light->Diffuse = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
		light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Type = LIGHT_TYPE_POINT;
		light->Enable = true;
		SetLightData(1, light);
	}
	Count++;

#ifdef _DEBUG	// デバッグ情報を表示する
	/*PrintDebugProc("Player:↑ → ↓ ←　Space\n");
	PrintDebugProc("Player:X:%f Y:%f Z:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);*/
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y + XM_PI, g_Player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);
	

	// モデル描画
	DrawModel(&g_Player.model);



	// パーツの階層アニメーション
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// パーツ自身のSRTを行う
		// 親か子かチェックする
		// 子の場合、親と子のmtxWorldを掛ける
		// 積を使って子を描画する

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Parts[i].scl.x, g_Parts[i].scl.y, g_Parts[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i].rot.x, g_Parts[i].rot.y, g_Parts[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Parts[i].pos.x, g_Parts[i].pos.y, g_Parts[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		if (g_Parts[i].parent != NULL)	// 子供だったら親と結合する
		{
			// 親のmtxWorldはどこにあるか
			//g_Parts[i].parent->mtxWorld = g_Player.mtxWorld;
			//g_Parts[i].parent = &g_Player;
			//g_Player.mtxWorldq;

			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i].parent->mtxWorld));
			// ↑
			// g_Player.mtxWorldを指している
		}

		XMStoreFloat4x4(&g_Parts[i].mtxWorld, mtxWorld);

		// 使われているなら処理する。ここまで処理している理由は他のパーツがこのパーツを参照している可能性があるから。
		if (g_Parts[i].use == false) continue;

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);


		// モデル描画
		DrawModel(&g_Parts[i].model);

	}


	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// プレイヤー情報を取得
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player;
}

