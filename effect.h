#pragma once
//=============================================================================
//
// エフェクト処理 [effect.h]
// Author : 梅津修也
//
//=============================================================================
#pragma once

//**
// マクロ定義
//**
#define MAX_EFFECT        (50)

//**
// 構造体定義
//**
struct EFFECT
{
    XMFLOAT3            pos;                    // ポリゴンの位置
    XMFLOAT3            rot;                    // ポリゴンの向き(回転)
    XMFLOAT3            scl;                    // ポリゴンの大きさ(スケール)
    XMFLOAT3            move;                    // プレイヤーの移動するベクトル
    MATERIAL            material;                // マテリアル


    XMFLOAT4X4            mtxWorld;                // ワールドマトリックス
    int                    lifetimeFrame;            // 寿命

    bool                ismodelLoaded;

    DX11_MODEL            model;                    // モデル情報

    bool                use;

};


//**
// プロトタイプ宣言
//**
HRESULT InitEffect(void);
void UninitEffect(void);
void UpdateEffect(void);
void DrawEffect(void);

void SetEffect(XMFLOAT3 pos, int lifetimeFrame);