#include <desktop_wallpaper.h>

#include <iostream>
#include <chrono>
#include <thread>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

namespace fs = std::filesystem;
namespace po = boost::program_options;

int main(int argc, char **argv) {
  po::options_description desc("Allowed options");
  desc.add_options()
      ("help", "produce help message")
      ("wallpaper", po::value<std::string>(), "wallpaper file or directory")
  ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
      std::cout << desc << std::endl;
      return 0;
  }

  if (vm.count("wallpaper") == 0) {
      std::cout << desc << std::endl;
      return 1;
  }

  fs::path wallpaper_path{vm["wallpaper"].as<std::string>()};
  if (!fs::exists(wallpaper_path)) {
    std::cout << "Wallpaper must exists." << std::endl;
    std::cout << wallpaper_path << std::endl;
    return 1;
  }

  DesktopWallpaper dw;
  if (fs::is_directory(wallpaper_path)) {
    using namespace std::chrono_literals;
    while (true) {
      for(auto& p: fs::directory_iterator(wallpaper_path)) {
        if (dw.SetNextMonitorWallpaper(p.path())) {
          std::this_thread::sleep_for(2s);
        }
      }
    }
    return 0;
  }

  if (dw.SetAllMonitorsWallpaper(wallpaper_path)) {
    std::cout << "Wallpaper changed successfully.";
    return 0;
  }
  std::cout << "Wallpaper change failed.";
  return 1;
}
