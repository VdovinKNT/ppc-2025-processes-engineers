#include <gtest/gtest.h>

#include <cstddef>
#include <random>
#include <string>

#include "util/include/perf_test_util.hpp"
#include "vdovin_a_words_counting/common/include/common.hpp"
#include "vdovin_a_words_counting/mpi/include/ops_mpi.hpp"
#include "vdovin_a_words_counting/seq/include/ops_seq.hpp"

namespace vdovin_a_words_counting {

class VdovinAWordsCountingRunPerfTestProcesses : public ppc::util::BaseRunPerfTests<InType, OutType> {
  const int kSeed_ = 123;
  const int correct_test_ = 1000000;
  InType input_data_;

  void SetUp() override {
    std::mt19937 gen(kSeed_);
    std::uniform_int_distribution<> word_len(1, 10);
    std::uniform_int_distribution<> chars('a', 'z');

    std::string test;
    test.reserve(static_cast<std::size_t>(correct_test_) * 12U);

    for (int i = 0; i < correct_test_; ++i) {
      int t_len = word_len(gen);
      test.push_back(' ');
      for (int j = 0; j < t_len; ++j) {
        test.push_back(static_cast<char>(chars(gen)));
      }
    }
    input_data_ = test;
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return output_data == correct_test_;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(VdovinAWordsCountingRunPerfTestProcesses, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks = ppc::util::MakeAllPerfTasks<InType, VdovinAWordsCountingMPI, VdovinAWordsCountingSEQ>(
    PPC_SETTINGS_vdovin_a_words_counting);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = VdovinAWordsCountingRunPerfTestProcesses::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, VdovinAWordsCountingRunPerfTestProcesses, kGtestValues, kPerfTestName);

}  // namespace vdovin_a_words_counting
