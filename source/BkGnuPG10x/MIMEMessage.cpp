/************************************************************************/
/*  File name : MIMEMessage.cpp                                         */
/************************************************************************/
/*  Contents  : MIME ���[�����b�Z�[�W                                   */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.20    2000.10.02          */
/*                                  Version 0.21    2000.10.04          */
/*                                  Version 0.30    2000.10.08          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/

/**** Incude Files ****/
#include "BkGnuPGDef.h"
#include "MIMEMessage.h"
#include "BkGnuPGInfo.h"
using namespace std;

#include "debug.h" //�Ō�ɒ�`


/**** Internal Macro ****/


/**** Typedef ****/


/**** Constants ****/
const int CBigBuffer::m_Capacity = 65536; //�o�b�t�@�̃u���b�N�P��


/**** Prototyping ****/


/*----------------------------------------------------------------------*/
/* Purpose        : �����o�b�t�@�����������������                      */
/* Inputs         : Bekcy! API �g�p���t���O                           */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
void CBigBuffer::Reset(bool bBkAPI)
{
    //�o�b�t�@�̉��
    if(m_Buffer!=NULL) {
        if(m_bBkAPI==true) {
            g_Info.m_BkApi.Free(m_Buffer);
        }
        else {
            free(m_Buffer);
        }
    }

    //����������
    m_bBkAPI = bBkAPI;
    Init();
}

/*----------------------------------------------------------------------*/
/* Purpose        : �o�b�t�@�ɕ������ǉ�����                          */
/* Inputs         : ������                                              */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
void CBigBuffer::AddStr(const char* str)
{
    if(str==NULL) {
        return;
    }
    else if((*str)=='\0') {
        if(m_Buffer==NULL) {
            m_nBlock = 1;
            m_nSize = 0;
            if(m_bBkAPI==true) {
                m_Buffer = (char*)g_Info.m_BkApi.Alloc(m_Capacity*m_nBlock);
            }
            else {
                m_Buffer = (char*)malloc(m_Capacity*m_nBlock);
            }
            m_Buffer[m_nSize] = '\0';
        }
        return;
    }

    //�L���p�V�e�B�̃`�F�b�N
    int len = strlen(str);
    for(;;) {
        int rest = m_Capacity*m_nBlock - m_nSize;
        if(rest<=len) {
            rest = len - rest;
            m_nBlock += rest/m_Capacity + 1;
            if(m_Buffer==NULL) {
                if(m_bBkAPI==true) {
                    m_Buffer = (char*)g_Info.m_BkApi.Alloc(m_Capacity*m_nBlock);
                }
                else {
                    m_Buffer = (char*)malloc(m_Capacity*m_nBlock);
                }
                m_Buffer[0] = '\0';
            }
            else {
                if(m_bBkAPI==true) {
                    m_Buffer = (char*)g_Info.m_BkApi.ReAlloc(m_Buffer, m_Capacity*m_nBlock);
                }
                else {
                    m_Buffer = (char*)realloc(m_Buffer, m_Capacity*m_nBlock);
                }
            }
        }
        else {
            break;
        }
    }

    //������̒ǉ�
    strcpy(&m_Buffer[m_nSize], str);
    m_nSize += len;

    return;
}

/*----------------------------------------------------------------------*/
/* Purpose        : �����̉��s�R�[�h���폜����                          */
/* Inputs         : �Ȃ�                                                */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
void CBigBuffer::RollBack(void)
{
    if(m_nSize>=2 && m_Buffer[m_nSize-2]=='\r' && m_Buffer[m_nSize-1]=='\n') {
        m_nSize -= 2;
        m_Buffer[m_nSize] = '\0';
    }
}

/*----------------------------------------------------------------------*/
/* Purpose        : �o�b�t�@�̃G�N�X�|�[�g                              */
/* Inputs         : ������                                              */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
char* CBigBuffer::Export(void)
{
    //�K���ȃT�C�Y�ɐ؂�l�߂� �� ���܂�Ӗ��Ȃ�����
    //if(m_bBkAPI==true) {
    //    m_Buffer = (char*)g_Info.m_BkApi.ReAlloc(m_Buffer, m_nSize+4);
    //}
    //else {
    //    m_Buffer = (char*)realloc(m_Buffer, m_nSize+4);
    //}

    char* p = m_Buffer;
    Init(); //�������͏���������
    return p;
}

