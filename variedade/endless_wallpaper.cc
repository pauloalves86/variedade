#include "endless_wallpaper.h"

namespace fs = std::filesystem;

EndlessWallpaper::EndlessWallpaper(const fs::path& wallpaper_path)
    : path_(wallpaper_path) {
  std::random_device rd;
  g_.seed(rd());
  init_iterator();
}

bool EndlessWallpaper::empty() const { return files_.empty(); }

void EndlessWallpaper::init_iterator() {
  auto it = fs::directory_iterator(path_);
  for (auto dir_it = fs::begin(it); dir_it != fs::end(it); ++dir_it) {
    if (dir_it->is_regular_file()) {
      files_.push_back(*dir_it);
    }
  }
  std::shuffle(files_.begin(), files_.end(), g_);
  it_ = files_.begin();
}

fs::path EndlessWallpaper::next() {
  auto dir_it = *it_++;
  if (it_ == files_.end()) {
    init_iterator();
  }
  return dir_it.path();
}
