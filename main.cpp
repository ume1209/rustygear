//=============================================================================
//
// メイン処理 [main.cpp]
// Author : 梅津　修也
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "player.h"
#include "enemy.h"
#include "coin.h"
#include "shadow.h"
#include "light.h"
#include "meshfield.h"
#include "meshwall.h"
#include "tree.h"
#include "collision.h"
#include "bullet.h"
#include "score.h"
#include "P_hp.h"
#include "sound.h"
#include "title.h"
#include "result.h"
#include "tutorial.h"
#include "particle.h"
#include "burst.h"
#include "effect.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CLASS_NAME		"AppClass"			// ウインドウのクラス名
#define WINDOW_NAME		""		// ウインドウのキャプション名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

void CheckHit(void);


//*****************************************************************************
// グローバル変数:
//*****************************************************************************
long g_MouseX = 0;
long g_MouseY = 0;


#ifdef _DEBUG
int		g_CountFPS;							// FPSカウンタ
char	g_DebugStr[2048] = WINDOW_NAME;		// デバッグ文字表示用

#endif

int	g_Mode = MODE_TITLE;					// 起動時の画面を設定

//=============================================================================
// メイン関数
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）

	// 時間計測用
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		CLASS_NAME,
		NULL
	};
	HWND		hWnd;
	MSG			msg;
	
	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// ウィンドウの作成
	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																		// ウィンドウの左座標
		CW_USEDEFAULT,																		// ウィンドウの上座標
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,									// ウィンドウ横幅
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	// ウィンドウ縦幅
		NULL,
		NULL,
		hInstance,
		NULL);

	// 初期化処理(ウィンドウを作成してから行う)
	if(FAILED(Init(hInstance, hWnd, TRUE)))
	{
		return -1;
	}

	// フレームカウント初期化
	timeBeginPeriod(1);	// 分解能を設定
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// システム時刻をミリ秒単位で取得
	dwCurrentTime = dwFrameCount = 0;

	// ウインドウの表示(初期化処理の後に呼ばないと駄目)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// メッセージループ
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳と送出
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }
		else
		{
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1秒ごとに実行
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPSを測定した時刻を保存
				dwFrameCount = 0;							// カウントをクリア
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60秒ごとに実行
			{
				dwExecLastTime = dwCurrentTime;	// 処理した時刻を保存

#ifdef _DEBUG	// デバッグ版の時だけFPSを表示する
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif

				Update();			// 更新処理
				Draw();				// 描画処理

#ifdef _DEBUG	// デバッグ版の時だけ表示する
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;
				AddCount(1);
			}
		}
	}

	timeEndPeriod(1);				// 分解能を戻す

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// 終了処理
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// プロシージャ
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;

	case WM_MOUSEMOVE:
		g_MouseX = LOWORD(lParam);
		g_MouseY = HIWORD(lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	InitRenderer(hInstance, hWnd, bWindow);

	InitLight();

	InitCamera();

	// サウンドの初期化処理
	InitSound(hWnd);

	// 入力処理の初期化
	InitInput(hInstance, hWnd);

	//// タイトルの初期化
	//InitTitle();

	//// チュートリアルの初期化
	//InitTutorial();

	InitParticle();

	// フィールドの初期化
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 20, 200, 13.0f, 13.0f);

	// 壁の初期化
	/*InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);*/
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT + 100.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT - 100.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT + 120.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT - 120.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);

	/*InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);*/

	// 壁(裏側用の半透明)
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);

	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);

	// 影の初期化処理
	InitShadow();

	// プレイヤーの初期化
	InitPlayer();

	// エネミーの初期化
	InitEnemy();

	// コインの初期化
	InitCoin();


	//// 木を生やす
	InitTree();

	// 弾の初期化
	InitBullet();

	// スコアの初期化
	InitScore();

	// HPの初期化
	InitP_Hp();

	// リザルトの初期化
	InitResult();

	// ライトを有効化
	SetLightEnable(true);

	// 背面ポリゴンをカリング
	SetCullingMode(CULL_MODE_BACK);

	// 最初のモードをセット
	SetMode(g_Mode);	// ここはSetModeのままで！


	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{
	// 終了のモードをセット
	SetMode(MODE_MAX);

	// サウンドの終了処理
	UninitSound();

	// スコアの終了処理
	UninitScore();

	// HPの終了処理
	UninitP_Hp();

	// パーティクルの終了処理
	UninitParticle();

	// 爆発の終了処理
	UninitBurst();

	// 爆発の終了処理
	UninitEffect();

	// 弾の終了処理
	UninitBullet();

	// 木の終了処理
	UninitTree();

	// エネミーの終了処理
	UninitEnemy();

	// コインの終了処理
	UninitCoin();

	// プレイヤーの終了処理
	UninitPlayer();

	// 影の終了処理
	UninitShadow();

	// 壁の終了処理
	UninitMeshWall();

	// 地面の終了処理
	UninitMeshField();

	// カメラの終了処理
	UninitCamera();

	//入力の終了処理
	UninitInput();

	////タイトルの終了処理
	//UninitTitle();

	////チュートリアルの終了処理
	//UninitTutorial();

	//リザルトの終了処理
	UninitResult();

	// レンダラーの終了処理
	UninitRenderer();
}

