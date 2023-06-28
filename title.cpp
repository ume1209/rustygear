//=============================================================================
//
// タイトル画面処理 [title.cpp]
// Author : 梅津　修也
//
//=============================================================================
#include "title.h"
#include "input.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(7)				// テクスチャの数
#define BUTTON_MAX					(2)				// ボタンの数

#define TEXTURE_WIDTH_LOGO			(480)			// ロゴサイズ
#define TEXTURE_HEIGHT_LOGO			(80)			// 


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/title.png",
	"data/TEXTURE/title_name.png",
	"data/TEXTURE/effect000.jpg",
	"data/TEXTURE/yazirusi.png",
	"data/TEXTURE/gamestart.png",
	"data/TEXTURE/exit.png",
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

float	alpha;
BOOL	flag_alpha;

static BOOL						g_Load = FALSE;
static BUTTON					g_Button[BUTTON_MAX];
static SELECTOR					g_Selector;

static float	effect_dx;
static float	effect_dy;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTitle(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// 変数の初期化
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = XMFLOAT3(g_w/2, g_h/2, 0.0f);
	g_TexNo = 0;

	alpha = 1.0f;
	flag_alpha = TRUE;

	effect_dx = 100.0f;
	effect_dy = 100.0f;





	for (int i = 0; i < BUTTON_MAX; i++)
	{
		g_Button[i].texNo = 4 + i;
		g_Button[i].pos = XMFLOAT3(480.0f, 350.0f + (100 * i), 0.0f);
		g_Button[i].h = 75;
		g_Button[i].w = 175;
		g_Button[i].alpha = 1.0f;
	}


	g_Selector.texNo = 3;
	g_Selector.pos = XMFLOAT3(350.0f, 200.0f, 0.0f);
	g_Selector.h = 50;
	g_Selector.w = 50;


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTitle(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTitle(void)
{

	if (GetKeyboardTrigger(DIK_RETURN) || (IsButtonTriggered(0, BUTTON_B)))
	{// Enter押したら、ステージを切り替える
		g_Button[g_Button[0].selector % BUTTON_MAX].alpha = 0.5f;
		
		switch (g_Button[0].selector % BUTTON_MAX)
		{
		case 0:
			SetMode(MODE_TUTORIAL);
			break;

		case 1:
			exit(-1);
			break;

		default:
			break;
		}
	}

	//タイトル画面での入力処理
	else if (GetKeyboardTrigger(DIK_DOWNARROW) || IsButtonTriggered(0, BUTTON_UP))
	{
		g_Button[0].selector++;
	}

	else if (GetKeyboardTrigger(DIK_UPARROW) || IsButtonTriggered(0, BUTTON_DOWN))
	{
		g_Button[0].selector += BUTTON_MAX - 1;
	}
	

	// セーブデータをロードする？
	/*if (GetKeyboardTrigger(DIK_L))
	{
		SetLoadGame(TRUE);
		SetMode(MODE_GAME);
	}*/

	g_Selector.pos.y = g_Button[g_Button[0].selector % BUTTON_MAX].pos.y;

	// テストでエフェクトの発生場所を移動させる
	float speed = 4.0f;

	if (GetKeyboardPress(DIK_DOWN))
	{
		effect_dy += speed;
	}
	else if (GetKeyboardPress(DIK_UP))
	{
		effect_dy -= speed;
	}

	if (GetKeyboardPress(DIK_RIGHT))
	{
		effect_dx += speed;
	}
	else if (GetKeyboardPress(DIK_LEFT))
	{
		effect_dx -= speed;
	}


#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc("Player:↑ → ↓ ←　Space\n");

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTitle(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// タイトルの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	{
		for (int i = 0; i < BUTTON_MAX; i++)
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Button[i].texNo]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(g_VertexBuffer, g_Button[i].pos.x, g_Button[i].pos.y, g_Button[i].w, g_Button[i].h,
				0.0f, 0.0f, 1.0f, 1.0f,XMFLOAT4(1.0f,1.0f,1.0f,g_Button[i].alpha));
			
			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);		//指定した画像を描写

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer,470.0f, 100.0f, 600.0f, 200.0f,
			0.0f, 0.0f, 1.0f, 1.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);		//指定した画像を描写

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, g_Selector.pos.x, g_Selector.pos.y, g_Selector.w, g_Selector.h,
			0.0f, 0.0f, 1.0f, 1.0f, XMFLOAT4(1.0f, 1.0f, 1.0f,1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	

//	// 加減算のテスト
//	SetBlendState(BLEND_MODE_ADD);		// 加算合成
////	SetBlendState(BLEND_MODE_SUBTRACT);	// 減算合成
//		
//	// テクスチャ設定
//	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);
//	
//	for (int i = 0; i < 30; i++)
//	{
//		// １枚のポリゴンの頂点とテクスチャ座標を設定
//		float dx = effect_dx;
//		float dy = effect_dy;
//		float sx = (float)(rand() % 100);
//		float sy = (float)(rand() % 100);
//
//		SetSpriteColor(g_VertexBuffer, dx + sx, dy + sy, 50, 50, 0.0f, 0.0f, 1.0f, 1.0f,
//			XMFLOAT4(1.0f, 0.3f, 1.0f, 0.5f));
//
//		// ポリゴン描画
//		GetDeviceContext()->Draw(4, 0);
//	}
//	SetBlendState(BLEND_MODE_ALPHABLEND);	// 半透明処理を元に戻す


}





