#include <windows.h>
#include <string.h>

#define UNLOCK_CODE "unlock"

volatile int unlocked = 0;

// Callback for the dialog box
INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static char input[64] = {0};
    switch (uMsg) {
        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK) {
                GetDlgItemTextA(hwndDlg, 1001, input, sizeof(input));
                if (strcmp(input, UNLOCK_CODE) == 0) {
                    unlocked = 1;
                    EndDialog(hwndDlg, 0);
                } else {
                    SetDlgItemTextA(hwndDlg, 1001, "");
                }
            }
            break;
        case WM_CLOSE:
            EndDialog(hwndDlg, 0);
            break;
    }
    return 0;
}

void ShowPopup() {
    // Register a simple dialog template in memory
    DLGTEMPLATE dlg = {0};
    dlg.style = DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU;
    dlg.cdit = 2;
    dlg.x = 10; dlg.y = 10; dlg.cx = 200; dlg.cy = 80;

    // Allocate memory for dialog template and controls
    BYTE dlgTemplate[1024] = {0};
    memcpy(dlgTemplate, &dlg, sizeof(dlg));

    // Create dialog with an edit box and OK button
    HWND hwnd = CreateDialogParamA(
        GetModuleHandle(NULL),
        (LPCSTR)dlgTemplate,
        NULL,
        DialogProc,
        0
    );
    if (hwnd) {
        CreateWindowExA(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_PASSWORD,
                        20, 20, 160, 20, hwnd, (HMENU)1001, NULL, NULL);
        CreateWindowExA(0, "BUTTON", "OK", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                        70, 50, 60, 24, hwnd, (HMENU)IDOK, NULL, NULL);
        ShowWindow(hwnd, SW_SHOW);
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0) > 0 && !unlocked) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    while (!unlocked) {
        ShowPopup();
    }
    return 0;
}