/*----------------------------------------------------------------------*/
/* Purpose        : ������                                              */
/* Inputs         : �R�s�[���̃C���X�^���X                              */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
void CMIMEMessage::Init(void)
{
    ClearNext();
    ClearChild();
    m_Type = "";
    m_SubType = "";
    m_Boundary = "";
    m_lstHeaders.clear();
    //m_lstBody.clear();
    //m_lstTrail.clear();
    m_Body.Reset(false);
    m_Trail.Reset(false);
}

/*----------------------------------------------------------------------*/
/* Purpose        : �C���X�^���X�̃R�s�[                                */
/* Inputs         : �R�s�[���̃C���X�^���X                              */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
void CMIMEMessage::Copy(const CMIMEMessage& org)
{
    if(org.m_pNext!=NULL) {
        SetNext(*org.m_pNext);
    }
    else {
        ClearNext();
    }
    if(org.m_pChild!=NULL) {
        SetChild(*org.m_pChild);
    }
    else {
        ClearChild();
    }
    m_Type = org.m_Type;
    m_SubType = org.m_SubType;
    m_Boundary = org.m_Boundary;
    m_lstHeaders = org.m_lstHeaders;
    //m_lstBody = org.m_lstBody;
    //m_lstTrail = org.m_lstTrail;

    m_Body.Reset(false);
    m_Body.AddStr(org.m_Body.Reference());
    m_Trail.Reset(false);
    m_Trail.AddStr(org.m_Trail.Reference());
}

/*----------------------------------------------------------------------*/
/* Purpose        : ����MIME���b�Z�[�W���Z�b�g                          */
/* Inputs         : MIME���b�Z�[�W                                      */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
void CMIMEMessage::SetNext(const CMIMEMessage& next)
{
    ClearNext();
    CMIMEMessage* item = new CMIMEMessage(next);
    m_pNext = item;
}

/*----------------------------------------------------------------------*/
/* Purpose        : �qMIME���b�Z�[�W���Z�b�g                            */
/* Inputs         : MIME���b�Z�[�W                                      */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
void CMIMEMessage::SetChild(const CMIMEMessage& child)
{
    ClearChild();
    CMIMEMessage* item = new CMIMEMessage(child);
    m_pChild = item;
}

/*----------------------------------------------------------------------*/
/* Purpose        : �qMIME���b�Z�[�W��ǉ�                              */
/* Inputs         : MIME���b�Z�[�W                                      */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
void CMIMEMessage::AddChild(const CMIMEMessage& child)
{
    CMIMEMessage* pNext = GetChild();
    if (pNext==NULL) {
        SetChild(child);
        return;
    }
    for(; (pNext->GetNext())!=NULL; pNext=pNext->GetNext());
    pNext->SetNext(child);
}

/*----------------------------------------------------------------------*/
/* Purpose        : MIME���b�Z�[�W���㏑���R�s�[����                    */
/* Inputs         : MIME���b�Z�[�W                                      */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
void CMIMEMessage::OverWrite(CMIMEMessage& item)
{
    //Header �̏㏑��
    m_Type = item.m_Type;
    m_SubType = item.m_SubType;
    m_Boundary = item.m_Boundary;
    for(vector<string>::iterator p=item.m_lstHeaders.begin(); p!=item.m_lstHeaders.end(); p++) {
        string::size_type idx = p->find_first_of(':');
        if(idx!=string::npos) {
            string name = p->substr(0, idx);
            string value = "";
            idx = p->find_first_not_of(" \t\r\n", idx+1);
            if(idx!=string::npos) {
                value = p->substr(idx);
            }
            SetHeader(name.c_str(), value.c_str());
        }
    }

    //�qMIME���b�Z�[�W���Z�b�g
    ClearChild();
    if(item.m_pChild!=NULL) {
        SetChild(*item.m_pChild);
    }
    ClearNext();
    if(item.m_pNext!=NULL) {
        SetNext(*item.m_pNext);
    }

    //Body ���Z�b�g
    m_Body.Reset(false);
    m_Body.AddStr(item.m_Body.Reference());
    m_Trail.Reset(false);
    m_Trail.AddStr(item.m_Trail.Reference());
}

