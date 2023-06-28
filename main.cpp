//=============================================================================
//
// ���C������ [main.cpp]
// Author : �~�Á@�C��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "player.h"
#include "enemy.h"
#include "coin.h"
#include "shadow.h"
#include "light.h"
#include "meshfield.h"
#include "meshwall.h"
#include "tree.h"
#include "collision.h"
#include "bullet.h"
#include "score.h"
#include "P_hp.h"
#include "sound.h"
#include "title.h"
#include "result.h"
#include "tutorial.h"
#include "particle.h"
#include "burst.h"
#include "effect.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define CLASS_NAME		"AppClass"			// �E�C���h�E�̃N���X��
#define WINDOW_NAME		""		// �E�C���h�E�̃L���v�V������

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

void CheckHit(void);


//*****************************************************************************
// �O���[�o���ϐ�:
//*****************************************************************************
long g_MouseX = 0;
long g_MouseY = 0;


#ifdef _DEBUG
int		g_CountFPS;							// FPS�J�E���^
char	g_DebugStr[2048] = WINDOW_NAME;		// �f�o�b�O�����\���p

#endif

int	g_Mode = MODE_TITLE;					// �N�����̉�ʂ�ݒ�

//=============================================================================
// ���C���֐�
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j
	UNREFERENCED_PARAMETER(lpCmdLine);		// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j

	// ���Ԍv���p
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		CLASS_NAME,
		NULL
	};
	HWND		hWnd;
	MSG			msg;
	
	// �E�B���h�E�N���X�̓o�^
	RegisterClassEx(&wcex);

	// �E�B���h�E�̍쐬
	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																		// �E�B���h�E�̍����W
		CW_USEDEFAULT,																		// �E�B���h�E�̏���W
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,									// �E�B���h�E����
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	// �E�B���h�E�c��
		NULL,
		NULL,
		hInstance,
		NULL);

	// ����������(�E�B���h�E���쐬���Ă���s��)
	if(FAILED(Init(hInstance, hWnd, TRUE)))
	{
		return -1;
	}

	// �t���[���J�E���g������
	timeBeginPeriod(1);	// ����\��ݒ�
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// �V�X�e���������~���b�P�ʂŎ擾
	dwCurrentTime = dwFrameCount = 0;

	// �E�C���h�E�̕\��(�����������̌�ɌĂ΂Ȃ��Ƒʖ�)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// ���b�Z�[�W���[�v
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()���Ă΂ꂽ�烋�[�v�I��
				break;
			}
			else
			{
				// ���b�Z�[�W�̖|��Ƒ��o
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }
		else
		{
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1�b���ƂɎ��s
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPS�𑪒肵��������ۑ�
				dwFrameCount = 0;							// �J�E���g���N���A
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60�b���ƂɎ��s
			{
				dwExecLastTime = dwCurrentTime;	// ��������������ۑ�

#ifdef _DEBUG	// �f�o�b�O�ł̎�����FPS��\������
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif

				Update();			// �X�V����
				Draw();				// �`�揈��

#ifdef _DEBUG	// �f�o�b�O�ł̎������\������
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;
				AddCount(1);
			}
		}
	}

	timeEndPeriod(1);				// ����\��߂�

	// �E�B���h�E�N���X�̓o�^������
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// �I������
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// �v���V�[�W��
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;

	case WM_MOUSEMOVE:
		g_MouseX = LOWORD(lParam);
		g_MouseY = HIWORD(lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//=============================================================================
// ����������
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	InitRenderer(hInstance, hWnd, bWindow);

	InitLight();

	InitCamera();

	// �T�E���h�̏���������
	InitSound(hWnd);

	// ���͏����̏�����
	InitInput(hInstance, hWnd);

	//// �^�C�g���̏�����
	//InitTitle();

	//// �`���[�g���A���̏�����
	//InitTutorial();

	InitParticle();

	// �t�B�[���h�̏�����
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 20, 200, 13.0f, 13.0f);

	// �ǂ̏�����
	/*InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);*/
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT + 100.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT - 100.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT + 120.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT - 120.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);

	/*InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);*/

	// ��(�����p�̔�����)
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);

	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);

	// �e�̏���������
	InitShadow();

	// �v���C���[�̏�����
	InitPlayer();

	// �G�l�~�[�̏�����
	InitEnemy();

	// �R�C���̏�����
	InitCoin();


	//// �؂𐶂₷
	InitTree();

	// �e�̏�����
	InitBullet();

	// �X�R�A�̏�����
	InitScore();

	// HP�̏�����
	InitP_Hp();

	// ���U���g�̏�����
	InitResult();

	// ���C�g��L����
	SetLightEnable(true);

	// �w�ʃ|���S�����J�����O
	SetCullingMode(CULL_MODE_BACK);

	// �ŏ��̃��[�h���Z�b�g
	SetMode(g_Mode);	// ������SetMode�̂܂܂ŁI


	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void Uninit(void)
{
	// �I���̃��[�h���Z�b�g
	SetMode(MODE_MAX);

	// �T�E���h�̏I������
	UninitSound();

	// �X�R�A�̏I������
	UninitScore();

	// HP�̏I������
	UninitP_Hp();

	// �p�[�e�B�N���̏I������
	UninitParticle();

	// �����̏I������
	UninitBurst();

	// �����̏I������
	UninitEffect();

	// �e�̏I������
	UninitBullet();

	// �؂̏I������
	UninitTree();

	// �G�l�~�[�̏I������
	UninitEnemy();

	// �R�C���̏I������
	UninitCoin();

	// �v���C���[�̏I������
	UninitPlayer();

	// �e�̏I������
	UninitShadow();

	// �ǂ̏I������
	UninitMeshWall();

	// �n�ʂ̏I������
	UninitMeshField();

	// �J�����̏I������
	UninitCamera();

	//���͂̏I������
	UninitInput();

	////�^�C�g���̏I������
	//UninitTitle();

	////�`���[�g���A���̏I������
	//UninitTutorial();

	//���U���g�̏I������
	UninitResult();

	// �����_���[�̏I������
	UninitRenderer();
}