//=============================================================================
// 更新処理
//=============================================================================
void Update(void)
{
	// 入力の更新処理
	UpdateInput();

	// カメラ更新
	UpdateCamera();


	// モードによって処理を分ける
	switch (g_Mode)
	{
	case MODE_TITLE:		// タイトル画面の更新
		UpdateTitle();
		break;

	case MODE_TUTORIAL:		// タイトル画面の更新
		UpdateTutorial();
		break;

	case MODE_GAME:			// ゲーム画面の更新

			// 地面処理の更新
		UpdateMeshField();

		// 壁処理の更新
		UpdateMeshWall();

		// パーティクルの更新
		UpdateParticle();

		// 爆発の更新
		UpdateBurst();

		// 爆発の更新
		UpdateEffect();

		// プレイヤーの更新処理
		UpdatePlayer();

		// エネミーの更新処理
		UpdateEnemy();

		// コインの更新処理
		UpdateCoin();

		// 木の更新処理
		UpdateTree();

		// 弾の更新処理
		UpdateBullet();

		// 影の更新処理
		UpdateShadow();

		// 当たり判定
		CheckHit();

		// スコアの更新処理
		UpdateScore();

		// HPの更新処理
		UpdateP_Hp();

		break;

	case MODE_RESULT:		// リザルト画面の更新
		UpdateResult();
		break;
	}


}

//=============================================================================
// 描画処理
//=============================================================================
void Draw(void)
{
	// バックバッファクリア
	Clear();

	// プレイヤー視点
	XMFLOAT3 pos = GetPlayer()->pos;
	pos.y = 0.0f;			// カメラ酔いを防ぐためにクリアしている
	SetCameraAT(pos);
	SetCamera();

	// モードによって処理を分ける
	switch (g_Mode)
	{
	case MODE_TITLE:		// タイトル画面の描画
		// 2Dの物を描画する処理
		// Z比較なし
		SetDepthEnable(false);

		// ライティングを無効
		SetLightEnable(false);

		DrawTitle();

		// ライティングを有効に
		SetLightEnable(true);

		// Z比較あり
		SetDepthEnable(true);
	
		break;
	case MODE_TUTORIAL:		// タイトル画面の描画
		// 2Dの物を描画する処理
		// Z比較なし
		SetDepthEnable(false);

		// ライティングを無効
		SetLightEnable(false);

		DrawTutorial();

		// ライティングを有効に
		SetLightEnable(true);

		// Z比較あり
		SetDepthEnable(true);

		break;
	case MODE_GAME:			// ゲーム画面の描画
			// 地面の描画処理
		DrawMeshField();

		// 影の描画処理
		DrawShadow();

		// プレイヤーの描画処理
		DrawPlayer();

		// エネミーの描画処理
		DrawEnemy();

		// コインの描画処理
		DrawCoin();

		// 弾の描画処理
		DrawBullet();

		// 壁の描画処理
		DrawMeshWall();

		// 木の描画処理
		DrawTree();

		// 爆発の描画処理
		DrawBurst();

		// 爆発の描画処理
		DrawEffect();
		// パーティクルの描画処理
		DrawParticle();

		// 2Dの物を描画する処理
		// Z比較なし
		SetDepthEnable(false);

		// ライティングを無効
		SetLightEnable(false);

		// スコアの描画処理
		DrawScore();
		// HPの描画処理
		DrawP_Hp();

		// ライティングを有効に
		SetLightEnable(true);

		// Z比較あり
		SetDepthEnable(true);

		break;
	case MODE_RESULT:		// リザルト画面の描画

		// 2Dの物を描画する処理
		// Z比較なし
		SetDepthEnable(false);

		// ライティングを無効
		SetLightEnable(false);

		DrawResult();

		// ライティングを有効に
		SetLightEnable(true);

		// Z比較あり
		SetDepthEnable(true);


		break;
	}

	



#ifdef _DEBUG
	// デバッグ表示
	DrawDebugProc();
#endif

	// バックバッファ、フロントバッファ入れ替え
	Present();
}


long GetMousePosX(void)
{
	return g_MouseX;
}


long GetMousePosY(void)
{
	return g_MouseY;
}


#ifdef _DEBUG
char* GetDebugStr(void)
{
	return g_DebugStr;
}
#endif

