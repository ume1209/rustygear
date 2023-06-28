//=============================================================================
//
// ���f������ [player.cpp]
// Author : �~�Á@�C��
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "player.h"
#include "shadow.h"
#include "light.h"
#include "bullet.h"
#include "sound.h"
#include "sprite.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_PLAYER			"data/MODEL/AC_Body.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_PARTS_0	"data/MODEL/AC_L_Arm.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_PARTS_1	"data/MODEL/AC_R_Arm.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_PARTS_2	"data/MODEL/AC_L_UL.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_PARTS_3	"data/MODEL/AC_R_UL.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_PARTS_4	"data/MODEL/AC_L_LL.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_PARTS_5	"data/MODEL/AC_R_LL.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_PARTS_6	"data/MODEL/Rifle.obj"			// �ǂݍ��ރ��f����


//#define	MODEL_PLAYER_PARTS_6	"data/MODEL/AC_R_Head.obj"			// �ǂݍ��ރ��f����


#define	VALUE_MOVE			(2.0f)							// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// ��]��

#define PLAYER_SHADOW_SIZE	(1.0f)							// �e�̑傫��
#define PLAYER_OFFSET_Y		(19.0f)							// �v���C���[�̑��������킹��

#define PLAYER_PARTS_MAX	(7)								// �v���C���[�̃p�[�c�̐�

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
enum STATE {
	taiki = 0,
	move_R,
	move_L,
	shot
};

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static PLAYER		g_Player;						// �v���C���[

static PLAYER		g_Parts[PLAYER_PARTS_MAX];		// �v���C���[�̃p�[�c�p

static float		floatidx;

static STATE		g_playerState = taiki;

// �v���C���[�̊K�w�A�j���[�V�����f�[�^
// �v���C���[�̓������E�ɓ������Ă���A�j���f�[�^
static PARTDATA stateListArm_R[] = {
	{XMFLOAT3(8.0f, 6.0f, 0.0f), XMFLOAT3(-30.0f * XM_PI / 180, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f)},
	{XMFLOAT3(8.0f, 6.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f)},
	{XMFLOAT3(8.0f, 6.0f, 0.0f), XMFLOAT3(-80.0f * XM_PI / 180, -40.0f * XM_PI / 180, 0.0f),XMFLOAT3(1.0f, 1.0f, 1.0f)}
};

static PARTDATA stateListArm_L[] = {
	{XMFLOAT3(-8.0f, 6.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -10.0f * XM_PI / 180), XMFLOAT3(1.0f, 1.0f, 1.0f)},
	{XMFLOAT3(-8.0f, 6.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -10.0f * XM_PI / 180), XMFLOAT3(1.0f, 1.0f, 1.0f)},
	{XMFLOAT3(-8.0f, 6.0f, 0.0f), XMFLOAT3(-80.0f * XM_PI / 180, 40.0f * XM_PI / 180, 0.0f),XMFLOAT3(1.0f, 1.0f, 1.0f)}
};

static PARTDATA stateListULeg_R[] = {
	{XMFLOAT3(3.5f, -4.0f, 0.0f), XMFLOAT3(0.0f, 0.0f,15.0f * XM_PI / 180),         XMFLOAT3(1.0f, 1.0f, 1.0f)},
	{XMFLOAT3(3.5f, -4.0f, 0.0f), XMFLOAT3(-60.0f * XM_PI / 180, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f)},
	{XMFLOAT3(3.5f, -4.0f, 0.0f), XMFLOAT3(0.0f, 0.0f,15.0f * XM_PI / 180),         XMFLOAT3(1.0f, 1.0f, 1.0f)},
};

static PARTDATA stateListULeg_L[] = {
	{XMFLOAT3(-3.5f, -4.0f, 0.0f), XMFLOAT3(0.0f, 0.0f,-15.0f * XM_PI / 180),         XMFLOAT3(1.0f, 1.0f, 1.0f)},
	{XMFLOAT3(-3.5f, -4.0f, 0.0f), XMFLOAT3(-60.0f * XM_PI / 180, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f)},
	{XMFLOAT3(-3.5f, -4.0f, 0.0f), XMFLOAT3(0.0f, 0.0f,-15.0f * XM_PI / 180),         XMFLOAT3(1.0f, 1.0f, 1.0f)},
};

static PARTDATA stateListLLeg_R[] = {
	{ XMFLOAT3(2.0f, -9.0f, 0.0f), XMFLOAT3(0.0f, 0.0f,  0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f)},
	{ XMFLOAT3(2.0f, -9.0f, 0.0f), XMFLOAT3(100.0f * XM_PI / 180, 0.0f, 0.0f),XMFLOAT3(1.0f, 1.0f, 1.0f)},
	{ XMFLOAT3(2.0f, -9.0f, 0.0f), XMFLOAT3(0.0f, 0.0f,  0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f)},
};

