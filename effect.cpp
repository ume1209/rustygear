//=============================================================================
//
// �G�t�F�N�g���� [effect.cpp]
// Author : �~�Á@�C��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "effect.h"
#include "enemy.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_EFFECT		"data/MODEL/kirakira.obj"			// �ǂݍ��ރ��f����



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************



//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static EFFECT		g_Effect[MAX_EFFECT];						// �v���C���[

static XMFLOAT3					g_posB;				// �G�t�F�N�g�����ʒu

static BOOL			g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitEffect(void)
{
	for (int effectIndex = 0; effectIndex < MAX_EFFECT; effectIndex++)
	{
		LoadModel(MODEL_EFFECT, &g_Effect[effectIndex].model);

		g_Effect[effectIndex].pos = { 0.0f, 0.0f, 0.0f };
		g_Effect[effectIndex].rot = { 0.0f, 0.0f, 0.0f };
		g_Effect[effectIndex].scl = { 1.0f, 1.0f, 1.0f };
		g_Effect[effectIndex].move = { 0.0f, 1.0f, 0.0f };


		g_Effect[effectIndex].use = true;
	}

	return S_OK;
}


//=============================================================================
// �I������
//=============================================================================
void UninitEffect(void)
{
	for (int effectIndex = 0; effectIndex < MAX_EFFECT; effectIndex++)
	{
		// ���f���̉������
		if (g_Effect[effectIndex].ismodelLoaded)
		{
			UnloadModel(&g_Effect[effectIndex].model);
			g_Effect[effectIndex].ismodelLoaded = false;
		}
	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEffect(void)
{
	//lifetime��16�t���[��
	ENEMY* pEnemy = GetEnemy();
	g_posB = pEnemy->pos;


	for (int effectIndex = 0; effectIndex < MAX_EFFECT; effectIndex++)
	{
		if (!g_Effect[effectIndex].use)
			continue;
		// �g�p��
		g_Effect[effectIndex].lifetimeFrame--;

		g_Effect[effectIndex].pos.x += g_Effect[effectIndex].move.x;
		g_Effect[effectIndex].pos.z += g_Effect[effectIndex].move.z;
		g_Effect[effectIndex].pos.y += g_Effect[effectIndex].move.y;

		g_Effect[effectIndex].rot.y += XM_PI / 5;

		if (g_Effect[effectIndex].lifetimeFrame < 0)
		{
			g_Effect[effectIndex].use = false;
			continue;
		}

		if (g_Effect[effectIndex].lifetimeFrame > 32)
			continue;

		if (g_Effect[effectIndex].lifetimeFrame < 16)
		{
			g_Effect[effectIndex].scl.x = (float)(g_Effect[effectIndex].lifetimeFrame) / 16 * 0.5f;
			g_Effect[effectIndex].scl.y = (float)(g_Effect[effectIndex].lifetimeFrame) / 16 * 0.5f;
			g_Effect[effectIndex].scl.z = (float)(g_Effect[effectIndex].lifetimeFrame) / 16 * 0.5f;
			continue;
		}

		g_Effect[effectIndex].scl.x = (float)(32 - g_Effect[effectIndex].lifetimeFrame) / 16 * 0.5f;
		g_Effect[effectIndex].scl.y = (float)(32 - g_Effect[effectIndex].lifetimeFrame) / 16 * 0.5f;
		g_Effect[effectIndex].scl.z = (float)(32 - g_Effect[effectIndex].lifetimeFrame) / 16 * 0.5f;

	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEffect(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);
	for (int effectIndex = 0; effectIndex < MAX_EFFECT; effectIndex++)
	{

		if (!g_Effect[effectIndex].use)
			continue;
		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Effect[effectIndex].scl.x, g_Effect[effectIndex].scl.y, g_Effect[effectIndex].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Effect[effectIndex].rot.x, g_Effect[effectIndex].rot.y + XM_PI, g_Effect[effectIndex].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Effect[effectIndex].pos.x, g_Effect[effectIndex].pos.y, g_Effect[effectIndex].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		// ����(�v���C���[)��mtxWorld��ۑ����Ă���B�p�[�c����������̂����R�ɂȂ�
		XMStoreFloat4x4(&g_Effect[effectIndex].mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_Effect[effectIndex].model);

	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);

}

//=============================================================================
// �G�t�F�N�g�̔�������
//=============================================================================
void SetEffect(XMFLOAT3 pos, int lifetimeFrame)
{
	int nidxEffect = -1;

	for (int effectIndex = 0; effectIndex < MAX_EFFECT; effectIndex++)
	{
		if (!g_Effect[effectIndex].use)
		{
			g_Effect[effectIndex].pos = pos;
			g_Effect[effectIndex].rot = { 0.0f,0.0f,0.0f };
			g_Effect[effectIndex].scl = { 0.1f,0.1f,0.1f };
			g_Effect[effectIndex].lifetimeFrame = lifetimeFrame;
			g_Effect[effectIndex].use = true;

			nidxEffect = effectIndex;

			break;
		}

	}

}

