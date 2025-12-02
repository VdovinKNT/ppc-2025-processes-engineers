#pragma once

#include "task/include/task.hpp"
#include "vdovin_a_words_counting/common/include/common.hpp"

namespace vdovin_a_words_counting {

class VdovinAWordsCountingMPI : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }
  explicit VdovinAWordsCountingMPI(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace vdovin_a_words_counting
