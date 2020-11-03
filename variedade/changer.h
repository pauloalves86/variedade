#pragma once
#include <future>

enum ChangerCommand { kStop = 0, kNext, kPrevious };

void changer(std::mutex* m, std::condition_variable* cv,
             const ChangerCommand* cmd);
