/************************************************************************/
/*  File name : LogFile.h                                               */
/************************************************************************/
/*  Contents  : �f�o�b�O�p���O�t�@�C��                                  */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.02          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/
#ifndef _LOGFILE_H_
#define _LOGFILE_H_

/**** Incude Files ****/
#include <string>
#include <fstream>


/**** Global Macro ****/


/**** Typedef ****/


/**** Prototyping ****/


/*----------------------------------------------------------------------*/
/* Class Name : CLogFile                                                */
/* Purpose    : �f�o�b�O�p���O�t�@�C��                                  */
/*----------------------------------------------------------------------*/
class CLogFile
{
// �����o�C���X�^���X
private:
    //�萔��`
    //�����ϐ�
    bool m_bWrite; //�������݋��t���O
    std::string m_LogPath; //���O�t�@�C���ւ̃p�X
public:
    //�萔��`

// �����o���\�b�h
private:
    //�C���X�^���X�̃R�s�[����点�Ȃ����߂̑[�u
    CLogFile(const CLogFile& org); //�R�s�[�R���X�g���N�^
    CLogFile& operator=(const CLogFile& org); //������Z�q
    //���������֐�
    std::string TimeString(void); //���ݎ����𕶎���ϊ�����
public:
    //�R���X�g���N�^�E�f�X�g���N�^
    CLogFile(const char* path=NULL) : m_bWrite(false) { Reset(path); } //�f�t�H���g�R���X�g���N�^
    virtual ~CLogFile() {} //�f�X�g���N�^
    //���Z�q
    //�C���^�t�F�[�X�֐�
    void Reset(const char* path); //����������
    void AppendMessage(const char* msg); //���b�Z�[�W�̏o��
    template<class T> void AppendValue(const char* caption, const T& val) //�l���o�͂���
    {
        using namespace std;
        if(m_bWrite==true) {
            try {
                //���b�Z�[�W��ǋL����
                ofstream of(m_LogPath.c_str(), ios_base::app); //�ǋL���[�h�ŃI�[�v��
                //�����ƂƂ��Ƀ��b�Z�[�W���o��
                of << TimeString() << ": " << caption << " = " << val << endl;
                of.close();
            }
            catch(...) {
                return; //�ǂ����悤���Ȃ��̂ŉ������Ȃ�
            }
        }
    }
};

/**** External Declarelation ****/
extern CLogFile g_LogFile; //���O�t�@�C�� ����


#endif // _LOGFILE_H_
/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
