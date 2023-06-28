//=============================================================================
//
// �X�R�A���� [P_hp.cpp]
// Author : �~�Á@�C��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "P_hp.h"
#include "sprite.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(16)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(32)	// 
#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[] = {
	"data/TEXTURE/number16x32.png",
	"data/TEXTURE/hp.png",
	
};


static bool						g_Use[TEXTURE_MAX];						// true:�g���Ă���  false:���g�p
static float					g_w[TEXTURE_MAX], g_h[TEXTURE_MAX];					// ���ƍ���
static XMFLOAT3					g_Pos[TEXTURE_MAX];						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

static int						g_P_Hp[TEXTURE_MAX];					// �X�R�A

//=============================================================================
// ����������
//=============================================================================
HRESULT InitP_Hp(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
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


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		// �v���C���[�̏�����
		g_Use[i] = true;
		g_w[i] = TEXTURE_WIDTH;
		g_h[i] = TEXTURE_HEIGHT;
		g_Pos[0] = { 770.0f, 20.0f, 0.0f };
		g_TexNo = 0;

		g_P_Hp[i] = 3;	// �X�R�A�̏�����
	}
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitP_Hp(void)
{
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

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateP_Hp(void)
{


#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawP_Hp(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	// ��������������
	int number = g_P_Hp[0];
	for (int i = 0; i < P_HP_DIGIT; i++)
	{
		// ����\�����錅�̐���
		float x = (float)(number % 10);

		// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Pos[i].x - g_w[i]*i;	// �X�R�A�̕\���ʒuX
		float py = g_Pos[i].y;			// �X�R�A�̕\���ʒuY
		float pw = g_w[i];				// �X�R�A�̕\����
		float ph = g_h[i];				// �X�R�A�̕\������

		float tw = 1.0f / 10;		// �e�N�X�`���̕�
		float th = 1.0f / 1;		// �e�N�X�`���̍���
		float tx = x * tw;			// �e�N�X�`���̍���X���W
		float ty = 0.0f;			// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		// ���̌���
		number /= 10;
	}
	
	
}


//=============================================================================
// �X�R�A�����Z����
// ����:add :�ǉ�����_���B�}�C�i�X���\
//=============================================================================
void AddP_Hp(int add)
{
	g_P_Hp[0] += add;
	if (g_P_Hp[0] > P_HP_MAX)
	{
		g_P_Hp[0] = P_HP_MAX;
	}

}


int GetP_Hp(void)
{
	return g_P_Hp[0];
}

