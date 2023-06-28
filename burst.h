//=============================================================================
//
// エフェクト処理 [burst.h]
// Author : 梅津　修也
//
//=============================================================================
#pragma once

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_BURST        (50)

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct BURST
{
    XMFLOAT3            pos;            // ポリゴンの位置
    XMFLOAT3            rot;            // ポリゴンの向き(回転)
    XMFLOAT3            scl;            // ポリゴンの大きさ(スケール)
    XMFLOAT3            move;           // プレイヤーの移動するベクトル
    XMFLOAT3            accel;          // 加速度
    MATERIAL            material;       // マテリアル

    XMFLOAT4X4          mtxWorld;       // ワールドマトリックス
    int                 lifetimeFrame;  // 寿命
    

    bool                ismodelLoaded;

    DX11_MODEL          model;          // モデル情報

    bool                use;

};


//**
// プロトタイプ宣言
//**
HRESULT InitBurst(void);
void UninitBurst(void);
void UpdateBurst(void);
void DrawBurst(void);

void SetBurst(XMFLOAT3 pos, int lifetimeFrame);