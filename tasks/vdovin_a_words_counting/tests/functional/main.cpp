#include <gtest/gtest.h>
#include <stb/stb_image.h>

#include <array>
#include <cstddef>
#include <random>
#include <string>
#include <tuple>

#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"
#include "vdovin_a_words_counting/common/include/common.hpp"
#include "vdovin_a_words_counting/mpi/include/ops_mpi.hpp"
#include "vdovin_a_words_counting/seq/include/ops_seq.hpp"

namespace vdovin_a_words_counting {

class VdovinAWordsCountingRunFuncTestsProcesses : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return std::get<0>(test_param);
  }

 protected:
  void SetUp() override {
    TestType params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    std::mt19937 gen(std::get<1>(params));
    std::uniform_int_distribution<> word_len(1, 10);
    std::uniform_int_distribution<> chars('a', 'z');

    correct_test_ = std::get<2>(params);
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
    return (output_data == correct_test_);
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_;
  int correct_test_ = 0;
};

namespace {

TEST_P(VdovinAWordsCountingRunFuncTestsProcesses, WordsCountingTests) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 3> kTestParam = {std::make_tuple("Gen_1_word_seed_123", 123, 1),
                                            std::make_tuple("Gen_7_word_seed_123", 123, 7),
                                            std::make_tuple("Gen_1000_word_seed_123", 123, 1000)};

const auto kTestTasksList = std::tuple_cat(
    ppc::util::AddFuncTask<VdovinAWordsCountingMPI, InType>(kTestParam, PPC_SETTINGS_vdovin_a_words_counting),
    ppc::util::AddFuncTask<VdovinAWordsCountingSEQ, InType>(kTestParam, PPC_SETTINGS_vdovin_a_words_counting));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName =
    VdovinAWordsCountingRunFuncTestsProcesses::PrintFuncTestName<VdovinAWordsCountingRunFuncTestsProcesses>;

INSTANTIATE_TEST_SUITE_P(WordsCountingTests, VdovinAWordsCountingRunFuncTestsProcesses, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace vdovin_a_words_counting
