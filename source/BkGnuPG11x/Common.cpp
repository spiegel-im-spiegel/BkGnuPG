/************************************************************************/
/*  File name : Common.cpp                                              */
/************************************************************************/
/*  Contents  : ���ʊ֐��Q                                              */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.02          */
/*                                  Version 0.30    2000.10.08          */
/*                                  Version 0.32    2000.10.26          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/

/**** Incude Files ****/
#include <mbstring.h>
#include "BkGnuPGDef.h"
#include "BkGnuPGInfo.h"
#include "LogFile.h"
using namespace std;

#include "debug.h" //�Ō�ɒ�`


/**** Internal Macro ****/


/**** Typedef ****/


/**** Prototyping ****/


/*----------------------------------------------------------------------*/
/* Purpose        : ��O�������b�Z�[�W��\������                        */
/* Inputs         : Window �n���h��                                     */
/*                  ���b�Z�[�WID                                        */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
void ErrorMessage(HWND hWnd, UINT uID)
{
    try {
        char szError[256];
        if((LoadString(g_Info.m_hInstance, uID, szError, 256))>0) {
            g_LogFile.AppendMessage(szError);
            MessageBox(hWnd, szError, "Message from BkGnuPG", MB_OK | MB_ICONWARNING);
            g_LogFile.AppendMessage(szError);
        }
    }
    catch(...) {
        FatalErrorMessage(hWnd, __FILE__, __LINE__); //�G���[���e���o��
    }
}

/*----------------------------------------------------------------------*/
/* Purpose        : �v���I�G���[���b�Z�[�W��\������                    */
/* Inputs         : �Ȃ�                                                */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
void FatalErrorMessage(HWND hWnd, const char* fileName, int lineno, const char* pMsg)
{
    try {
        DWORD errNo = GetLastError();
        string ferrMsg;
        if(pMsg==NULL) {
            LPVOID p;
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                        NULL,
                        errNo,
                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                        (LPTSTR)&p, 0, NULL);
            ferrMsg = (char*)p;
            LocalFree(p);
        }
        else {
            ferrMsg = pMsg;
        }
        char szError[256];
        if((LoadString(g_Info.m_hInstance, IDS_GPG_EXCEPTION, szError, 256))>0) {
            string errMsg = szError;
            errMsg += "\n\n\"" + ferrMsg;
            TrimSpace(errMsg);
            errMsg += "\" ";
            if(errNo) { //�G���[�R�[�h��0�ȊO?
                MessageBox(hWnd, errMsg.c_str(), "Message from BkGnuPG", MB_OK | MB_ICONERROR);
            }
            string file = fileName;
            string::size_type idx = file.find_last_of("\\");
            if(idx!=string::npos) {
                errMsg += file.substr(idx+1);
            }
            else {
                errMsg += file;
            }
            g_LogFile.AppendValue(errMsg.c_str(), lineno); //�l���o�͂���
        }
    }
    catch(...) {
        return ; //�ǂ����悤���Ȃ��̂ŉ������Ȃ�
    }
}

/*----------------------------------------------------------------------*/
/* Purpose        : �t�@�C�����Ƃ��ė��p�\�Ȗ��O�ɕϊ�����            */
/* Inputs         : �C�ӂ̖��O                                          */
/*                  �t�@�C�����i�[�o�b�t�@                              */
/* Ootput         : �t�@�C����                                          */
/*                                                                      */
/* �� BkCommon.cpp �̓����֐���STL������                                */
/*----------------------------------------------------------------------*/
string& GetLegalFileName(const char* name, string& fileName)
{
    string namebuf = name;
    fileName = "";
    //�O��̋󔒕������폜
    TrimSpace(namebuf);

    //�����ϊ�
    string::size_type len = namebuf.length();
    if(len==0) { //�󔒕��������Ȃ�
        return fileName;
    }
    for(string::size_type i=0; i<len; i++) {
        unsigned char c = (unsigned char)namebuf[i];
        if ((_ismbblead(c))!=0) { //�}���`�o�C�g������1byte��
            fileName += (char)c;
            i++;
            fileName += namebuf[i];
        }
        else if(c<' ' || c==0x7f) { //ASCII�ȊO
            c = ' ';
            fileName += (char)c;
        }
        else {
            switch (c) {
            case '*':
            case '<':
            case '>':
            case '?':
            case '\\':
            case '|':
            case '/':
                c = '=';
                break;
            case ':':
                c = ';';
                break;
            case '\"':
                c = '\'';
                break;
            default:
                break;
            }
            fileName += (char)c;
        }
    }

    //�O��̋󔒕������폜
    TrimSpace(fileName);

    //������'.'������΍폜
    len = fileName.length();
    if(len>0 && fileName[len-1]=='.') {
        fileName = fileName.substr(0, len-1);
    }

    return fileName;
}