static PARTDATA stateListLLeg_L[] = {
	{ XMFLOAT3(-2.0f, -9.0f, 0.0f), XMFLOAT3(0.0f, 0.0f,  0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f)},
	{ XMFLOAT3(-2.0f, -9.0f, 0.0f), XMFLOAT3(100.0f * XM_PI / 180, 0.0f, 0.0f),XMFLOAT3(1.0f, 1.0f, 1.0f)},
	{ XMFLOAT3(-2.0f, -9.0f, 0.0f), XMFLOAT3(0.0f, 0.0f,  0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f)},
};

static PARTDATA stateListRIfle[] = {
	{ XMFLOAT3(2.0f, -13.0f, 3.0f), XMFLOAT3(0.0f, 90.0f * XM_PI / 180,  60.0f * XM_PI / 180),         XMFLOAT3(1.8f, 1.8f, 1.8f)},
	{ XMFLOAT3(2.0f, -13.0f, 3.0f), XMFLOAT3(0.0f, 90.0f * XM_PI / 180,  60.0f * XM_PI / 180),         XMFLOAT3(1.8f, 1.8f, 1.8f) },
	{ XMFLOAT3(2.0f, -13.0f, 3.0f), XMFLOAT3(-40.0f * XM_PI / 180, 85.0f * XM_PI / 180,  90.0f * XM_PI / 180),         XMFLOAT3(1.8f, 1.8f, 1.8f) },
};

static PARTDATA* g_Player_Part[] = {
	stateListArm_R,
	stateListArm_L,
	stateListULeg_R,
	stateListULeg_L,
	stateListLLeg_R,
	stateListLLeg_L,
	stateListRIfle
};