//=============================================================================
// �X�V����
//=============================================================================
void Update(void)
{
	// ���͂̍X�V����
	UpdateInput();

	// �J�����X�V
	UpdateCamera();


	// ���[�h�ɂ���ď����𕪂���
	switch (g_Mode)
	{
	case MODE_TITLE:		// �^�C�g����ʂ̍X�V
		UpdateTitle();
		break;

	case MODE_TUTORIAL:		// �^�C�g����ʂ̍X�V
		UpdateTutorial();
		break;

	case MODE_GAME:			// �Q�[����ʂ̍X�V

			// �n�ʏ����̍X�V
		UpdateMeshField();

		// �Ǐ����̍X�V
		UpdateMeshWall();

		// �p�[�e�B�N���̍X�V
		UpdateParticle();

		// �����̍X�V
		UpdateBurst();

		// �����̍X�V
		UpdateEffect();

		// �v���C���[�̍X�V����
		UpdatePlayer();

		// �G�l�~�[�̍X�V����
		UpdateEnemy();

		// �R�C���̍X�V����
		UpdateCoin();

		// �؂̍X�V����
		UpdateTree();

		// �e�̍X�V����
		UpdateBullet();

		// �e�̍X�V����
		UpdateShadow();

		// �����蔻��
		CheckHit();

		// �X�R�A�̍X�V����
		UpdateScore();

		// HP�̍X�V����
		UpdateP_Hp();

		break;

	case MODE_RESULT:		// ���U���g��ʂ̍X�V
		UpdateResult();
		break;
	}


}