//=============================================================================
// モードの設定
//=============================================================================
void SetMode(int mode)
{
	// モードを変える前に全部メモリを解放しちゃう
	StopSound();			// まず曲を止める

	// モードを変える前に全部メモリを解放しちゃう

	// タイトル画面の終了処理
	UninitTitle();

	// チュートリアル画面の終了処理
	UninitTutorial();

	// パーティクルの終了処理
	UninitParticle();

	// 爆発の終了処理
	UninitBurst();

	// プレイヤーの終了処理
	UninitPlayer();

	// エネミーの終了処理
	UninitEnemy();

	// コインの終了処理
	UninitCoin();

	// バレットの終了処理
	UninitBullet();

	// 爆発の終了処理
	UninitBurst();

	// 爆発の終了処理
	UninitEffect();

	// シャドウの終了処理
	UninitShadow();

	// スコアの終了処理
	UninitScore();

	// HPの終了処理
	UninitP_Hp();

	// リザルトの終了処理
	UninitResult();


	g_Mode = mode;	// 次のモードをセットしている

	switch (g_Mode)
	{
	case MODE_TITLE:
		// タイトル画面の初期化
		InitTitle();
		PlaySound(SOUND_LABEL_BGM_title);
		break;

	case MODE_TUTORIAL:
		// チュートリアル画面の初期化
		InitTutorial();

		PlaySound(SOUND_LABEL_BGM_title);
		break;

	case MODE_GAME:
		// ゲーム画面の初期化
		InitParticle();
		InitBurst();
		InitEffect();
		InitShadow();
		InitPlayer();
		InitEnemy();
		InitCoin();

		InitBullet();
		InitScore();
		InitP_Hp();

		PlaySound(SOUND_LABEL_BGM_stage);
		break;

	case MODE_RESULT:
		InitResult();
		PlaySound(SOUND_LABEL_BGM_result);
		break;

	case MODE_MAX:
		break;
	}
}

//=============================================================================
// モードの取得
//=============================================================================
int GetMode(void)
{
	return g_Mode;
}



//=============================================================================
// 当たり判定処理
//=============================================================================
void CheckHit(void)
{
	ENEMY  *enemy  = GetEnemy();	// エネミーのポインターを初期化
	PLAYER *player = GetPlayer();	// プレイヤーのポインターを初期化
	COIN* coin = GetCoin();	// コインのポインターを初期化
	BULLET *bullet = GetBullet();	// 弾のポインターを初期化
	
	// プレイヤーと壁の当たり判定

	if (player->pos.x < MAP_LEFT + 10)
	{
		player->pos.x = 1 + MAP_LEFT + 10;
	}

	if (player->pos.x > MAP_RIGHT-10)
	{
		player->pos.x = -1 + MAP_RIGHT - 10;
	}

	// 敵とプレイヤーキャラ
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		//敵の有効フラグをチェックする
		if (enemy[i].use == false)
			continue;

		//BCの当たり判定
		if (CollisionBC(player->pos, enemy[i].pos, player->size, enemy[i].size))
		{
			// 敵に当たったらHP－1
			player->hp -= 1;
			AddP_Hp(-1);
			if (player->hp == 0)
			{
				////HPが0ならリザルト
				//SetMode(MODE_RESULT);
			}

			// 敵キャラクターは倒される
			enemy[i].use = false;
			

			for (int j = 0; j < 10; j++)
			{
				SetBurst(enemy[i].pos,30);
			}
			/*SetTree(enemy[i].pos,10.0f,10.0f,30);*/

			SetBurst(enemy[i].pos, 36);

		}
	}

	// コインとプレイヤーキャラ
	for (int i = 0; i < MAX_COIN; i++)
	{
		//敵の有効フラグをチェックする
		if (coin[i].use == false)
			continue;

		//BCの当たり判定
		if (CollisionBC(player->pos, coin[i].pos, player->size, coin[i].size))
		{
			// 敵キャラクターは倒される
			coin[i].use = false;
			

			XMFLOAT3 pos = coin[i].pos;
			pos.x += (rand()% 20) - 10;
			pos.y += (rand() % 10);

			for (int j = 0; j < 10; j++)
			{
				SetEffect(pos, 36);
			}
			/*SetTree(coin[i].pos, 10.0f, 10.0f, 30);*/

			SetEffect(coin[i].pos, 36);

			// スコアを足す
			AddScore(100);

		}
	}


	// プレイヤーの弾と敵
	for (int i = 0; i < MAX_BULLET; i++)
	{
		//弾の有効フラグをチェックする
		if (bullet[i].use == false)
			continue;

		// 敵と当たってるか調べる
		for (int j = 0; j < MAX_ENEMY; j++)
		{
			//敵の有効フラグをチェックする
			if (enemy[j].use == false)
				continue;

			//BCの当たり判定
			if (CollisionBC(bullet[i].pos, enemy[j].pos, bullet[i].fWidth, enemy[j].size))
			{

				// 敵キャラクターは倒される
				enemy[j].use = false;
				ReleaseShadow(enemy[j].shadowIdx);

				// スコアを足す
				AddScore(10);
			}
		}

	}


	//// エネミーが全部死亡したら状態遷移
	//int enemy_count = 0;
	//for (int i = 0; i < MAX_ENEMY; i++)
	//{
	//	if (enemy[i].use == false)
	//	{
	//		continue;
	//	}
	//	enemy_count++;
	//}

	//// エネミーが０匹？
	//if (enemy_count == 0)
	//{
	//	 SetMode(MODE_RESULT);

	//}

}

