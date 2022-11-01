#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <stdio.h>
#include "resource.h"

#include <string>
#include <fstream>

HINSTANCE hInst;

enum EmuType
{
    FCEUX,
    Mesen,
    Bizhawk,
    Generic
};

struct NesSetup
{
    EmuType m_Type = FCEUX;
    std::string m_EmuPath;
    std::string m_RomPath;
    std::string m_EasyConnectInfo;
    std::string m_CommandLine;

    void Save()
    {
        CreateFolder("");
        WriteToFile(GetSetupFileName());
    }
    void Load()
    {
        ReadFromFile(GetSetupFileName());
    }

    void CreateFolder(const std::string& data)
    {
        std::string folder = GetDocumentFolder();
        CreateDirectory(folder.c_str(), NULL);

        if (data != "")
        {
            folder.append(data);
            CreateDirectory(folder.c_str(), NULL);
        }
    }

    void WriteToFile(const std::string& data)
    {
        std::ofstream out(data.c_str());
        out << "EmuType=" << (int)m_Type << std::endl;
        out << "EmuPath=" << m_EmuPath << std::endl;
        out << "RomPath=" << m_RomPath << std::endl;
        out << "EasyConnectInfo=" << m_EasyConnectInfo << std::endl;
    }
    void ReadFromFile(const std::string& data)
    {
        std::ifstream in(data.c_str());

        std::string line;
        while (std::getline(in, line))
        {
            auto index = line.find_first_of('=');
            if (index != std::string::npos)
            {
                std::string first = line.substr(0, index);
                std::string second = line.substr(index + 1);
                if (first == "EmuType")
                {
                    m_Type = (EmuType)atoi(second.c_str());
                }
                else if (first == "EmuPath")
                {
                    m_EmuPath = second;
                }
                else if (first == "RomPath")
                {
                    m_RomPath = second;
                }
                else if (first == "EasyConnectInfo")
                {
                    m_EasyConnectInfo = second;
                }
            }
        }

        if (m_CommandLine != "")
        {
            m_EasyConnectInfo = m_CommandLine;
        }
    }

    std::string GetEmuFolder()
    {
        auto lastSlash = m_EmuPath.find_last_of("/\\");
        if (lastSlash == std::string::npos)
        {
            return "";
        }
        return m_EmuPath.substr(0, lastSlash);
    }

    std::string GetDocumentFolder()
    {
        char my_documents[MAX_PATH];
        HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);

        std::string ret = ".";

        if (result == S_OK)
        {
            ret = my_documents;
        }
        ret.append("/NesTrisLauncher");

