#pragma once

#include <string>
#include <tuple>

#include "task/include/task.hpp"

namespace vdovin_a_words_counting {

using InType = std::string;
using OutType = int;
using TestType = std::tuple<std::string, int, int>;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace vdovin_a_words_counting
