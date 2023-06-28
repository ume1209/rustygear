//=============================================================================
//
// 弾発射処理 [bullet.cpp]
// Author : 梅津　修也
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "shadow.h"
#include "bullet.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(1)				// テクスチャの数

#define	BULLET_WIDTH		(20.0f)			// 頂点サイズ
#define	BULLET_HEIGHT		(20.0f)			// 頂点サイズ

#define	BULLET_SPEED		(10.0f)			// 弾の移動スピード


//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexBullet(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static BULLET						g_Bullet[MAX_BULLET];	// 木ワーク
static int							g_TexNo;				// テクスチャ番号
static bool							g_bAlpaTest;		// アルファテストON/OFF


static char *g_TextureName[] =
{
	"data/TEXTURE/nc257368.png",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBullet(void)
{
	MakeVertexBullet();

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// 弾ワークの初期化
	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{
		ZeroMemory(&g_Bullet[nCntBullet].material, sizeof(g_Bullet[nCntBullet].material));
		g_Bullet[nCntBullet].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Bullet[nCntBullet].pos = { 0.0f, 0.0f, 0.0f };
		g_Bullet[nCntBullet].scl = { 1.0f, 1.0f, 1.0f };
		g_Bullet[nCntBullet].spd = BULLET_SPEED;
		g_Bullet[nCntBullet].fWidth = BULLET_WIDTH;
		g_Bullet[nCntBullet].fHeight = BULLET_HEIGHT;
		g_Bullet[nCntBullet].use = false;

	}
	g_bAlpaTest = true;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBullet(void)
{
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{// テクスチャの解放
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if (g_VertexBuffer != NULL)
	{// 頂点バッファの解放
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateBullet(void)
{

	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (g_Bullet[i].use)
		{

			// 弾の移動処理
			g_Bullet[i].pos.x -= sinf(g_Bullet[i].rot.y) * g_Bullet[i].spd;
			g_Bullet[i].pos.z += cosf(g_Bullet[i].rot.y) * g_Bullet[i].spd;

			// 影の位置設定
			SetPositionShadow(g_Bullet[i].shadowIdx, XMFLOAT3(g_Bullet[i].pos.x, 0.1f, g_Bullet[i].pos.z));

			if (!g_Bullet[i].use)
				continue;

			g_Bullet[i].lifetimeframe--;

			if (g_Bullet[i].lifetimeframe < 0)
			{
				g_Bullet[i].use = false;
			}
			if (g_Bullet[i].use == false)
			{
				ReleaseShadow(g_Bullet[i].shadowIdx);
			}
		}

	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBullet(void)
{
	// αテスト設定
	if (g_bAlpaTest == true)
	{
		// αテストを有効に
		SetAlphaTestEnable(true);
	}

	// ライティングを無効
	SetLightEnable(false);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, mtxView;
	CAMERA* cam = GetCamera();
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (g_Bullet[i].use)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();
			
			// ビューマトリックスを取得
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			//mtxWorld = XMMatrixInverse(nullptr, mtxView);
			//mtxWorld.r[3].m128_f32[0] = 0.0f;
			//mtxWorld.r[3].m128_f32[1] = 0.0f;
			//mtxWorld.r[3].m128_f32[2] = 0.0f;

			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];


			// スケールを反映
			mtxScl = XMMatrixScaling(g_Bullet[i].scl.x, g_Bullet[i].scl.y, g_Bullet[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Bullet[i].pos.x, g_Bullet[i].pos.y, g_Bullet[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			// マテリアル設定
			SetMaterial(g_Bullet[i].material);

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ライティングを有効に
	SetLightEnable(true);

	// αテストを無効に
	SetAlphaTestEnable(false);
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexBullet(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// 頂点バッファに値をセットする
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = BULLET_WIDTH;
	float fHeight = BULLET_HEIGHT;

	// 頂点座標の設定
	vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// 拡散光の設定
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// 弾のパラメータをセット
//=============================================================================
int SetBullet(XMFLOAT3 pos, float fWidth, float fHeight, int lifetimeframe)
{
	int nIdxBullet = -1;

	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{
		if (!g_Bullet[nCntBullet].use)
		{
			pos.z += 25.0f;
			pos.x += 0.5f;
			pos.y -= 3.5f;
			g_Bullet[nCntBullet].pos = pos;
			g_Bullet[nCntBullet].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_Bullet[nCntBullet].fWidth = fWidth;
			g_Bullet[nCntBullet].fHeight = fHeight;

			g_Bullet[nCntBullet].use = true;
			g_Bullet[nCntBullet].lifetimeframe = lifetimeframe;

			// 影の設定
			g_Bullet[nCntBullet].shadowIdx = CreateShadow(g_Bullet[nCntBullet].pos, 0.5f, 0.5f);

			nIdxBullet = nCntBullet;

		//	PlaySound(SOUND_LABEL_SE_shot000);

			break;
		}
	}

	return nIdxBullet;
}

//=============================================================================
// 弾の取得
//=============================================================================
BULLET *GetBullet(void)
{
	return &(g_Bullet[0]);
}

//=============================================================================
// ベジェ曲線
//=============================================================================
XMFLOAT3 GetBezier(XMFLOAT3 initialPos, XMFLOAT3 middlePos, XMFLOAT3 endPos, float percentage) 
{
	XMFLOAT3 result = initialPos;
	float invP = 1 - percentage;
	//これか
	result.x = invP * invP * initialPos.x + invP * 2 * percentage * middlePos.x + percentage * percentage * endPos.x;
	result.y = invP * invP * initialPos.y + invP * 2 * percentage * middlePos.y + percentage * percentage * endPos.y;
	result.z = invP * invP * initialPos.z + invP * 2 * percentage * middlePos.z + percentage * percentage * endPos.z;
	//もしくは
	//XMVECTOR initial = XMLoadFloat3(initialPos);
	//XMVECTOR middle = XMLoadFloat3(middlePos);
	//XMVECTOR end = XMLoadFloat3(endPos);

	//initial = invP * invP * initial + invP * 2 * percentage * middle + percentage * percentage * end
	//	XMStoreFloat3(result, &initial);


	return result;

}