//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{


	LoadModel(MODEL_PLAYER, &g_Player.model);
	g_Player.load = true;

	g_Player.pos = { 0.0f, PLAYER_OFFSET_Y, -10.0f };
	g_Player.rot = { 0.0f, 0.0f, 0.0f };
	g_Player.scl = { 0.6f, 0.6f, 0.6f };

	g_Player.spd = 0.0f;			// �ړ��X�s�[�h�N���A
	g_Player.size = PLAYER_SIZE;	// �����蔻��̑傫��
	
	g_Player.hp_max = 3;
	g_Player.hp = 3;
	g_Player.use = true;

	// �����Ńv���C���[�p�̉e���쐬���Ă���
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE/2, PLAYER_SHADOW_SIZE/2);
	//          ��
	//        ���̃����o�[�ϐ������������e��Index�ԍ�



	// �K�w�A�j���[�V�����p�̏���������
	g_Player.parent = NULL;			// �{�́i�e�j�Ȃ̂�NULL������

	// �p�[�c�̏�����
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		g_Parts[i].use = false;

		// �ʒu�E��]�E�X�P�[���̏����ݒ�
		g_Parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);	// �e�̌��_����̃I�t�Z�b�g���W
		g_Parts[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);	// �����̉�]���
		g_Parts[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);	// �����̊g��k��

		// �e�q�֌W
		g_Parts[i].parent = &g_Player;		// �� �����ɐe�̃A�h���X������
		g_Parts[0].parent = &g_Player;		// �r��������e�͖{�́i�v���C���[�j
		g_Parts[1].parent = &g_Player;
		g_Parts[2].parent = &g_Player;
		g_Parts[3].parent = &g_Player;
		g_Parts[4].parent = &g_Parts[2];
		g_Parts[5].parent = &g_Parts[3];
		g_Parts[6].parent = &g_Player;

		//g_Parts[6].parent = &g_Player;
	//	g_Parts[��].parent= &g_Paerts[�r];	// �w���r�̎q���������ꍇ�̗�


		// �K�w�A�j���[�V�����p�̃����o�[�ϐ��̏�����
		g_Parts[i].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
		g_Parts[i].tblNo = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
		g_Parts[i].tblMax = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g

		// �p�[�c�̓ǂݍ��݂͂܂����Ă��Ȃ�
		g_Parts[i].load = 0;
	}

	// �E��r�p�[�c�̐ݒ�
	LoadModel(MODEL_PLAYER_PARTS_0, &g_Parts[0].model);
	g_Parts[0].use = true;
	g_Parts[0].load = true;
	g_Parts[0].parent = &g_Player;

	// ����r�p�[�c�̐ݒ�
	LoadModel(MODEL_PLAYER_PARTS_1, &g_Parts[1].model);
	g_Parts[1].use = true;
	g_Parts[1].load = true;
	g_Parts[1].parent = &g_Player;

	// �E��r�p�[�c�̐ݒ�
	LoadModel(MODEL_PLAYER_PARTS_2, &g_Parts[2].model);
	g_Parts[2].use = true;
	g_Parts[2].load = true;
	g_Parts[2].parent = &g_Player;

	// ����r�p�[�c�̐ݒ�
	LoadModel(MODEL_PLAYER_PARTS_3, &g_Parts[3].model);
	g_Parts[3].use = true;
	g_Parts[3].load = true;
	g_Parts[3].parent = &g_Player;

	// �E��r�p�[�c�̐ݒ�
	LoadModel(MODEL_PLAYER_PARTS_4, &g_Parts[4].model);
	g_Parts[4].use = true;
	g_Parts[4].load = true;
	g_Parts[4].parent = &g_Parts[2];

	// ����r�p�[�c�̐ݒ�
	LoadModel(MODEL_PLAYER_PARTS_5, &g_Parts[5].model);
	g_Parts[5].use = true;
	g_Parts[5].load = true;
	g_Parts[5].parent = &g_Parts[3];

	// ���C�t���p�[�c�̐ݒ�
	LoadModel(MODEL_PLAYER_PARTS_6, &g_Parts[6].model);
	g_Parts[6].use = true;
	g_Parts[6].load = true;
	g_Parts[6].parent = &g_Parts[0];

	//// ���p�[�c�̐ݒ�
	//LoadModel(MODEL_PLAYER_PARTS_6, &g_Parts[6].model);
	//g_Parts[6].tblNo = 6;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
	//g_Parts[6].tblMax = sizeof(move_tbl_Head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	//g_Parts[6].use = true;
	//g_Parts[6].load = true;
	//g_Parts[6].parent = &g_Player;


	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
{
	// ���f���̉������
	if (g_Player.load)
	{
		UnloadModel(&g_Player.model);
		g_Player.load = false;
	}

	// �p�[�c�̉������
	{
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePlayer(void)
{
	CAMERA *cam = GetCamera();
	bool isMove = false;
	int Count = 0;
	g_playerState = taiki;
	g_Player.rot.x = 15 * XM_PI / 180;
	g_Player.rot.z = 0;
	g_Player.rot.y =180 * XM_PI / 180;
	g_Player.pos.y += sinf(floatidx) * 0.1f;
	floatidx += XM_PI / 50;

	if(g_Player.hp == 1)
		SetModelDiffuse(&g_Player.model, 1, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	// �ړ��������Ⴄ
	if (GetKeyboardPress(DIK_LEFT))
	{	// ���ֈړ�
		g_Player.spd = 2;
		g_Player.rot.z = 15 * XM_PI / 180;
		g_Player.pos.x -= g_Player.spd;
		g_playerState = move_L;
	}
	

	if (GetKeyboardPress(DIK_RIGHT))
	{	// �E�ֈړ�
		g_Player.spd = 2;
		g_Player.rot.z = -15 * XM_PI / 180;
		g_Player.pos.x += g_Player.spd;
		g_playerState = move_R;

	}
	

	//if (GetKeyboardPress(DIK_UP))
	//{	// ��ֈړ�
	//	g_Player.spd = VALUE_MOVE;
	//	g_Player.dir = XM_PI;
	//}
	//if (GetKeyboardPress(DIK_DOWN))
	//{	// ���ֈړ�
	//	g_Player.spd = VALUE_MOVE;
	//	g_Player.dir = 0.0f;
	//}


#ifdef _DEBUG
	/*if (GetKeyboardPress(DIK_R))
	{
		g_Player.pos.z = g_Player.pos.x = 0.0f;
		g_Player.rot.y = g_Player.dir = 0.0f;
		g_Player.spd = 0.0f;
	}*/
#endif


	//	// Key���͂���������ړ���������
	if (g_Player.spd > 1.0f)
	{
		isMove = true;

		//g_Player.rot.y = g_Player.dir + cam->rot.y;

		//// ���͂̂����������փv���C���[���������Ĉړ�������
		//g_Player.pos.x -= sinf(g_Player.rot.y) * g_Player.spd;
		//g_Player.pos.z -= cosf(g_Player.rot.y) * g_Player.spd;
	}

	// �e���v���C���[�̈ʒu�ɍ��킹��
	XMFLOAT3 pos = g_Player.pos;
	pos.y -=
		(PLAYER_OFFSET_Y - 0.1f);
	SetPositionShadow(g_Player.shadowIdx, pos);

	// �e���ˏ���
	if (GetKeyboardPress(DIK_SPACE))
	{
		g_playerState = shot;
		g_Player.rot.x = -5 * XM_PI / 180;

		SetBullet(g_Player.pos, 5.0f,5.0f,1000);
		PlaySound(SOUND_LABEL_SE_shot000);
	}

	g_Player.spd *= 0.5f;


	// �K�w�A�j���[�V����
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// �g���Ă���Ȃ珈������
		if (g_Parts[i].use == true)
		{	
			PARTDATA* tbl = g_Player_Part[i];	// �s���e�[�u���̃A�h���X���擾
			
			switch (g_playerState)
			{
			case taiki:
				g_Parts[i].pos = tbl[taiki].pos;
				g_Parts[i].rot = tbl[taiki].rot;
				g_Parts[i].scl = tbl[taiki].scl;
				break;

			case move_R:
				if (i % 2 == 0)
				{
					g_Parts[i].pos = tbl[1].pos;
					g_Parts[i].rot = tbl[1].rot;
					g_Parts[i].scl = tbl[1].scl;
				}
				break;

			case move_L:
				if (i % 2 == 1)
				{
					g_Parts[i].pos = tbl[1].pos;
					g_Parts[i].rot = tbl[1].rot;
					g_Parts[i].scl = tbl[1].scl;
				}
				break;

			case shot:
				g_Parts[i].pos = tbl[shot - 1].pos;
				g_Parts[i].rot = tbl[shot - 1].rot;
				g_Parts[i].scl = tbl[shot - 1].scl;
				break;

			//default:
			//	g_playerState = taiki;
			}
			



			// ���`��Ԃ̏���
		//	int nowNo = (int)g_Parts[i].time;			// �������ł���e�[�u���ԍ������o���Ă���
		//	int maxNo = g_Parts[i].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
		//	int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
		//	INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Parts[i].tblNo];	// �s���e�[�u���̃A�h���X���擾

		//	XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
		//	XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
		//	XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

		//	XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
		//	XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
		//	XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

		//	float nowTime = g_Parts[i].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

		//	Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
		//	Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
		//	Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

		//	// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
		//	XMStoreFloat3(&g_Parts[i].pos, nowPos + Pos);

		//	// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
		//	XMStoreFloat3(&g_Parts[i].rot, nowRot + Rot);

		//	// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
		//	XMStoreFloat3(&g_Parts[i].scl, nowScl + Scl);

		//	// frame���g�Ď��Ԍo�ߏ���������
		//	if (isMove == true)
		//	{
		//		g_Parts[i].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
		//	}
		//	//else
		//	//{
		//	//	g_Parts[i].time = 0.0f;	// ���Ԃ�i�߂Ă���
		//	//}

		//	if ((int)g_Parts[i].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
		//	{
		//		g_Parts[i].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
		//	}

		}

	}



	{	// �|�C���g���C�g�̃e�X�g
		LIGHT *light = GetLightData(1);
		XMFLOAT3 pos = g_Player.pos;
		pos.y += 20.0f;

		light->Position = pos;
		light->Diffuse = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
		light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Type = LIGHT_TYPE_POINT;
		light->Enable = true;
		SetLightData(1, light);
	}
	Count++;

#ifdef _DEBUG	// �f�o�b�O����\������
	/*PrintDebugProc("Player:�� �� �� ���@Space\n");
	PrintDebugProc("Player:X:%f Y:%f Z:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);*/
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{
	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y + XM_PI, g_Player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);
	

	// ���f���`��
	DrawModel(&g_Player.model);



	// �p�[�c�̊K�w�A�j���[�V����
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// �p�[�c���g��SRT���s��
		// �e���q���`�F�b�N����
		// �q�̏ꍇ�A�e�Ǝq��mtxWorld���|����
		// �ς��g���Ďq��`�悷��

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Parts[i].scl.x, g_Parts[i].scl.y, g_Parts[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i].rot.x, g_Parts[i].rot.y, g_Parts[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Parts[i].pos.x, g_Parts[i].pos.y, g_Parts[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		if (g_Parts[i].parent != NULL)	// �q����������e�ƌ�������
		{
			// �e��mtxWorld�͂ǂ��ɂ��邩
			//g_Parts[i].parent->mtxWorld = g_Player.mtxWorld;
			//g_Parts[i].parent = &g_Player;
			//g_Player.mtxWorldq;

			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i].parent->mtxWorld));
			// ��
			// g_Player.mtxWorld���w���Ă���
		}

		XMStoreFloat4x4(&g_Parts[i].mtxWorld, mtxWorld);

		// �g���Ă���Ȃ珈������B�����܂ŏ������Ă��闝�R�͑��̃p�[�c�����̃p�[�c���Q�Ƃ��Ă���\�������邩��B
		if (g_Parts[i].use == false) continue;

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);


		// ���f���`��
		DrawModel(&g_Parts[i].model);

	}


	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// �v���C���[�����擾
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player;
}

