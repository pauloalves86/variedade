#pragma once
#include <string>

struct Source {
  bool enabled;
  std::string type;
  std::string location;

  bool operator==(const Source& r) {
    return enabled == r.enabled && type == r.type && location == r.location;
  }
};