        return ret;
    }
    std::string GetSetupFileName()
    {
        std::string ret = GetDocumentFolder();
        ret.append("/config.ini");

        return ret;
    }

    void WriteAllLuaScript()
    {
        // this needs to happen in the right order, because it only adds one level per time
        CreateFolder("/lua");
        CreateFolder("/lua/nestrischamps");
        CreateFolder("/lua/nestrischamps/lib");
        CreateFolder("/lua/nestrischamps/emus");

        WriteLuaScript(IDR_Nestrischamps,   "/lua",                     "nestrischamps.lua");
        WriteLuaScript(IDR_FrameManager,    "/lua/nestrischamps",       "frameManager.lua");
        WriteLuaScript(IDR_Getters,         "/lua/nestrischamps",       "getters.lua");
        WriteLuaScript(IDR_Playfield,       "/lua/nestrischamps",       "playfield.lua");
        WriteLuaScript(IDR_Util,            "/lua/nestrischamps",       "util.lua");
        WriteLuaScript(IDR_Variables,       "/lua/nestrischamps",       "variables.lua");
        WriteLuaScript(IDR_Lib_Bit,         "/lua/nestrischamps/lib",   "bit.lua");
        WriteLuaScript(IDR_Lib_Websocket,   "/lua/nestrischamps/lib",   "websocket.lua");
        WriteLuaScript(IDR_Emus_Bizhawk,    "/lua/nestrischamps/emus",  "bizhawk.lua");
        WriteLuaScript(IDR_Emus_Fceux,      "/lua/nestrischamps/emus",  "fceux.lua");
        WriteLuaScript(IDR_Emus_Mesen,      "/lua/nestrischamps/emus",  "mesen.lua");

    }

    void WriteLuaScript(UINT res, const std::string& folder, const std::string& file)
    {
        std::string FullFileName = GetDocumentFolder();
        FullFileName.append(folder);
        FullFileName.append("/");
        FullFileName.append(file);

        HRSRC hRes = FindResource(NULL,
                            MAKEINTRESOURCE(res),
                            "DATA");

        HGLOBAL hResourceLoaded = LoadResource(NULL, hRes);
        char* lpResLock = (char *)LockResource(hResourceLoaded);
        DWORD dwSizeRes = SizeofResource(NULL, hRes);

        std::ofstream out;
        out.open(FullFileName.c_str(), std::ios::out|std::ios::binary|std::ios::trunc);
        out.write((const char *) lpResLock, dwSizeRes);
    }

    void StartEmulator()
    {
        switch (m_Type)
        {
        case FCEUX:
            StartFCEUX();
            break;
        case Mesen:
            StartMesen();
            break;
        case Bizhawk:
            StartBizhawk();
            break;
        case Generic:
            StartGeneric();
            break;
        }
    }

    void StartFCEUX()
    {
        std::string luaScript = GetDocumentFolder();
        luaScript.append("/lua/nestrischamps.lua");
        std::string luaConfig = GetDocumentFolder();
        luaConfig.append("/lua/easyconnect.lua");

        {
            std::ofstream out;
            out.open(luaConfig.c_str(), std::ios::out|std::ios::trunc);
            out << "DEFAULTEASYCONNECT= \"" << m_EasyConnectInfo << "\"" << std::endl;
        }

        std::string param;

        param.append("-lua \"");
        param.append(luaScript);
        param.append("\" \"");
        param.append(m_RomPath);
        param.append("\"");

        ShellExecute(
            NULL,
            NULL,
            m_EmuPath.c_str(),
            param.c_str(),
            GetEmuFolder().c_str(),
            SW_SHOW);
    }

    void StartMesen()
    {
        std::string luaFolder = GetDocumentFolder();
        luaFolder.append("/lua");
        std::string luaScript = luaFolder;
        luaScript.append("/nestrischamps.lua");
        std::string luaConfig = luaFolder;
        luaConfig.append("/easyconnect.lua");
        std::string mesenExecutableCopy = GetDocumentFolder();
        mesenExecutableCopy.append("/mesen.exe");

        {
           std::ifstream src(m_EmuPath.c_str(), std::ios::binary);
           std::ofstream dest(mesenExecutableCopy.c_str(), std::ios::binary);
           dest << src.rdbuf();
        }
        {
            std::ofstream out;
            out.open(luaConfig.c_str(), std::ios::out|std::ios::trunc);
            out << "DEFAULTEASYCONNECT= \"" << m_EasyConnectInfo << "\"" << std::endl;
        }

        std::string param;

        param.append("\"");
        param.append(m_RomPath);
        param.append("\" \"");
        param.append(luaScript);
        param.append("\"");

        ShellExecute(
            NULL,
            NULL,
            mesenExecutableCopy.c_str(),
            param.c_str(),
            GetDocumentFolder().c_str(),
            SW_SHOW);
    }

    void StartBizhawk()
    {
        std::string luaFolder = GetDocumentFolder();
        luaFolder.append("/lua");
        std::string luaScript = luaFolder;
        luaScript.append("/nestrischamps.lua");
        std::string luaConfig = luaFolder;
        luaConfig.append("/easyconnect.lua");

        {
            std::ofstream out;
            out.open(luaConfig.c_str(), std::ios::out|std::ios::trunc);
            out << "DEFAULTEASYCONNECT= \"" << m_EasyConnectInfo << "\"" << std::endl;
        }

        std::string param;

        param.append("\"");
        param.append(m_RomPath);
        param.append("\" --lua=\"");
        param.append(luaScript);
        param.append("\"");

        ShellExecute(
            NULL,
            NULL,
            m_EmuPath.c_str(),
            param.c_str(),
            GetEmuFolder().c_str(),
            SW_SHOW);
    }

    void StartGeneric()
    {
        std::string param;

        param.append("\"");
        param.append(m_RomPath);
        param.append("\" \"");
        param.append(m_EasyConnectInfo);
        param.append("\" \"");
        param.append(GetDocumentFolder());
        param.append("\"");

        ShellExecute(
            NULL,
            NULL,
            m_EmuPath.c_str(),
            param.c_str(),
            GetEmuFolder().c_str(),
            SW_SHOW);
    }
};

