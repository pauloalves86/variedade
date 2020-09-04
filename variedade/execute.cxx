#include "execute.h"

#include <windows.h>

bool execute(const std::string& prog, const std::string& args) {
  STARTUPINFOA si = {};
  PROCESS_INFORMATION pi = {};
  std::string cmdline = prog + " " + args;
  if (CreateProcessA(NULL, cmdline.data(), NULL, NULL, FALSE,
                     NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi)) {
    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD exit_code = 1;
    GetExitCodeProcess(pi.hProcess, &exit_code);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return exit_code == 0;
  }
  return false;
}
