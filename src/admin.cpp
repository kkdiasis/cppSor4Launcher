#include "admin.h"
#include <windows.h>
#include <shellapi.h>

static int esta_como_admin(void)
{
    BOOL admin = FALSE;
    HANDLE token = NULL;
    if(OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token))
    {
        TOKEN_ELEVATION elev;
        DWORD tam = sizeof(elev);
        if(GetTokenInformation(token, TokenElevation, &elev, tam, &tam))
            admin = elev.TokenIsElevated;
        CloseHandle(token);
    }
    return admin;
}

void garantir_admin(void)
{
    if(esta_como_admin()) return;  // ja e admin, nao faz nada

    // pega o caminho do proprio executavel
    char caminho[MAX_PATH];
    GetModuleFileNameA(NULL, caminho, MAX_PATH);

    // relanca pedindo elevacao
    SHELLEXECUTEINFOA info = {0};
    info.cbSize = sizeof(info);
    info.lpVerb = "runas";        // solicita UAC
    info.lpFile = caminho;
    info.nShow  = SW_SHOWNORMAL;
    ShellExecuteExA(&info);

    ExitProcess(0);  // fecha a instancia atual
}