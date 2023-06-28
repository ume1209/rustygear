//=============================================================================
//
// �G�l�~�[���f������ [enemy.cpp]
// Author : �~�Á@�C��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "enemy.h"
#include "shadow.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_ENEMY			"data/MODEL/toge.obj"		// �ǂݍ��ރ��f����

#define	VALUE_MOVE			(5.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define ENEMY_SHADOW_SIZE	(0.4f)						// �e�̑傫��
#define ENEMY_OFFSET_Y		(10.0f)						// �G�l�~�[�̑��������킹��


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];				// �G�l�~�[


int g_Enemy_load = 0;


//static INTERPOLATION_DATA move_tbl[] = {	// pos, rot, scl, frame
//	{ XMFLOAT3(   0.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*2 },
//	{ XMFLOAT3(-200.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*1 },
//	{ XMFLOAT3(-200.0f, ENEMY_OFFSET_Y, 200.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*0.5f },
//	{ XMFLOAT3(   0.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*2 },
//
//};

static int		g_Count;					// �J�E���g

//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		LoadModel(MODEL_ENEMY, &g_Enemy[i].model);
		g_Enemy[i].load = true;

		g_Enemy[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_Enemy[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Enemy[i].size = ENEMY_SIZE;	// �����蔻��̑傫��

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Enemy[0].model, &g_Enemy[0].diffuse[0]);

		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

		g_Enemy[i].move_time = 0.0f;	// ���`��ԗp�̃^�C�}�[���N���A
		g_Enemy[i].tbl_adr = NULL;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Enemy[i].tbl_size = 0;		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		g_Enemy[i].use = false;			// true:�����Ă�

	}


	//// 0�Ԃ������`��Ԃœ������Ă݂�
	//g_Enemy[0].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	//g_Enemy[0].tbl_adr = move_tbl;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	//g_Enemy[0].tbl_size = sizeof(move_tbl) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemy(void)
{

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model);
			g_Enemy[i].load = false;
		}

	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEnemy(void)
{
	// �G�l�~�[�𓮂����ꍇ�́A�e�����킹�ē���������Y��Ȃ��悤�ɂˁI
	for (int i = 0; i < MAX_ENEMY; i++)
	{

		if (g_Count < 600)
		{

			int E_flug = rand() % 10000;

			if (E_flug == 1)
			{
				SetEnemy(XMFLOAT3(-20.0f, ENEMY_OFFSET_Y, 400.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
			}
			if (E_flug == 2)
			{
				SetEnemy(XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 400.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
			}

			if (E_flug == 3)
			{
				SetEnemy(XMFLOAT3(20.0f, ENEMY_OFFSET_Y, 400.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
			}

		}

		if (g_Count > 600 && g_Count < 1800)
		{

			int E_flug = rand() % 10000;

			if (E_flug > 0 && E_flug < 3)
			{
				SetEnemy(XMFLOAT3(-20.0f, ENEMY_OFFSET_Y, 400.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
			}
			if (E_flug > 3 && E_flug < 6)
			{
				SetEnemy(XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 400.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
			}

			if (E_flug > 6 && E_flug < 9)
			{
				SetEnemy(XMFLOAT3(20.0f, ENEMY_OFFSET_Y, 400.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
			}

		}

		if (g_Count > 1800)
		{

			int E_flug = rand() % 10000;

			if (E_flug > 0 && E_flug < 4)
			{
				SetEnemy(XMFLOAT3(-20.0f, ENEMY_OFFSET_Y, 400.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
			}
			if (E_flug > 4 && E_flug <8)
			{
				SetEnemy(XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 400.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
			}

			if (E_flug > 8 && E_flug < 12)
			{
				SetEnemy(XMFLOAT3(20.0f, ENEMY_OFFSET_Y, 400.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
			}

		}

	

		if (g_Enemy[i].use == true)			// ���̃G�l�~�[���g���Ă���H
		{	
			/*int gap = g_Enemy[i].pos.z - g_Enemy[i].pos.z;

			if (fabsf(gap) < 1)
			{
				g_Enemy[i].use = false;
			}*/

			g_Enemy[i].pos.z -= 5.0f;

			// Yes
			//if (g_Enemy[i].tbl_adr != NULL)	// ���`��Ԃ����s����H
			//{								// ���`��Ԃ̏���
				//// �ړ�����
				//int		index = (int)g_Enemy[i].move_time;
				//float	time = g_Enemy[i].move_time - index;
				//int		size = g_Enemy[i].tbl_size;

				//float dt = 1.0f / g_Enemy[i].tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
				//g_Enemy[i].move_time += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

				//if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
				//{
				//	g_Enemy[i].move_time = 0.0f;
				//	index = 0;
				//}

				//// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
				//XMVECTOR p1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].pos);	// ���̏ꏊ
				//XMVECTOR p0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
				//XMVECTOR vec = p1 - p0;
				//XMStoreFloat3(&g_Enemy[i].pos, p0 + vec * time);

				//// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
				//XMVECTOR r1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].rot);	// ���̊p�x
				//XMVECTOR r0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].rot);	// ���݂̊p�x
				//XMVECTOR rot = r1 - r0;
				//XMStoreFloat3(&g_Enemy[i].rot, r0 + rot * time);

				//// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
				//XMVECTOR s1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].scl);	// ����Scale
				//XMVECTOR s0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].scl);	// ���݂�Scale
				//XMVECTOR scl = s1 - s0;
				//XMStoreFloat3(&g_Enemy[i].scl, s0 + scl * time);

			//}

			// �e���v���C���[�̈ʒu�ɍ��킹��
			XMFLOAT3 pos = g_Enemy[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(g_Enemy[i].shadowIdx, pos);

			if (g_Enemy[i].pos.z < -80)
			{
				g_Enemy[i].use = false;

			}

		}

		if (g_Enemy[i].use == false)
		{
			ReleaseShadow(g_Enemy[i].shadowIdx);
		}
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == false) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_Enemy[i].model);
	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
ENEMY *GetEnemy(void)
{
	return &g_Enemy[0];
}

//=============================================================================
// �G�l�~�[�̃p�����[�^���Z�b�g
//=============================================================================
int SetEnemy(XMFLOAT3 pos, XMFLOAT3 rot)
{
	int nIdxEnemy = -1;

	for (int nCntEnemy = 0; nCntEnemy < MAX_ENEMY; nCntEnemy++)
	{
		if (!g_Enemy[nCntEnemy].use)
		{
			g_Enemy[nCntEnemy].pos = pos;
			g_Enemy[nCntEnemy].rot = rot;
			g_Enemy[nCntEnemy].scl = { 0.8f, 0.8f, 0.8f };
			g_Enemy[nCntEnemy].use = true;

			nIdxEnemy = nCntEnemy;

			//	PlaySound(SOUND_LABEL_SE_shot000);

			break;
		}
	}

	return nIdxEnemy;
}


//=============================================================================
// �J�E���g�����Z����
// ����:add :�ǉ�����_���B�}�C�i�X���\
//=============================================================================
void AddCount(int add)
{
	g_Count += add;
}
