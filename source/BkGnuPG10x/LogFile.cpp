/************************************************************************/
/*  File name : LogFile  .cpp                                           */
/************************************************************************/
/*  Contents  : �f�o�b�O�p���O�t�@�C��                                  */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.02          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/

/**** Incude Files ****/
#include "LogFile.h"

#include <ctime>
using namespace std;

#include "debug.h" //�Ō�ɒ�`


/**** Internal Macro ****/


/**** Typedef ****/


/**** Prototyping ****/


/*----------------------------------------------------------------------*/
/* Purpose        : ����������                                          */
/* Inputs         : ���O�t�@�C���ւ̃p�X                                */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
void CLogFile::Reset(const char* path)
{
    if(path==NULL) {
        m_bWrite = false;
        m_LogPath = "";
    }
    else {
        m_bWrite = true;
        m_LogPath = path;
    }

    return;
}

/*----------------------------------------------------------------------*/
/* Purpose        : ���b�Z�[�W�̏o��                                    */
/* Inputs         : ���b�Z�[�W������                                    */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
void CLogFile::AppendMessage(const char* msg)
{
    if(m_bWrite==false) {
        return;
    }

    //���b�Z�[�W��ǋL����
    try {
        ofstream of(m_LogPath.c_str(), ios_base::app); //�ǋL���[�h�ŃI�[�v��
        //�����ƂƂ��Ƀ��b�Z�[�W���o��
        of << TimeString() << ": " << msg << endl;
        of.close();
    }
    catch(...) {
        return;
    }

    return;
}

/*----------------------------------------------------------------------*/
/* Purpose        : ���ݎ����𕶎���ϊ�����                            */
/* Inputs         : �Ȃ�                                                */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
string CLogFile::TimeString(void)
{
    time_t sysTm;
    time(&sysTm); //�V�X�e���^�C�}�l�擾
    struct tm *t = localtime(&sysTm);
    string s = asctime(t);
    string::size_type len = s.find_first_of("\n");
    if(len!=string::npos) {
        s = s.substr(0, len);
    }

    return s;
}

/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
