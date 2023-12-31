//=============================================================================
//
// エネミーモデル処理 [coin.h]
// Author : 梅津　修也
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_COIN		(30)					// エネミーの数

#define	COIN_SIZE		(9.0f)				// 当たり判定の大きさ


//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct COIN
{
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)

	bool				use;
	bool				load;
	DX11_MODEL			model;				// モデル情報
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色

	float				spd;				// 移動スピード
	float				size;				// 当たり判定の大きさ
	int					shadowIdx;			// 影のインデックス番号
	
	INTERPOLATION_DATA	*tbl_adr;			// アニメデータのテーブル先頭アドレス
	int					tbl_size;			// 登録したテーブルのレコード総数
	float				move_time;			// 実行時間

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitCoin(void);
void UninitCoin(void);
void UpdateCoin(void);
void DrawCoin(void);

int SetCoin(XMFLOAT3 pos, XMFLOAT3 rot);

COIN *GetCoin(void);