/*----------------------------------------------------------------------*/
/* Purpose        : �e�L�X�g�t�@�C�����e�𕶎�����Ƃ��ă������Ƀ��[�h����*/
/* Inputs         : �t�@�C���ւ̃p�X��                                  */
/* Ootput         : �t�@�C�����e(������)                                */
/*                                                                      */
/* �� BkCommon.cpp �̓����֐��̕ʎ����B�������̊m�ۂ� CBeckyAPI::Alloc  */
/*    ���g�p����̂Ŏ�舵������                                        */
/*----------------------------------------------------------------------*/
char* FileToString(const string& filepath)
{
    //�t�@�C���I�[�v��
    HANDLE hFile = CreateFile(filepath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile==INVALID_HANDLE_VALUE) {
        return NULL;
    }
    DWORD nSize = GetFileSize(hFile, NULL); //4GB�ȏ�̃T�C�Y�ɂ͑Ή����Ȃ�
    if(nSize==0) {
        CloseHandle(hFile);
        return NULL;
    }

    //�t�@�C�����e�̃R�s�[
    char* strBuf = (char*)g_Info.m_BkApi.Alloc(nSize+4);
    if(strBuf!=NULL) {
        DWORD lSize;
        if((ReadFile(hFile, strBuf, nSize, &lSize, NULL))==FALSE) {
            g_Info.m_BkApi.Free(strBuf);
            strBuf = NULL;
        }
        else {
            strBuf[lSize] = '\0';
        }
    }

    //�t�@�C���N���[�Y
    CloseHandle(hFile);
    return strBuf;
}

/*----------------------------------------------------------------------*/
/* Purpose        : �uname <email>�v�`���̕������v�f�ɕ�������        */
/* Inputs         : ���ɂȂ镶����                                      */
/*                  ���O�i�[�o�b�t�@                                    */
/*                  e-mail�i�[�o�b�t�@                                  */
/* Ootput         : �Ȃ�                                                */
/*                                                                      */
/* �� BkCommon.cpp �̓����֐���STL������                                */
/*----------------------------------------------------------------------*/
void GetNameAndAddr(const char* src, string& name, string& email)
{
    name = email = "";
    string srcStr = src;
    TrimSpace(srcStr); //�O��̋󔒂�����
    string::size_type len = srcStr.length();
    if(len==0) { //���g���Ȃ�
        return;
    }

    // name <email> �`���Ō�������
    bool bQuote = false;
    string::size_type idx;
    for(idx=0; idx<len; idx++) {
        unsigned char c = (unsigned char)srcStr[idx];
        if ((_ismbblead(c))!=0) { //�}���`�o�C�g������1byte��
            idx++;
        }
        else if(c=='\"') {
            bQuote = !bQuote;
        }
        else if(c=='<' && bQuote==false) {
            break;
        }
    }
    if(idx!=len) { //�f���~�^���o
        string sbuf = srcStr.substr(idx+1);
        string::size_type idx2 = sbuf.find_first_of('>');
        email = sbuf.substr(0, idx2);
        TrimSpace(email); //�O��̋󔒂�����
        if(idx>0) {
            name = srcStr.substr(0, idx);
            TrimSpace(name); //�O��̋󔒂�����
        }
        if((name.length())==0 && idx2!=string::npos) { // <email> name ��������Ȃ�
            idx2++;
            if((sbuf.length())<idx2) {
                name = sbuf.substr(idx2);
                TrimSpace(name); //�O��̋󔒂�����
            }
        }
        return;
    }
    else { // email (name) �`���ŒT���Ă݂�
        idx = srcStr.find_first_of('(');
        if(idx==string::npos) { //e-mail�݂̂̋L�q?
            email = srcStr;
        }
        else {
            string sbuf = srcStr.substr(idx+1);
            string::size_type idx2 = sbuf.find_first_of(')');
            name = sbuf.substr(0, idx2);
            TrimSpace(name); //�O��̋󔒂�����
            if(idx>0) {
                email = srcStr.substr(0, idx);
                TrimSpace(email); //�O��̋󔒂�����
            }
            if((sbuf.length())==0 && idx2!=string::npos) { // (name) email ��������Ȃ�
                idx2++;
                if((sbuf.length())<idx2) {
                    email = sbuf.substr(idx2);
                    TrimSpace(email); //�O��̋󔒂�����
                }
            }
        }
    }

    return;
}