//=============================================================================
// �`�揈��
//=============================================================================
void Draw(void)
{
	// �o�b�N�o�b�t�@�N���A
	Clear();

	// �v���C���[���_
	XMFLOAT3 pos = GetPlayer()->pos;
	pos.y = 0.0f;			// �J����������h�����߂ɃN���A���Ă���
	SetCameraAT(pos);
	SetCamera();

	// ���[�h�ɂ���ď����𕪂���
	switch (g_Mode)
	{
	case MODE_TITLE:		// �^�C�g����ʂ̕`��
		// 2D�̕���`�悷�鏈��
		// Z��r�Ȃ�
		SetDepthEnable(false);

		// ���C�e�B���O�𖳌�
		SetLightEnable(false);

		DrawTitle();

		// ���C�e�B���O��L����
		SetLightEnable(true);

		// Z��r����
		SetDepthEnable(true);
	
		break;
	case MODE_TUTORIAL:		// �^�C�g����ʂ̕`��
		// 2D�̕���`�悷�鏈��
		// Z��r�Ȃ�
		SetDepthEnable(false);

		// ���C�e�B���O�𖳌�
		SetLightEnable(false);

		DrawTutorial();

		// ���C�e�B���O��L����
		SetLightEnable(true);

		// Z��r����
		SetDepthEnable(true);

		break;
	case MODE_GAME:			// �Q�[����ʂ̕`��
			// �n�ʂ̕`�揈��
		DrawMeshField();

		// �e�̕`�揈��
		DrawShadow();

		// �v���C���[�̕`�揈��
		DrawPlayer();

		// �G�l�~�[�̕`�揈��
		DrawEnemy();

		// �R�C���̕`�揈��
		DrawCoin();

		// �e�̕`�揈��
		DrawBullet();

		// �ǂ̕`�揈��
		DrawMeshWall();

		// �؂̕`�揈��
		DrawTree();

		// �����̕`�揈��
		DrawBurst();

		// �����̕`�揈��
		DrawEffect();
		// �p�[�e�B�N���̕`�揈��
		DrawParticle();

		// 2D�̕���`�悷�鏈��
		// Z��r�Ȃ�
		SetDepthEnable(false);

		// ���C�e�B���O�𖳌�
		SetLightEnable(false);

		// �X�R�A�̕`�揈��
		DrawScore();
		// HP�̕`�揈��
		DrawP_Hp();

		// ���C�e�B���O��L����
		SetLightEnable(true);

		// Z��r����
		SetDepthEnable(true);

		break;
	case MODE_RESULT:		// ���U���g��ʂ̕`��

		// 2D�̕���`�悷�鏈��
		// Z��r�Ȃ�
		SetDepthEnable(false);

		// ���C�e�B���O�𖳌�
		SetLightEnable(false);

		DrawResult();

		// ���C�e�B���O��L����
		SetLightEnable(true);

		// Z��r����
		SetDepthEnable(true);


		break;
	}

	



#ifdef _DEBUG
	// �f�o�b�O�\��
	DrawDebugProc();
#endif

	// �o�b�N�o�b�t�@�A�t�����g�o�b�t�@����ւ�
	Present();
}


long GetMousePosX(void)
{
	return g_MouseX;
}


long GetMousePosY(void)
{
	return g_MouseY;
}


#ifdef _DEBUG
char* GetDebugStr(void)
{
	return g_DebugStr;
}
#endif

//=============================================================================
// ���[�h�̐ݒ�
//=============================================================================
void SetMode(int mode)
{
	// ���[�h��ς���O�ɑS������������������Ⴄ
	StopSound();			// �܂��Ȃ��~�߂�

	// ���[�h��ς���O�ɑS������������������Ⴄ

	// �^�C�g����ʂ̏I������
	UninitTitle();

	// �`���[�g���A����ʂ̏I������
	UninitTutorial();

	// �p�[�e�B�N���̏I������
	UninitParticle();

	// �����̏I������
	UninitBurst();

	// �v���C���[�̏I������
	UninitPlayer();

	// �G�l�~�[�̏I������
	UninitEnemy();

	// �R�C���̏I������
	UninitCoin();

	// �o���b�g�̏I������
	UninitBullet();

	// �����̏I������
	UninitBurst();

	// �����̏I������
	UninitEffect();

	// �V���h�E�̏I������
	UninitShadow();

	// �X�R�A�̏I������
	UninitScore();

	// HP�̏I������
	UninitP_Hp();

	// ���U���g�̏I������
	UninitResult();


	g_Mode = mode;	// ���̃��[�h���Z�b�g���Ă���

	switch (g_Mode)
	{
	case MODE_TITLE:
		// �^�C�g����ʂ̏�����
		InitTitle();
		PlaySound(SOUND_LABEL_BGM_title);
		break;

	case MODE_TUTORIAL:
		// �`���[�g���A����ʂ̏�����
		InitTutorial();

		PlaySound(SOUND_LABEL_BGM_title);
		break;

	case MODE_GAME:
		// �Q�[����ʂ̏�����
		InitParticle();
		InitBurst();
		InitEffect();
		InitShadow();
		InitPlayer();
		InitEnemy();
		InitCoin();

		InitBullet();
		InitScore();
		InitP_Hp();

		PlaySound(SOUND_LABEL_BGM_stage);
		break;

	case MODE_RESULT:
		InitResult();
		PlaySound(SOUND_LABEL_BGM_result);
		break;

	case MODE_MAX:
		break;
	}
}

