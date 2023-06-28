#pragma once
//=============================================================================
//
// �G�t�F�N�g���� [effect.h]
// Author : �~�ÏC��
//
//=============================================================================
#pragma once

//**
// �}�N����`
//**
#define MAX_EFFECT        (50)

//**
// �\���̒�`
//**
struct EFFECT
{
    XMFLOAT3            pos;                    // �|���S���̈ʒu
    XMFLOAT3            rot;                    // �|���S���̌���(��])
    XMFLOAT3            scl;                    // �|���S���̑傫��(�X�P�[��)
    XMFLOAT3            move;                    // �v���C���[�̈ړ�����x�N�g��
    MATERIAL            material;                // �}�e���A��


    XMFLOAT4X4            mtxWorld;                // ���[���h�}�g���b�N�X
    int                    lifetimeFrame;            // ����

    bool                ismodelLoaded;

    DX11_MODEL            model;                    // ���f�����

    bool                use;

};


//**
// �v���g�^�C�v�錾
//**
HRESULT InitEffect(void);
void UninitEffect(void);
void UpdateEffect(void);
void DrawEffect(void);

void SetEffect(XMFLOAT3 pos, int lifetimeFrame);