/*----------------------------------------------------------------------*/
/* Purpose        : �t�@�C���̗L����₢���킹��                        */
/* Inputs         : �t�@�C����(�t���p�X)                                */
/* Ootput         : Boolearn                                            */
/*                                                                      */
/* �� BkCommon.cpp �̓����֐���STL������                                */
/*----------------------------------------------------------------------*/
bool IsFileExist(const string& path)
{
    WIN32_FIND_DATA FindData;
    HANDLE hFind = ::FindFirstFile(path.c_str(), &FindData);
    if (hFind != INVALID_HANDLE_VALUE) {
        ::FindClose(hFind);
        if((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==0) { //�f�B���N�g���łȂ�
            return true;
        }
    }
    return false;
}

/*----------------------------------------------------------------------*/
/* Purpose        : �t�@�C���폜 (���C���h�J�[�h�L��)                   */
/* Inputs         : �t�@�C����(�t���p�X)                                */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
void RemoveFiles(const string& path)
{
    string dpath;
    try {
        //�f�B���N�g�������擾����
        string::size_type idx = path.find_last_of("\\");
        string dir = "";
        if(idx!=string::npos) {
            dir = path.substr(0, idx+1);
        }
        //�t�@�C�������ƍ폜
        WIN32_FIND_DATA FindData;
        HANDLE hFind = ::FindFirstFile(path.c_str(), &FindData);
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==0) { //�f�B���N�g���łȂ�
                    dpath = dir + FindData.cFileName; //�p�X�쐬
                    DeleteFile(dpath.c_str()); //�폜!
#ifdef _DEBUG
                    g_LogFile.AppendValue("delete file", dpath);
#endif
                }
            } while((FindNextFile(hFind, &FindData))!=FALSE);
            ::FindClose(hFind);
        }
    }
    catch(...) {
        g_LogFile.AppendValue("cannot delete file", dpath);
    }
    return;
}

/*----------------------------------------------------------------------*/
/* Purpose        : �O��̋󔒕�������菜��                            */
/* Inputs         : ������f�[�^                                        */
/* Ootput         : ������̕�����f�[�^                                */
/*----------------------------------------------------------------------*/
string& TrimSpace(string& element)
{
    string::size_type idx = element.find_first_not_of(" \t\r\n");
    string str;
    if(idx!=string::npos) {
        element = str = element.substr(idx); //�擪�̋󔒕���������
    }
    else { //�󔒕��������Ȃ�(^^;)
        return element = "";
    }
    idx = str.find_last_not_of(" \t\r\n");
    if(idx!=string::npos) {
        element = str.substr(0, idx+1); //�����̋󔒕���������
    }

    return element;
}

/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
