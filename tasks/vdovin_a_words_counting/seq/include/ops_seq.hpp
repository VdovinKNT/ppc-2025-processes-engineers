#pragma once

#include "task/include/task.hpp"
#include "vdovin_a_words_counting/common/include/common.hpp"

namespace vdovin_a_words_counting {

class VdovinAWordsCountingSEQ : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kSEQ;
  }
  explicit VdovinAWordsCountingSEQ(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace vdovin_a_words_counting
