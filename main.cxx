#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <thread>
#include <vector>
// clang-format off
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
// clang-format on
#include "desktop_wallpaper.h"
#include "apply_filter.h"
#include "get_config_dir.h"

namespace fs = std::filesystem;
namespace po = boost::program_options;

int main(int argc, char** argv) {
  po::options_description desc("Allowed options");
  desc.add_options()  // clang-format off
    ("help", "produce help message")
    ("wallpapers_dir", po::value<std::string>(), "wallpapers directory")
    ("timer", po::value<int>()->default_value(60),
     "change wallpaper every N seconds")
  ;  // clang-format on

  po::options_description desc_file("Config only options");
  desc_file.add_options()  // clang-format off
    ("filter", po::value<std::string>(), "imagemagick filter")
  ;  // clang-format on
  po::positional_options_description p;
  p.add("wallpapers_dir", -1);

  auto config_file = get_config_dir() / "config.ini";

  po::variables_map vm;
  po::store(
      po::command_line_parser(argc, argv).options(desc).positional(p).run(),
      vm);
  if (fs::exists(config_file)) {
    std::fstream buffer(config_file);
    po::parsed_options parsed =
        po::parse_config_file(buffer, desc.add(desc_file));
    po::store(parsed, vm);
  }
  po::notify(vm);

  if (vm.count("help")) {
    std::cerr << desc << std::endl;
    return 0;
  }

  if (vm.count("wallpapers_dir") == 0) {
    std::cerr << "wallpapers directory is required" << std::endl;
    std::cerr << desc << std::endl;
    return 1;
  }

  fs::path wallpaper_path{vm["wallpapers_dir"].as<std::string>()};
  if (!fs::exists(wallpaper_path)) {
    std::cerr << "wallpapers directory must exists" << std::endl;
    std::cerr << wallpaper_path << std::endl;
    return 1;
  }
  if (!fs::is_directory(wallpaper_path)) {
    std::cerr << "wallpapers must be a directory" << std::endl;
    std::cerr << wallpaper_path << std::endl;
    return 1;
  }

  bool use_filter = vm.count("filter") > 0;
  std::random_device rd;
  std::mt19937 g(rd());
  std::chrono::seconds duration(vm["timer"].as<int>());
  DesktopWallpaper dw;
  while (true) {
    auto it = fs::directory_iterator(wallpaper_path);
    std::vector<fs::directory_entry> files(fs::begin(it), fs::end(it));
    std::shuffle(files.begin(), files.end(), g);
    for (auto& file : files) {
      if (!file.is_regular_file()) {
        continue;
      }
      fs::path filepath(file.path());
      if (use_filter) {
        filepath = apply_filter(vm["filter"].as<std::string>(), file.path());
      }
      if (!dw.SetNextMonitorWallpaper(filepath)) {
        std::cerr << filepath << std::endl;
      }
      std::this_thread::sleep_for(duration);
    }
  }
  return 0;
}
