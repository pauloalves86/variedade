#pragma once
#include <future>

void changer(std::mutex* m, std::condition_variable* cv, const bool* exit);