/*----------------------------------------------------------------------*/
/* Purpose        : Body�֘A�̏��(Header�܂�)�𔲂��o��                */
/* Inputs         : MIME���b�Z�[�W�i�[�p�C���X�^���X                    */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
CMIMEMessage& CMIMEMessage::GetBody(CMIMEMessage& item)
{
    //������
    item.Init();

    //�w�b�_���̒��o
    item.m_Type = m_Type;
    item.m_SubType = m_SubType;
    item.m_Boundary = m_Boundary;
    string sbuf;
    GetHeader("Content-Type", sbuf);
    //SepareteHeaderValue(sbuf);
    item.SetHeader("Content-Type", sbuf.c_str());
    GetHeader("Content-Transfer-Encoding", sbuf);
    item.SetHeader("Content-Transfer-Encoding", sbuf.c_str());

    //�qMIME���b�Z�[�W���Z�b�g
    item.ClearChild();
    if(m_pChild!=NULL) {
        item.SetChild(*m_pChild);
    }
    item.ClearNext();
    if(m_pNext!=NULL) {
        item.SetNext(*m_pNext);
    }

    //Body ���Z�b�g
    item.m_Body.Reset(false);
    item.m_Body.AddStr(m_Body.Reference());
    item.m_Trail.Reset(false);
    item.m_Trail.AddStr(m_Trail.Reference());

    return item;
}

/*----------------------------------------------------------------------*/
/* Purpose        : ���b�Z�[�W�̃C���|�[�g                              */
/* Inputs         : ���b�Z�[�W                                          */
/*                  �o�E���_��������                                    */
/* Ootput         : ���̃|�C���^                                        */
/*----------------------------------------------------------------------*/
const char* CMIMEMessage::FromString(const char* src, const char* boundParent)
{
    //������
    Init();

    const char* str = src;
    const char* org = str;
    bool bBody = false;
    for(;;) {
        string lineBuf;
        str = GetLine(str, lineBuf);
        if((lineBuf.length())==0) { //�����񂪂Ȃ�
            break;
        }
        if(bBody==false) { //�w�b�_��
            if(lineBuf=="\r\n") { //�w�b�_�̏I���
                //m_lstHeaders.pushback(lineBuf);
                bBody = true;
            }
            else {
                while (str!=NULL && ((*str)==' ' || (*str)=='\t')) { //�w�b�_�̑���
                    string sbuf;
                    str = GetLine(str, sbuf);
                    lineBuf += sbuf;
                }
                m_lstHeaders.push_back(lineBuf);
                if(strnicmp(lineBuf.c_str(), "Content-Type:", 13)==0) { //Content-Type
                    string::size_type idxType = lineBuf.find_first_not_of(" \t", 13);
                    string sbuf = lineBuf.substr(idxType);
                    idxType = sbuf.find_first_of("/ \t");
                    string::size_type idxSubType = string::npos;
                    if(idxType!=string::npos) {
                        idxSubType = sbuf.find_first_of("; \r\n\t", idxType+1);
                    }
                    if(idxType!=string::npos && idxSubType!=string::npos) {
                        m_Type = sbuf.substr(0, idxType);
                        m_SubType = sbuf.substr(idxType+1, idxSubType-idxType-1);
                        string option = sbuf.substr(idxSubType+1);
                        for(; (option.length())>0; ) {
                            string opt, val;
                            GetOptValue(option, opt, val);
                            if(stricmp(opt.c_str(), "boundary") == 0) {
                                m_Boundary = val;
                            }
                        }
                    }
                }
            }
        }
        else { //Body
            if((lineBuf.compare(0, 2, "--"))==0) { //�o�E���_��?
                string bound;
                string::size_type idx = lineBuf.find_first_of("\r\n", 2);
                if(idx!=string::npos) {
                    bound = lineBuf.substr(2, idx-2);
                }
                else {
                    bound = lineBuf.substr(2);
                }
                if((m_Boundary.length())>0) { // multipart
                    if(bound==m_Boundary) {
                        CMIMEMessage sub;
                        str = sub.FromString(str, m_Boundary.c_str());
                        AddChild(sub);
                        org = str;
                        continue;
                    }
                    else if((m_Boundary+"--")==bound) { //�o�E���_���̏I���
                        m_Body.RollBack(); //Body�����݂���Ȃ烍�[���o�b�N���Ă���
                        if(boundParent!=NULL) { //�eMIME�̃o�E���_��������
                            for(;;) {
                                string lineBuf2;
                                str = GetLine(str, lineBuf2);
                                if((lineBuf2.length())==0) { //�����񂪂Ȃ�
                                    break;
                                }
                                if((lineBuf2.compare(2, strlen(boundParent), boundParent))==0) {
                                    //m_lstTrail.pop_back();
                                    m_Trail.RollBack();
                                    str = org;
                                    break;
                                }
                                //m_lstTrail.push_back(lineBuf2);
                                m_Trail.AddStr(lineBuf2.c_str());
                                org = str;
                            }
                        }
                        break;
                    }
                }
                if(boundParent!=NULL) { //�eMIME�̃o�E���_��
                    if((lineBuf.compare(2, strlen(boundParent), boundParent))==0) {
                        //m_lstBody.pop_back();
                        m_Body.RollBack();
                        str = org;
                        break;
                    }
                }
            }
            //m_lstBody.push_back(lineBuf);
            m_Body.AddStr(lineBuf.c_str());
        }
        org = str;
    }

    return str;
}

