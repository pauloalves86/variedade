#include "apply_filter.h"

#include "execute.h"

namespace fs = std::filesystem;

fs::path apply_filter(const std::string& filter, const fs::path& filepath) {
  auto tmp = fs::temp_directory_path() / filepath.filename();
  auto args = "convert \"" + filepath.string() + "\" ";
  args += filter  + " \"" + tmp.string() + "\"";
  if (execute("magick", args)) {
    return tmp;
  }
  return filepath;
}
