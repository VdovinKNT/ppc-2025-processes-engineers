#include "vdovin_a_words_counting/seq/include/ops_seq.hpp"

#include "vdovin_a_words_counting/common/include/common.hpp"

namespace vdovin_a_words_counting {

VdovinAWordsCountingSEQ::VdovinAWordsCountingSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool VdovinAWordsCountingSEQ::ValidationImpl() {
  return (!GetInput().empty()) && (GetOutput() == 0);
}

bool VdovinAWordsCountingSEQ::PreProcessingImpl() {
  return true;
}

bool VdovinAWordsCountingSEQ::RunImpl() {
  auto input = GetInput();
  if (input.empty()) {
    return false;
  }

  int counter = 0;
  bool on_word = false;
  for (char ch : input) {
    if (ch == ' ') {
      if (on_word) {
        ++counter;
        on_word = false;
      }
    } else {
      on_word = true;
    }
  }
  if (on_word) {
    ++counter;
  }
  GetOutput() = counter;
  return true;
}

bool VdovinAWordsCountingSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace vdovin_a_words_counting
