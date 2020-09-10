#pragma once

#include <filesystem>
#include <random>
#include <vector>

class EndlessWallpaper {
 public:
  EndlessWallpaper(const std::filesystem::path& wallpaper_path);

  bool empty() const;
  void init_iterator();
  std::filesystem::path next();

 private:
  std::mt19937 g_;
  std::filesystem::path path_;
  std::vector<std::filesystem::directory_entry> files_;
  std::vector<std::filesystem::directory_entry>::iterator it_;
};
