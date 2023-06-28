//=============================================================================
//
// �G�t�F�N�g���� [burst.cpp]
// Author : �~�Á@�C��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "burst.h"
#include "enemy.h"
#include "meshfield.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_BURST		"data/MODEL/burst.obj"			// �ǂݍ��ރ��f����



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************



//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static BURST		g_Burst[MAX_BURST];						// �v���C���[

static XMFLOAT3					g_posB;				// �G�t�F�N�g�����ʒu

static BOOL			g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitBurst(void)
{
	for (int effectIndex = 0; effectIndex < MAX_BURST; effectIndex++)
	{
		LoadModel(MODEL_BURST, &g_Burst[effectIndex].model);

		g_Burst[effectIndex].pos  = { 0.0f, 0.0f, 0.0f };
		g_Burst[effectIndex].rot  = { 0.0f, 0.0f, 0.0f };
		g_Burst[effectIndex].scl  = { 1.0f, 1.0f, 1.0f };
		g_Burst[effectIndex].move = { 0.0f, 0.0f, 0.0f };
		g_Burst[effectIndex].accel= { 0.0f, -0.5f, -1.0f };

		g_Burst[effectIndex].use = true;
	}

	return S_OK;
}


//=============================================================================
// �I������
//=============================================================================
void UninitBurst(void)
{
	for (int effectIndex = 0; effectIndex < MAX_BURST; effectIndex++)
	{
		// ���f���̉������
		if (g_Burst[effectIndex].ismodelLoaded)
		{
			UnloadModel(&g_Burst[effectIndex].model);
			g_Burst[effectIndex].ismodelLoaded = false;
		}
	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateBurst(void)
{
	//lifetime��16�t���[��
	ENEMY* pEnemy = GetEnemy();
	g_posB = pEnemy->pos;


	for (int effectIndex = 0; effectIndex < MAX_BURST; effectIndex++)
	{
		if (!g_Burst[effectIndex].use)
			continue;
		// �g�p��
		g_Burst[effectIndex].lifetimeFrame--;

		XMVECTOR pos   = XMLoadFloat3(&g_Burst[effectIndex].pos);
		XMVECTOR move  = XMLoadFloat3(&g_Burst[effectIndex].move);
		XMVECTOR accel = XMLoadFloat3(&g_Burst[effectIndex].accel);

		pos += move;
		move += accel;

		XMStoreFloat3(&g_Burst[effectIndex].pos, pos);
		XMStoreFloat3(&g_Burst[effectIndex].move, move);

		XMFLOAT3 hitPosition;
		XMFLOAT3 normal;
		RayHitField(g_Burst[effectIndex].pos, &hitPosition, &normal);

		if (g_Burst[effectIndex].pos.y < hitPosition.y)
		{
			g_Burst[effectIndex].pos.y = hitPosition.y;
			g_Burst[effectIndex].move.y *= -0.5f;
		}


		g_Burst[effectIndex].rot.z += XM_PI / 20;

		if (g_Burst[effectIndex].lifetimeFrame < 0)
		{
			g_Burst[effectIndex].use = false;
			continue;
		}


		//if (g_Burst[effectIndex].lifetimeFrame > 32)
		//	continue;

		//if (g_Burst[effectIndex].lifetimeFrame < 16)
		//{
		//	g_Burst[effectIndex].scl.x = (float)(g_Burst[effectIndex].lifetimeFrame) / 16 * 1.0f;
		//	g_Burst[effectIndex].scl.y = (float)(g_Burst[effectIndex].lifetimeFrame) / 16 * 1.0f;
		//	g_Burst[effectIndex].scl.z = (float)(g_Burst[effectIndex].lifetimeFrame) / 16 * 1.0f;
		//	continue;
		//}

		//g_Burst[effectIndex].scl.x = (float)(32 - g_Burst[effectIndex].lifetimeFrame) / 16 * 1.0f;
		//g_Burst[effectIndex].scl.y = (float)(32 - g_Burst[effectIndex].lifetimeFrame) / 16 * 1.0f;
		//g_Burst[effectIndex].scl.z = (float)(32 - g_Burst[effectIndex].lifetimeFrame) / 16 * 1.0f;

	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBurst(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);
	for (int effectIndex = 0; effectIndex < MAX_BURST; effectIndex++)
	{

		if (!g_Burst[effectIndex].use)
			continue;
		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Burst[effectIndex].scl.x, g_Burst[effectIndex].scl.y, g_Burst[effectIndex].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Burst[effectIndex].rot.x, g_Burst[effectIndex].rot.y + XM_PI, g_Burst[effectIndex].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Burst[effectIndex].pos.x, g_Burst[effectIndex].pos.y, g_Burst[effectIndex].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		// ����(�v���C���[)��mtxWorld��ۑ����Ă���B�p�[�c����������̂����R�ɂȂ�
		XMStoreFloat4x4(&g_Burst[effectIndex].mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_Burst[effectIndex].model);

	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);

}

//=============================================================================
// �G�t�F�N�g�̔�������
//=============================================================================
void SetBurst(XMFLOAT3 pos, int lifetimeFrame)
{
	int nidxBurst = -1;

	for (int effectIndex = 0; effectIndex < MAX_BURST; effectIndex++)
	{
		if (!g_Burst[effectIndex].use)
		{
			g_Burst[effectIndex].pos = pos;
			g_Burst[effectIndex].rot = { 0.0f,0.0f,0.0f };
			g_Burst[effectIndex].scl = { 1.0f,1.0f,1.0f };
			g_Burst[effectIndex].lifetimeFrame = lifetimeFrame;
			g_Burst[effectIndex].use = true;
			float dir = (rand()%17) / 16.0f * (2*XM_PI);
			float force = rand() % 5;
			g_Burst[effectIndex].move = { sinf(dir) * force,4.0f,cosf(dir) * force };
			


			nidxBurst = effectIndex;

			break;
		}

	}

}
