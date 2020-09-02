// https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/nn-shobjidl_core-idesktopwallpaper
#pragma once
#include <string>
#include <filesystem>

struct IDesktopWallpaper;

class DesktopWallpaper {
  IDesktopWallpaper* desktop_wallpaper_ = nullptr;
  int last_monitor_id_ = -1;

 public:
  DesktopWallpaper();
  ~DesktopWallpaper();

  bool SetAllMonitorsWallpaper(const std::filesystem::path& wallpaper);
  bool SetNextMonitorWallpaper(const std::filesystem::path& wallpaper);
};
