#include <desktop_wallpaper.h>

// clang-format off
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shobjidl.h>
// clang-format on

#include <iostream>

namespace fs = std::filesystem;

DesktopWallpaper::DesktopWallpaper() {
  HRESULT hr = CoInitialize(NULL);
  if (FAILED(hr)) {
    return;
  }
  hr = CoCreateInstance(__uuidof(DesktopWallpaper), NULL, CLSCTX_ALL,
                        IID_PPV_ARGS(&desktop_wallpaper_));
  if (FAILED(hr)) {
    return;
  }
}

DesktopWallpaper::~DesktopWallpaper() {
  if (desktop_wallpaper_) {
    desktop_wallpaper_->Release();
  }
  CoUninitialize();
}

bool DesktopWallpaper::SetAllMonitorsWallpaper(const fs::path &wallpaper) {
  HRESULT hr = desktop_wallpaper_->SetWallpaper(nullptr, wallpaper.c_str());
  return SUCCEEDED(hr);
}

bool DesktopWallpaper::SetNextMonitorWallpaper(const fs::path &wallpaper) {
  UINT count = 0;
  HRESULT hr = desktop_wallpaper_->GetMonitorDevicePathCount(&count);
  if (FAILED(hr)) {
    std::cout << "Count failed." << std::endl;
    return false;
  }
  UINT monitor_id = UINT(last_monitor_id_ + 1);
  if (monitor_id >= count) {
    monitor_id = 0;
  }
  last_monitor_id_ = monitor_id;
  LPWSTR monitor;
  hr = desktop_wallpaper_->GetMonitorDevicePathAt(monitor_id, &monitor);
  if (FAILED(hr)) {
    std::cout << "Monitor path failed." << std::endl;
    return false;
  }
  hr = desktop_wallpaper_->SetWallpaper(monitor, wallpaper.c_str());
  if (FAILED(hr)) {
    std::cout << hr << std::endl;
  }
  return SUCCEEDED(hr);
}
