//=============================================================================
//
// �G�t�F�N�g���� [burst.h]
// Author : �~�Á@�C��
//
//=============================================================================
#pragma once

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_BURST        (50)

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct BURST
{
    XMFLOAT3            pos;            // �|���S���̈ʒu
    XMFLOAT3            rot;            // �|���S���̌���(��])
    XMFLOAT3            scl;            // �|���S���̑傫��(�X�P�[��)
    XMFLOAT3            move;           // �v���C���[�̈ړ�����x�N�g��
    XMFLOAT3            accel;          // �����x
    MATERIAL            material;       // �}�e���A��

    XMFLOAT4X4          mtxWorld;       // ���[���h�}�g���b�N�X
    int                 lifetimeFrame;  // ����
    

    bool                ismodelLoaded;

    DX11_MODEL          model;          // ���f�����

    bool                use;

};


//**
// �v���g�^�C�v�錾
//**
HRESULT InitBurst(void);
void UninitBurst(void);
void UpdateBurst(void);
void DrawBurst(void);

void SetBurst(XMFLOAT3 pos, int lifetimeFrame);