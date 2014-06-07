/************************************************************************/
/*  File name : BkGnuPG.cpp                                             */
/************************************************************************/
/*  Contents  : Becky! GNU Privacy Guard プラグイン メイン処理          */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.01    2000.09.14          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.02          */
/*                                  Version 0.21    2000.10.03          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 0.32    2000.10.26          */
/*                                  Version 0.40    2000.10.31          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/

/**** Incude Files ****/
#include "BkGnuPGDef.h"
#include "BkGnuPG.h"
#include "BkGnuPGInfo.h"
#include "SetupDialog.h"
#include "GnuPGModule.h"
#include "CallBacks.h"
#include "LogFile.h"
#include <time.h>

#include "debug.h" //最後に定義


/**** Global Instance ****/
CBkGnuPGInfo g_Info; //BkGnuPG 制御情報クラス
CLogFile g_LogFile; //ログファイル


/**** Internal Macro ****/


/**** Typedef ****/


/**** Prototyping ****/


/////////////////////////////////////////////////////////////////////////////
// DLL entry point
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch(ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
#ifdef _DEBUG
        // memory leak 検索用
        int dbgFlag;
        dbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
        dbgFlag |= (_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
        _CrtSetDbgFlag(dbgFlag);
#endif
        try {
            //情報の初期化
            if((g_Info.InitInstance((HINSTANCE)hModule))==false) {
                return FALSE;
            }
            //ログファイルの初期化
            if(g_Info.m_bDebugLog==true) { //ログ出力あり
                g_LogFile.Reset(g_Info.m_LogPath.c_str());
            }
            else {
                g_LogFile.Reset(NULL);
            }
            time_t t;
            srand(time(&t)); //擬似乱数初期化
            g_LogFile.AppendMessage("BkGnuPG ATTACH");
        }
        catch(...) {
            FatalErrorMessage(NULL, __FILE__, __LINE__); //致命的エラー内容を出力
        }
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        g_LogFile.AppendMessage("BkGnuPG DETACH");
        break;
    }
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Callbacks from Becky!

#ifdef __cplusplus
extern "C"{
#endif

////////////////////////////////////////////////////////////////////////
// Called when the program is started and the main window is created.
int WINAPI BKC_OnStart()
{
    /*
    Since BKC_OnStart is called after Becky!'s main window is
    created, at least BKC_OnMenuInit with BKC_MENU_MAIN is called
    before BKC_OnStart. So, do not assume BKC_OnStart is called
    prior to any other callback.
    */
    // Always return 0.
    return 0;
}

////////////////////////////////////////////////////////////////////////
// Called when the main window is closing.
int WINAPI BKC_OnExit()
{
    // Return -1 if you don't want to quit.
    return 0;
}

////////////////////////////////////////////////////////////////////////
// Called when menu is intialized.
int WINAPI BKC_OnMenuInit(HWND hWnd, HMENU hMenu, int nType)
{
    switch (nType) {
    case BKC_MENU_MAIN:
        try {
            HMENU hSubMenu = GetSubMenu(hMenu, 4); //Toolメニューにセット
            AppendMenu(hSubMenu, MF_SEPARATOR, 0, NULL);
            char szMsg[82];
            if(g_Info.m_MainMenuID[CBkGnuPGInfo::Decript]==0) {
                LoadString(g_Info.m_hInstance, IDS_GPG_DECRYPT, szMsg, 80);
                g_Info.m_MainMenuID[CBkGnuPGInfo::Decript] = g_Info.m_BkApi.RegisterCommand(szMsg, nType, Decript);
            }
            LoadString(g_Info.m_hInstance, IDS_GPGMENU_DECRYPT, szMsg, 80);
            AppendMenu(hSubMenu, MF_STRING, g_Info.m_MainMenuID[CBkGnuPGInfo::Decript], szMsg);
        }
        catch(...) {
            FatalErrorMessage(hWnd, __FILE__, __LINE__); //致命的エラー内容を出力
        }
        break;
    case BKC_MENU_LISTVIEW:
        try {
            char szMsg[82];
            if(g_Info.m_LVMenuID[CBkGnuPGInfo::Decript]==0) {
                LoadString(g_Info.m_hInstance, IDS_GPG_DECRYPT, szMsg, 80);
                g_Info.m_LVMenuID[CBkGnuPGInfo::Decript] = g_Info.m_BkApi.RegisterCommand(szMsg, nType, Decript);
            }
            int max = GetMenuItemCount(hMenu);
            int i;
            for(i=0; i<max && (GetMenuItemID(hMenu, i))!=0x80A7; i++); //「プロパティ」メニュー項目を探す
            LoadString(g_Info.m_hInstance, IDS_GPGMENU_DECRYPT, szMsg, 80);
            InsertMenu(hMenu, i, MF_BYPOSITION | MF_STRING, g_Info.m_LVMenuID[CBkGnuPGInfo::Decript], szMsg); //「プロパティ」の前に項目を挿入する
        }
        catch(...) {
            FatalErrorMessage(hWnd, __FILE__, __LINE__); //致命的エラー内容を出力
        }
        break;
    case BKC_MENU_TREEVIEW:
        break;
    case BKC_MENU_MSGVIEW:
        try {
            char szMsg[82];
            if(g_Info.m_LVMenuID[CBkGnuPGInfo::Decript]==0) {
                LoadString(g_Info.m_hInstance, IDS_GPG_DECRYPT, szMsg, 80);
                g_Info.m_LVMenuID[CBkGnuPGInfo::Decript] = g_Info.m_BkApi.RegisterCommand(szMsg, nType, Decript);
            }
            int max = GetMenuItemCount(hMenu);
            int i;
            for(i=0; i<max && (GetMenuItemID(hMenu, i))!=0x80A7; i++); //「プロパティ」メニュー項目を探す
            LoadString(g_Info.m_hInstance, IDS_GPGMENU_DECRYPT, szMsg, 80);
            InsertMenu(hMenu, i, MF_BYPOSITION | MF_STRING, g_Info.m_LVMenuID[CBkGnuPGInfo::Decript], szMsg); //「プロパティ」の前に項目を挿入する
        }
        catch(...) {
            FatalErrorMessage(hWnd, __FILE__, __LINE__); //致命的エラー内容を出力
        }
        break;
    case BKC_MENU_MSGEDIT:
        break;
    case BKC_MENU_TASKTRAY:
        break;
    case BKC_MENU_COMPOSE:
        try {
            HMENU hSubMenu = GetSubMenu(hMenu, 3);
            AppendMenu(hSubMenu, MF_SEPARATOR, 0, NULL);
            char szMsg[82];

            if(g_Info.m_CompMenuID[CBkGnuPGInfo::Encrypt]==0) {
                LoadString(g_Info.m_hInstance, IDS_GPG_ENCRYPT, szMsg, 80);
                g_Info.m_CompMenuID[CBkGnuPGInfo::Encrypt] = g_Info.m_BkApi.RegisterCommand(szMsg, nType, Encrypt);
            }
            LoadString(g_Info.m_hInstance, IDS_GPGMENU_ENCRYPT, szMsg, 80);
            AppendMenu(hSubMenu, MF_STRING, g_Info.m_CompMenuID[CBkGnuPGInfo::Encrypt], szMsg);

            if(g_Info.m_CompMenuID[CBkGnuPGInfo::Sign]==0) {
                LoadString(g_Info.m_hInstance, IDS_GPG_SIGN, szMsg, 80);
                g_Info.m_CompMenuID[CBkGnuPGInfo::Sign] = g_Info.m_BkApi.RegisterCommand(szMsg, nType, Sign);
            }
            LoadString(g_Info.m_hInstance, IDS_GPGMENU_SIGN, szMsg, 80);
            AppendMenu(hSubMenu, MF_STRING, g_Info.m_CompMenuID[CBkGnuPGInfo::Sign], szMsg);

            if(g_Info.m_CompMenuID[CBkGnuPGInfo::EncryptSign]==0) {
                LoadString(g_Info.m_hInstance, IDS_GPG_SIGNENCRYPT, szMsg, 80);
                g_Info.m_CompMenuID[CBkGnuPGInfo::EncryptSign] = g_Info.m_BkApi.RegisterCommand(szMsg, nType, EncryptSign);
            }
            LoadString(g_Info.m_hInstance, IDS_GPGMENU_SIGNENCRYPT, szMsg, 80);
            AppendMenu(hSubMenu, MF_STRING, g_Info.m_CompMenuID[CBkGnuPGInfo::EncryptSign], szMsg);

            if(g_Info.m_CompMenuID[CBkGnuPGInfo::ExpPubKey]==0) {
                LoadString(g_Info.m_hInstance, IDS_GPG_ATTACHKEY, szMsg, 80);
                g_Info.m_CompMenuID[CBkGnuPGInfo::ExpPubKey] = g_Info.m_BkApi.RegisterCommand(szMsg, nType, PubKeyExport);
            }
            LoadString(g_Info.m_hInstance, IDS_GPGMENU_ATTACHKEY, szMsg, 80);
            AppendMenu(hSubMenu, MF_STRING, g_Info.m_CompMenuID[CBkGnuPGInfo::ExpPubKey], szMsg);
        }
        catch(...) {
            FatalErrorMessage(hWnd, __FILE__, __LINE__); //致命的エラー内容を出力
        }
        break;
    case BKC_MENU_COMPEDIT:
        try {
            char szMsg[82];

            AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
            if(g_Info.m_CompMenuID[CBkGnuPGInfo::Encrypt]==0) {
                LoadString(g_Info.m_hInstance, IDS_GPG_ENCRYPT, szMsg, 80);
                g_Info.m_CompMenuID[CBkGnuPGInfo::Encrypt] = g_Info.m_BkApi.RegisterCommand(szMsg, nType, Encrypt);
            }
            LoadString(g_Info.m_hInstance, IDS_GPGMENU_ENCRYPT, szMsg, 80);
            AppendMenu(hMenu, MF_STRING, g_Info.m_CompMenuID[CBkGnuPGInfo::Encrypt], szMsg);

            if(g_Info.m_CompMenuID[CBkGnuPGInfo::Sign]==0) {
                LoadString(g_Info.m_hInstance, IDS_GPG_SIGN, szMsg, 80);
                g_Info.m_CompMenuID[CBkGnuPGInfo::Sign] = g_Info.m_BkApi.RegisterCommand(szMsg, nType, Sign);
            }
            LoadString(g_Info.m_hInstance, IDS_GPGMENU_SIGN, szMsg, 80);
            AppendMenu(hMenu, MF_STRING, g_Info.m_CompMenuID[CBkGnuPGInfo::Sign], szMsg);

            if(g_Info.m_CompMenuID[CBkGnuPGInfo::EncryptSign]==0) {
                LoadString(g_Info.m_hInstance, IDS_GPG_SIGNENCRYPT, szMsg, 80);
                g_Info.m_CompMenuID[CBkGnuPGInfo::EncryptSign] = g_Info.m_BkApi.RegisterCommand(szMsg, nType, EncryptSign);
            }
            LoadString(g_Info.m_hInstance, IDS_GPGMENU_SIGNENCRYPT, szMsg, 80);
            AppendMenu(hMenu, MF_STRING, g_Info.m_CompMenuID[CBkGnuPGInfo::EncryptSign], szMsg);

            if(g_Info.m_CompMenuID[CBkGnuPGInfo::ExpPubKey]==0) {
                LoadString(g_Info.m_hInstance, IDS_GPG_ATTACHKEY, szMsg, 80);
                g_Info.m_CompMenuID[CBkGnuPGInfo::ExpPubKey] = g_Info.m_BkApi.RegisterCommand(szMsg, nType, PubKeyExport);
            }
            LoadString(g_Info.m_hInstance, IDS_GPGMENU_ATTACHKEY, szMsg, 80);
            AppendMenu(hMenu, MF_STRING, g_Info.m_CompMenuID[CBkGnuPGInfo::ExpPubKey], szMsg);
        }
        catch(...) {
            FatalErrorMessage(hWnd, __FILE__, __LINE__); //致命的エラー内容を出力
        }
        break;
    case BKC_MENU_COMPREF:
        break;
    default:
        break;
    }
    // Always return 0.
    return 0;
}

////////////////////////////////////////////////////////////////////////
// Called when a folder is opened.
int WINAPI BKC_OnOpenFolder(LPCTSTR lpFolderID)
{
    // Always return 0.
    return 0;
}

////////////////////////////////////////////////////////////////////////
// Called when a mail is selected.
int WINAPI BKC_OnOpenMail(LPCTSTR lpMailID)
{
    // Always return 0.
    return 0;
}

////////////////////////////////////////////////////////////////////////
// Called every minute.
int WINAPI BKC_OnEveryMinute()
{
    // Always return 0.
    return 0;
}

////////////////////////////////////////////////////////////////////////
// Called when a compose windows is opened.
int WINAPI BKC_OnOpenCompose(HWND hWnd, int nMode/* See COMPOSE_MODE_* in BeckyApi.h */)
{
    // Always return 0.
    return 0;
}

////////////////////////////////////////////////////////////////////////
// Called when the composing message is saved.
int WINAPI BKC_OnOutgoing(HWND hWnd, int nMode/* 0:SaveToOutbox, 1:SaveToDraft, 2:SaveToReminder*/)
{
    // Return -1 if you do not want to send it yet.
    return 0;
}

////////////////////////////////////////////////////////////////////////
// Called when a key is pressed.
int WINAPI BKC_OnKeyDispatch(HWND hWnd, int nKey/* virtual key code */, int nShift/* Shift state. 0x40=Shift, 0x20=Ctrl, 0x60=Shift+Ctrl, 0xfe=Alt*/)
{
    // Return TRUE if you want to suppress subsequent command associated to this key.
    return 0;
}

////////////////////////////////////////////////////////////////////////
// Called when a message is retrieved and saved to a folder
int WINAPI BKC_OnRetrieve(LPCTSTR lpMessage/* Message source*/, LPCTSTR lpMailID/* Mail ID*/)
{
    // Always return 0.
    return 0;
}

////////////////////////////////////////////////////////////////////////
// Called when a message is spooled
int WINAPI BKC_OnSend(LPCTSTR lpMessage/* Message source */)
{
    // Return BKC_ONSEND_PROCESSED, if you have processed this message
    // and don't need Becky! to send it.
    // Becky! will move this message to Sent box when the sending
    // operation is done.
    // CAUTION: You are responsible for the destination of this
    // message if you return BKC_ONSEND_PROCESSED.

    // Return BKC_ONSEND_ERROR, if you want to cancel the sending operation.
    // You are responsible for displaying an error message.

    // Return 0 to proceed the sending operation.
    return 0;
}

////////////////////////////////////////////////////////////////////////
// Called when all messages are retrieved
int WINAPI BKC_OnFinishRetrieve(int nNumber/* Number of messages*/)
{
    // Always return 0.
    return 0;
}


////////////////////////////////////////////////////////////////////////
// Called when plug-in setup is needed.
int WINAPI BKC_OnPlugInSetup(HWND hWnd)
{
    try {
        CSetupDialog dlg;
        if((dlg.ExecDialog(hWnd, 0, g_Info))==true) { //Setup Dialog が正常終了
            g_Info.SetEnv(); //環境変数を保存する
        }
    }
    catch(...) {
        FatalErrorMessage(hWnd, __FILE__, __LINE__); //致命的エラー内容を出力
        return 1;
    }

    // Return nonzero if you have processed.
    return 1;
}


////////////////////////////////////////////////////////////////////////
// Called when plug-in information is being retrieved.
typedef struct tagBKPLUGININFO
{
    char szPlugInName[80]; // Name of the plug-in
    char szVendor[80]; // Name of the vendor
    char szVersion[80]; // Version string
    char szDescription[256]; // Short description about this plugin
} BKPLUGININFO, *LPBKPLUGININFO;

int WINAPI BKC_OnPlugInInfo(LPBKPLUGININFO lpPlugInInfo)
{
    try {
        LoadString(g_Info.m_hInstance, IDS_PIN_NAME, lpPlugInInfo->szPlugInName, 80);
        LoadString(g_Info.m_hInstance, IDS_PIN_VENDER, lpPlugInInfo->szVendor, 80);
        LoadString(g_Info.m_hInstance, IDS_PIN_VERSION, lpPlugInInfo->szVersion, 80);
        LoadString(g_Info.m_hInstance, IDS_PIN_DESCRIPTION, lpPlugInInfo->szDescription, 256);
    }
    catch(...) {
        FatalErrorMessage(NULL, __FILE__, __LINE__); //致命的エラー内容を出力
        return 0;
    }

    // Always return 0.
    return 0;
}

////////////////////////////////////////////////////////////////////////
// Called when drag and drop operation occurs.
int WINAPI BKC_OnDragDrop(LPCSTR lpTgt, LPCSTR lpSrc, int nCount, int dropEffect)
{
    /*
    lpTgt:  A folder ID of the target folder.
            You can assume it is a root mailbox, if the string
            contains only one '\' character.
    lpSrc:  Either a folder ID or mail IDs. Multiple mail IDs are
            separated by '\n' (0x0a).
            You can assume it is a folder ID, if the string
            doesn't contain '?' character.
    nCount: Number of items to be dropped.
            It can be more than one, if you drop mail items.
    dropEffect: Type of drag and drop operation
            1: Copy
            2: Move
            4: Link (Used for filtering setup in Becky!)
    */
    // If you want to cancel the default drag and drop action,
    // return -1;
    // Do not assume the default action (copy, move, etc.) is always
    // processed, because other plug-ins might cancel the operation.
    return 0;
}

#ifdef __cplusplus
}
#endif
/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