/*----------------------------------------------------------------------*/
/* Purpose        : ���b�Z�[�W�̃G�N�X�|�[�g                            */
/* Inputs         : �Ȃ�                                                */
/* Ootput         : ���b�Z�[�W������                                    */
/*                                                                      */
/* �� �������̊m�ۂ� CBeckyAPI::Alloc ���g�p����̂Ŏ�舵������        */
/*----------------------------------------------------------------------*/
char* CMIMEMessage::ToString(void)
{
    CBigBuffer buffer; //���b�Z�[�W�i�[�o�b�t�@

    //Headder
    for(vector<string>::iterator p=m_lstHeaders.begin(); p!=m_lstHeaders.end(); p++) {
        buffer.AddStr(p->c_str()); //Headder�̒ǉ�
    }

    //Body
    const char* body = m_Body.Reference();
    if(body!=NULL) {
        buffer.AddStr("\r\n");
        buffer.AddStr(m_Body.Reference());
    }

    //Child
    if((GetChild())!=NULL) {
        //buffer.AddStr("\r\n");
        CMIMEMessage* item = GetChild();
        for(;item!=NULL;) {
            //�o�E���_��
            buffer.AddStr("\r\n--");
            buffer.AddStr(m_Boundary.c_str());
            buffer.AddStr("\r\n");
            //�{��
            char* substr = item->ToString();
            buffer.AddStr(substr);
            g_Info.m_BkApi.Free(substr);
            //buffer.AddStr("\r\n");
            item = item->GetNext();
        }
        buffer.AddStr("\r\n--");
        buffer.AddStr(m_Boundary.c_str());
        buffer.AddStr("--\r\n");
        //Trail
        buffer.AddStr(m_Trail.Reference());
    }

    return buffer.Export();
}

/*----------------------------------------------------------------------*/
/* Purpose        : �w�b�_���̌���                                    */
/* Inputs         : �t�B�[���h��                                        */
/*                  �t�B�[���h���                                      */
/* Ootput         : �t�B�[���h���                                      */
/*----------------------------------------------------------------------*/
string& CMIMEMessage::GetHeader(const char* header, string& dataBuf)
{
    dataBuf = "";
    int len = strlen(header);
    for(vector<string>::iterator p=m_lstHeaders.begin(); p!=m_lstHeaders.end(); p++) {
        //if((p->compare(0, len, header))==0 && p->at(len)==':') {
        string sbuf;
        sbuf = p->substr(0, len);
        if((stricmp(sbuf.c_str(), header))==0 && p->at(len)==':') {
            dataBuf = p->substr(len+1);
            string::size_type idx = dataBuf.find_first_not_of(" \t\r\n");
            if(idx!=string::npos) {
                dataBuf = dataBuf.substr(idx); //�擪�̋󔒕���������
            }
            break;
        }
    }

    return dataBuf;
}

/*----------------------------------------------------------------------*/
/* Purpose        : �w�b�_�����Z�b�g����                              */
/* Inputs         : �t�B�[���h��                                        */
/*                  �t�B�[���h���                                      */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
bool CMIMEMessage::SetHeader(const char* header, const char* data)
{
    if(header==NULL || *header=='\0') {
        return false;
    }

    //�w�b�_��񌟍�
    int len = strlen(header);
    for(vector<string>::iterator p=m_lstHeaders.begin(); p!=m_lstHeaders.end(); p++) {
        //if((p->compare(0, len, header))==0 && p->at(len)==':') {
        string sbuf;
        sbuf = p->substr(0, len);
        if((stricmp(sbuf.c_str(), header))==0 && p->at(len)==':') {
            if(data==NULL || *data=='\0') { //�t�B�[���h���폜����
                m_lstHeaders.erase(p);
            }
            else { //���������ւ���
                sbuf = header;
                sbuf += ": ";
                sbuf += data;
                (*p) = sbuf;
            }
            return true;
        }
    }

    //�w�b�_����ǉ�����
    if(data!=NULL && *data!='\0') {
        string sbuf = header;
        sbuf += ": ";
        sbuf += data;
        m_lstHeaders.push_back(sbuf);
    }

    return true;
}

