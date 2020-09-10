#include "changer.h"

#include <yaml-cpp/yaml.h>

#include "apply_filter.h"
#include "desktop_wallpaper.h"
#include "endless_wallpaper.h"
#include "get_config_dir.h"

namespace fs = std::filesystem;

void changer(std::mutex* m, std::condition_variable* cv, const bool* exit) {
  auto config_file = get_config_dir() / "config.yaml";
  YAML::Node config = YAML::LoadFile(config_file.string());
  if (!config["wallpapers_dir"]) {
    return;
  }
  fs::path wallpaper_path{config["wallpapers_dir"].as<std::string>()};
  if (!fs::exists(wallpaper_path)) {
    return;
  }
  EndlessWallpaper ew(wallpaper_path);
  DesktopWallpaper dw;
  std::chrono::seconds timer(config["timer"].as<int>(600));
  auto filter = config["filter"].as<std::string>();
  while (!ew.empty()) {
    fs::path filepath(ew.next());
    if (!filter.empty()) {
      filepath = apply_filter(filter, filepath);
    }
    if (!dw.SetNextMonitorWallpaper(filepath)) {
      // std::cerr << filepath << std::endl;
    }
    std::unique_lock<std::mutex> lk(*m);
    if (cv->wait_for(lk, timer) == std::cv_status::no_timeout) {
      if (*exit) {
        return;
      }
    }
  }
}