//=============================================================================
// ���[�h�̎擾
//=============================================================================
int GetMode(void)
{
	return g_Mode;
}



//=============================================================================
// �����蔻�菈��
//=============================================================================
void CheckHit(void)
{
	ENEMY  *enemy  = GetEnemy();	// �G�l�~�[�̃|�C���^�[��������
	PLAYER *player = GetPlayer();	// �v���C���[�̃|�C���^�[��������
	COIN* coin = GetCoin();	// �R�C���̃|�C���^�[��������
	BULLET *bullet = GetBullet();	// �e�̃|�C���^�[��������
	
	// �v���C���[�ƕǂ̓����蔻��

	if (player->pos.x < MAP_LEFT + 10)
	{
		player->pos.x = 1 + MAP_LEFT + 10;
	}

	if (player->pos.x > MAP_RIGHT-10)
	{
		player->pos.x = -1 + MAP_RIGHT - 10;
	}

	// �G�ƃv���C���[�L����
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		//�G�̗L���t���O���`�F�b�N����
		if (enemy[i].use == false)
			continue;

		//BC�̓����蔻��
		if (CollisionBC(player->pos, enemy[i].pos, player->size, enemy[i].size))
		{
			// �G�ɓ���������HP�|1
			player->hp -= 1;
			AddP_Hp(-1);
			if (player->hp == 0)
			{
				////HP��0�Ȃ烊�U���g
				//SetMode(MODE_RESULT);
			}

			// �G�L�����N�^�[�͓|�����
			enemy[i].use = false;
			

			for (int j = 0; j < 10; j++)
			{
				SetBurst(enemy[i].pos,30);
			}
			/*SetTree(enemy[i].pos,10.0f,10.0f,30);*/

			SetBurst(enemy[i].pos, 36);

		}
	}

	// �R�C���ƃv���C���[�L����
	for (int i = 0; i < MAX_COIN; i++)
	{
		//�G�̗L���t���O���`�F�b�N����
		if (coin[i].use == false)
			continue;

		//BC�̓����蔻��
		if (CollisionBC(player->pos, coin[i].pos, player->size, coin[i].size))
		{
			// �G�L�����N�^�[�͓|�����
			coin[i].use = false;
			

			XMFLOAT3 pos = coin[i].pos;
			pos.x += (rand()% 20) - 10;
			pos.y += (rand() % 10);

			for (int j = 0; j < 10; j++)
			{
				SetEffect(pos, 36);
			}
			/*SetTree(coin[i].pos, 10.0f, 10.0f, 30);*/

			SetEffect(coin[i].pos, 36);

			// �X�R�A�𑫂�
			AddScore(100);

		}
	}


	// �v���C���[�̒e�ƓG
	for (int i = 0; i < MAX_BULLET; i++)
	{
		//�e�̗L���t���O���`�F�b�N����
		if (bullet[i].use == false)
			continue;

		// �G�Ɠ������Ă邩���ׂ�
		for (int j = 0; j < MAX_ENEMY; j++)
		{
			//�G�̗L���t���O���`�F�b�N����
			if (enemy[j].use == false)
				continue;

			//BC�̓����蔻��
			if (CollisionBC(bullet[i].pos, enemy[j].pos, bullet[i].fWidth, enemy[j].size))
			{

				// �G�L�����N�^�[�͓|�����
				enemy[j].use = false;
				ReleaseShadow(enemy[j].shadowIdx);

				// �X�R�A�𑫂�
				AddScore(10);
			}
		}

	}


	//// �G�l�~�[���S�����S�������ԑJ��
	//int enemy_count = 0;
	//for (int i = 0; i < MAX_ENEMY; i++)
	//{
	//	if (enemy[i].use == false)
	//	{
	//		continue;
	//	}
	//	enemy_count++;
	//}

	//// �G�l�~�[���O�C�H
	//if (enemy_count == 0)
	//{
	//	 SetMode(MODE_RESULT);

	//}

}