/*----------------------------------------------------------------------*/
/* Purpose        : �w�肵���^�C�v��MIME���b�Z�[�W��T��                */
/* Inputs         : �^�C�v��                                            */
/*                  �T�u�^�C�v��                                        */
/* Ootput         : MIME���b�Z�[�W                                      */
/*----------------------------------------------------------------------*/
CMIMEMessage* CMIMEMessage::FindCMIMEMessage(const char* type, const char* subType)
{
    //������MIME�^�C�v�𒲂ׂ�
    bool bType = (type!=NULL && *type!='\0');
    bool bSubType = (subType!=NULL && *subType!='\0');
    if((bType==false || (bType==true && (stricmp(m_Type.c_str(), type))==0))
            && (bSubType==false || (bSubType==true && (stricmp(m_SubType.c_str(), subType))==0))) {
        return this;
    }

    //�qMIME��T��
    CMIMEMessage* item = GetChild();
    if(item!=NULL) {
        item = item->FindCMIMEMessage(type, subType);
    }
    //����MIME��T��
    if(item==NULL) {
        item = GetNext();
        if(item!=NULL) {
            item = item->FindCMIMEMessage(type, subType);
        }
    }

    return item;
}

/*----------------------------------------------------------------------*/
/* Purpose        : ��s���̕�������擾                                */
/* Inputs         : ���b�Z�[�W                                          */
/*                  ������o�b�t�@                                      */
/* Ootput         : ���̃|�C���^                                        */
/*----------------------------------------------------------------------*/
const char* CMIMEMessage::GetLine(const char* src, string& lineBuf)
{
    lineBuf = "";
    if(src==NULL || *src=='\0') {
        return NULL;
    }
    for(const char* p=src;; p++) {
        if(*p=='\0') {
            return NULL; //�����Ȃ�
        }
        else if(*p=='\r') {
            if(*(p+1)=='\n') {
                p++;
            }
            lineBuf += "\r\n";
            p++;
            break;
        }
        else if(*p=='\n') {
            lineBuf += "\r\n"; //���K��
            p++;
            break;
        }
        else {
            lineBuf += *p;
        }
    }

    return p;
}

/*----------------------------------------------------------------------*/
/* Purpose        : �����񂩂� opt=val �̏��𔲂��o��                 */
/* Inputs         : ������                                              */
/*                  �I�v�V������                                        */
/*                  �I�v�V�����̒l                                      */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
string& CMIMEMessage::GetOptValue(string& option, string& opt, string& val)
{
    opt = val = "";
    string::size_type idx = option.find_first_not_of(" \t\r\n;");
    if(idx!=string::npos) {
        option = option.substr(idx); //�擪�̋󔒕���������
    }
    else { //�L���ȕ�����Ȃ�
        return option = "";
    }
    idx = option.find_first_of('=');
    if(idx==string::npos) { //opt=val �`���̏�񂪂Ȃ�����
        return option = "";
    }
    opt = option.substr(0, idx);
    string::size_type idx2 = option.find_first_not_of(" \t\r\n;", idx+1);
    if(idx2!=string::npos) {
        option = option.substr(idx2); //�擪�̋󔒕���������
    }
    else { //�L���ȕ�����Ȃ�
        return option = "";
    }
    if(option[0]=='\"') {
        idx = option.find_first_of('\"', 1);
        if(idx==string::npos) {
            val = option.substr(1);
            option = "";
        }
        else {
            val = option.substr(1, idx-1);
            option = option.substr(idx+1);
        }
    }
    else {
        idx = option.find_first_of(" \t\r\n;");
        if(idx!=string::npos) {
            val = option.substr(0, idx);
            option = option.substr(idx+1); //�擪�̋󔒕���������
        }
        else { //�L���ȕ�����Ȃ�
            val = option;
            option = "";
        }
    }
    idx = option.find_first_not_of(" \t\r\n;");
    if(idx!=string::npos) {
        option = option.substr(idx); //�擪�̋󔒕���������
    }
    else { //�L���ȕ�����Ȃ�
        option = "";
    }

    return option;
}

#if 0
/*----------------------------------------------------------------------*/
/* Purpose        : �t�B�[���h�����I�v�V�������Ƃɉ��s����            */
/* Inputs         : ������                                              */
/* Ootput         : ���`��̕�����                                      */
/*----------------------------------------------------------------------*/
string& CMIMEMessage::SepareteHeaderValue(string& value)
{
    for(;;) {
        string::size_type idx = value.find("; ");
        if(idx!=string::npos) {
            string str = value.substr(0, idx);
            str += ";\r\n";
            str += value.substr(idx+1);
            value = str;
        }
        else {
            break;
        }
    }

    return value;
}
#endif

/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
