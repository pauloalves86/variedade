#include "variedade/get_config_dir.h"

#include <Shlobj.h>

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

std::string last_error() {
  LPSTR msg_buff;
  DWORD dw = GetLastError();
  FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                     FORMAT_MESSAGE_IGNORE_INSERTS,
                 NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                 (LPSTR)&msg_buff, 0, NULL);
  std::string message(msg_buff);
  LocalFree(msg_buff);
  return message;
}

fs::path get_config_dir() {
  PWSTR path = NULL;
  HRESULT hr = SHGetKnownFolderPath(FOLDERID_Profile, 0, NULL, &path);
  if (SUCCEEDED(hr)) {
    fs::path config_dir{path};
    CoTaskMemFree(path);
    config_dir /= ".variedade";
    return config_dir;
  }
  std::cerr << last_error() << std::endl;
  return fs::path();
}
