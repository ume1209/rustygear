//=============================================================================
//
// �G�l�~�[���f������ [coin.cpp]
// Author : �~�Á@�C��
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
// �}�N����`
//*****************************************************************************
#define	MODEL_COIN			"data/MODEL/coin.obj"		// �ǂݍ��ރ��f����

#define	VALUE_MOVE			(5.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define COIN_SHADOW_SIZE	(0.4f)						// �e�̑傫��
#define COIN_OFFSET_Y		(10.0f)						// �G�l�~�[�̑��������킹��


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static COIN			g_Coin[MAX_COIN];				// �G�l�~�[


int g_Coin_load = 0;


static INTERPOLATION_DATA move_tbl[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(   0.0f, COIN_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*2 },
	{ XMFLOAT3(-200.0f, COIN_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*1 },
	{ XMFLOAT3(-200.0f, COIN_OFFSET_Y, 200.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*0.5f },
	{ XMFLOAT3(   0.0f, COIN_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*2 },

};

static int		g_Count;					// �J�E���g

//=============================================================================
// ����������
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

		g_Coin[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Coin[i].size = COIN_SIZE;	// �����蔻��̑傫��

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Coin[0].model, &g_Coin[0].diffuse[0]);

		XMFLOAT3 pos = g_Coin[i].pos;
		pos.y -= (COIN_OFFSET_Y - 0.1f);
		g_Coin[i].shadowIdx = CreateShadow(pos, COIN_SHADOW_SIZE, COIN_SHADOW_SIZE);

		g_Coin[i].move_time = 0.0f;	// ���`��ԗp�̃^�C�}�[���N���A
		g_Coin[i].tbl_adr = NULL;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Coin[i].tbl_size = 0;		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		g_Coin[i].use = false;			// true:�����Ă�

	}


	//// 0�Ԃ������`��Ԃœ������Ă݂�
	//g_Coin[0].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	//g_Coin[0].tbl_adr = move_tbl;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	//g_Coin[0].tbl_size = sizeof(move_tbl) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	return S_OK;
}

//=============================================================================
// �I������
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
// �X�V����
//=============================================================================
void UpdateCoin(void)
{
	// �G�l�~�[�𓮂����ꍇ�́A�e�����킹�ē���������Y��Ȃ��悤�ɂˁI
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
	

		if (g_Coin[i].use == true)			// ���̃G�l�~�[���g���Ă���H
		{	
			g_Coin[i].pos.z -= 5.0f;
			g_Coin[i].rot.y += VALUE_ROTATE;
			// Yes
			//if (g_Coin[i].tbl_adr != NULL)	// ���`��Ԃ����s����H
			//{								// ���`��Ԃ̏���
				//// �ړ�����
				//int		index = (int)g_Coin[i].move_time;
				//float	time = g_Coin[i].move_time - index;
				//int		size = g_Coin[i].tbl_size;

				//float dt = 1.0f / g_Coin[i].tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
				//g_Coin[i].move_time += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

				//if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
				//{
				//	g_Coin[i].move_time = 0.0f;
				//	index = 0;
				//}

				//// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
				//XMVECTOR p1 = XMLoadFloat3(&g_Coin[i].tbl_adr[index + 1].pos);	// ���̏ꏊ
				//XMVECTOR p0 = XMLoadFloat3(&g_Coin[i].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
				//XMVECTOR vec = p1 - p0;
				//XMStoreFloat3(&g_Coin[i].pos, p0 + vec * time);

				//// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
				//XMVECTOR r1 = XMLoadFloat3(&g_Coin[i].tbl_adr[index + 1].rot);	// ���̊p�x
				//XMVECTOR r0 = XMLoadFloat3(&g_Coin[i].tbl_adr[index + 0].rot);	// ���݂̊p�x
				//XMVECTOR rot = r1 - r0;
				//XMStoreFloat3(&g_Coin[i].rot, r0 + rot * time);

				//// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
				//XMVECTOR s1 = XMLoadFloat3(&g_Coin[i].tbl_adr[index + 1].scl);	// ����Scale
				//XMVECTOR s0 = XMLoadFloat3(&g_Coin[i].tbl_adr[index + 0].scl);	// ���݂�Scale
				//XMVECTOR scl = s1 - s0;
				//XMStoreFloat3(&g_Coin[i].scl, s0 + scl * time);

			//}

			// �e���v���C���[�̈ʒu�ɍ��킹��
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
// �`�揈��
//=============================================================================
void DrawCoin(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_COIN; i++)
	{
		if (g_Coin[i].use == false) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Coin[i].scl.x, g_Coin[i].scl.y, g_Coin[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Coin[i].rot.x, g_Coin[i].rot.y + XM_PI, g_Coin[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Coin[i].pos.x, g_Coin[i].pos.y, g_Coin[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Coin[i].mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_Coin[i].model);
	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
COIN *GetCoin(void)
{
	return &g_Coin[0];
}

//=============================================================================
// �e�̃p�����[�^���Z�b�g
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

