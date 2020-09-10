#include <windows.h>

#include <fstream>
#include <future>
#include <iostream>
#include <thread>

#include "changer.h"
#include "resource.h"

namespace fs = std::filesystem;

const auto TRAYICON_ID = 13;
const auto WINDOW_CLASS_NAME = L"VARIEDADEWM";
std::mutex g_m;
std::condition_variable g_cv;

void remove_tray_icon(HWND hWnd, UINT uID) {
  NOTIFYICONDATAW nid = {};
  nid.cbSize = sizeof(nid);
  nid.hWnd = hWnd;
  nid.uID = uID;
  Shell_NotifyIconW(NIM_DELETE, &nid);
}

BOOL show_popup_menu(HWND hWnd, POINT* curpos, int wDefaultItem) {
  HMENU hPop = CreatePopupMenu();
  int i = 0;
  if (!curpos) {
    POINT pt;
    GetCursorPos(&pt);
    curpos = &pt;
  }
  InsertMenuW(hPop, i++, MF_BYPOSITION | MF_STRING, IDM_NEXT, L"Next");
  InsertMenuW(hPop, i++, MF_BYPOSITION | MF_STRING, IDM_PREVIOUS, L"Previous");
  InsertMenuW(hPop, i++, MF_BYPOSITION | MF_STRING, IDM_EXIT, L"Exit");
  SetMenuDefaultItem(hPop, IDM_NEXT, FALSE);
  SetFocus(hWnd);
  SendMessageW(hWnd, WM_INITMENUPOPUP, (WPARAM)hPop, 0);
  WORD cmd = TrackPopupMenu(
      hPop, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY,
      curpos->x, curpos->y, 0, hWnd, NULL);
  SendMessageW(hWnd, WM_COMMAND, cmd, 0);
  DestroyMenu(hPop);
  return cmd;
}

void on_tray_icon_show_menu(HWND hWnd) {
  SetForegroundWindow(hWnd);
  show_popup_menu(hWnd, NULL, -1);
  PostMessage(hWnd, WM_APP + 1, 0, 0);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam,
                         LPARAM lparam) {
  switch (message) {
    case WM_COMMAND:
      switch (LOWORD(wparam)) {
        case IDM_NEXT:
          g_cv.notify_one();
          break;
        case IDM_PREVIOUS:
          break;
        case IDM_EXIT:
          remove_tray_icon(hwnd, TRAYICON_ID);
          DestroyWindow(hwnd);
          break;
        default:
          return DefWindowProc(hwnd, message, wparam, lparam);
      }
      break;
    case WM_PAINT: {
      PAINTSTRUCT ps = {};
      BeginPaint(hwnd, &ps);
      EndPaint(hwnd, &ps);
      break;
    }
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    case WM_APP + 1: {
      WORD id = LOWORD(lparam);
      if (id == WM_RBUTTONUP || id == WM_LBUTTONUP)
        on_tray_icon_show_menu(hwnd);
      break;
    }
    default:
      return DefWindowProc(hwnd, message, wparam, lparam);
  }
  return 0;
}

ATOM register_window_class(HINSTANCE instance) {
  WNDCLASSEXW wcex = {};
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = instance;
  wcex.hIcon = LoadIconW(instance, MAKEINTRESOURCEW(IDI_VARIEDADEWM));
  wcex.hCursor = LoadCursorW(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_VARIEDADEWM);
  wcex.lpszClassName = WINDOW_CLASS_NAME;
  wcex.hIconSm = LoadIconW(wcex.hInstance, MAKEINTRESOURCEW(IDI_VARIEDADEWM));
  return RegisterClassExW(&wcex);
}

void add_tray_icon(HINSTANCE instance, HWND hwnd, UINT uid, UINT callback_msg,
                   LPWSTR tooltip) {
  NOTIFYICONDATAW nid = {};
  nid.cbSize = sizeof(nid);
  nid.hWnd = hwnd;
  nid.uID = uid;
  nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
  nid.uCallbackMessage = callback_msg;
  nid.hIcon = LoadIconW(instance, MAKEINTRESOURCEW(IDI_VARIEDADEWM));
  wsprintfW(nid.szTip, tooltip);
  Shell_NotifyIconW(NIM_ADD, &nid);
}

BOOL init_instance(HINSTANCE instance) {
  HWND hwnd = CreateWindowW(WINDOW_CLASS_NAME, L"Variedade Wallpaper Changer",
                            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0,
                            CW_USEDEFAULT, 0, NULL, NULL, instance, NULL);
  SetWindowPos(hwnd, NULL, 150, 150, 200, 100, 0);
  if (!hwnd) {
    return FALSE;
  }
  add_tray_icon(instance, hwnd, TRAYICON_ID, WM_APP + 1, L"VariedadeWM");
  UpdateWindow(hwnd);
  return TRUE;
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, PWSTR cmdline,
                    int cmdshow) {
  register_window_class(instance);
  if (!init_instance(instance)) {
    return 1;
  }
  bool exit = false;
  std::thread t(changer, &g_m, &g_cv, &exit);
  MSG msg;
  HACCEL acell = LoadAcceleratorsW(instance, MAKEINTRESOURCEW(IDC_VARIEDADEWM));
  while (GetMessage(&msg, NULL, 0, 0)) {
    if (!TranslateAccelerator(msg.hwnd, acell, &msg)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
  g_m.lock();
  exit = true;
  g_m.unlock();
  t.join();
  return 0;
}