NesSetup g_setupData;

void BrowseFile(
        HWND hwndOwner,
        UINT dlgItem,
        const char* title,
        const char* filter)
{
    OPENFILENAME ofn = {0};       // common dialog box structure

    char textBuf[2048]= {0};
    GetDlgItemText(hwndOwner, dlgItem, (LPSTR)&textBuf, 2048);

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hInstance = NULL;
    ofn.hwndOwner = hwndOwner;
    ofn.lpstrFile = textBuf;
    ofn.nMaxFile = 2047;
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrTitle = title;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_NOVALIDATE;

    if (GetOpenFileName(&ofn) == TRUE)
    {
        SetDlgItemText(hwndOwner, dlgItem, textBuf);
    }
}

BOOL CALLBACK DlgMain(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_INITDIALOG:
    {
        g_setupData.Load();
        SendDlgItemMessage(hwndDlg, EMUCOMBO, CB_ADDSTRING, 0, (LPARAM)"FCEUX");
        SendDlgItemMessage(hwndDlg, EMUCOMBO, CB_ADDSTRING, 0, (LPARAM)"Mesen");
        SendDlgItemMessage(hwndDlg, EMUCOMBO, CB_ADDSTRING, 0, (LPARAM)"Bizhawk");
        SendDlgItemMessage(hwndDlg, EMUCOMBO, CB_ADDSTRING, 0, (LPARAM)"Generic");
        SendDlgItemMessage(hwndDlg, EMUCOMBO, CB_SETCURSEL, g_setupData.m_Type, 0);
        SendDlgItemMessage(hwndDlg, EMUPATH, EM_SETLIMITTEXT, 2047, 0);
        SetDlgItemText(hwndDlg, EMUPATH, g_setupData.m_EmuPath.substr(0, 2046).c_str());
        SendDlgItemMessage(hwndDlg, ROMPATH, EM_SETLIMITTEXT, 2047, 0);
        SetDlgItemText(hwndDlg, ROMPATH, g_setupData.m_RomPath.substr(0, 2046).c_str());
        SendDlgItemMessage(hwndDlg, CONNECT, EM_SETLIMITTEXT, 2047, 0);
        SetDlgItemText(hwndDlg, CONNECT, g_setupData.m_EasyConnectInfo.substr(0, 2046).c_str());
    }
    return TRUE;

    case WM_CLOSE:
    {
        EndDialog(hwndDlg, 0);
    }
    return TRUE;

    case WM_COMMAND:
    {
        switch(LOWORD(wParam))
        {
        case STARTBUT:
        case SAVEBUT:
            {
                g_setupData.m_Type = (EmuType)SendDlgItemMessage(hwndDlg, EMUCOMBO, CB_GETCURSEL, 0, 0);

                char textBuf[2048];

                GetDlgItemText(hwndDlg, EMUPATH, (LPSTR)&textBuf, 2048);
                g_setupData.m_EmuPath = textBuf;

                GetDlgItemText(hwndDlg, ROMPATH, (LPSTR)&textBuf, 2048);
                g_setupData.m_RomPath = textBuf;

                GetDlgItemText(hwndDlg, CONNECT, (LPSTR)&textBuf, 2048);
                g_setupData.m_EasyConnectInfo = textBuf;

                g_setupData.Save();
                g_setupData.WriteAllLuaScript();
                if (LOWORD(wParam) == STARTBUT)
                {
                    g_setupData.StartEmulator();
                }
            }

            EndDialog(hwndDlg, 0);
            break;
        case EMUBUT:
            BrowseFile(
                hwndDlg,
                EMUPATH,
                "Select emulator executable",
                "Executable (*.exe)\0*.exe\0Batch File (*.bat)\0*.bat\0All Files (*.*)\0*.*\0");
            break;
        case ROMBUT:
            BrowseFile(
                hwndDlg,
                ROMPATH,
                "Select Nes Tetris ROM",
                "Nes ROM File (*.exe)\0*.nes\0All Files (*.*)\0*.*\0");
            break;
        }
    }
    return TRUE;
    }
    return FALSE;
}


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    g_setupData.m_CommandLine = lpCmdLine;
    hInst=hInstance;
    InitCommonControls();
    return DialogBox(hInst, MAKEINTRESOURCE(DLG_MAIN), NULL, (DLGPROC)DlgMain);
}
