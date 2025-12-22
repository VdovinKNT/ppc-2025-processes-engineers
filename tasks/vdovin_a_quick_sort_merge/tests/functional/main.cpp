#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <random>
#include <string>
#include <tuple>
#include <vector>

#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"
#include "vdovin_a_quick_sort_merge/common/include/common.hpp"
#include "vdovin_a_quick_sort_merge/mpi/include/ops_mpi.hpp"
#include "vdovin_a_quick_sort_merge/seq/include/ops_seq.hpp"

namespace vdovin_a_quick_sort_merge {

class VdovinAQuickSortMergeFuncTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return std::to_string(std::get<0>(test_param)) + "_" + std::get<1>(test_param);
  }

 protected:
  void SetUp() override {
    TestType params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    std::string test_name = std::get<1>(params);

    if (test_name == "empty") {
      input_data_ = {};
    } else if (test_name == "single") {
      input_data_ = {73};
    } else if (test_name == "sorted") {
      input_data_ = {7, 13, 19, 23, 29, 31, 37, 41, 43, 47, 53};
    } else if (test_name == "reversed") {
      input_data_ = {97, 89, 83, 79, 73, 71, 67, 61, 59, 53, 47};
    } else if (test_name == "duplicates") {
      input_data_ = {17, 29, 17, 11, 29, 7, 17, 19, 29, 11, 23};
    } else if (test_name == "negative") {
      input_data_ = {-17, 31, -13, 0, -19, 11, 23, -7, 31, 5};
    } else if (test_name == "large") {
      input_data_.resize(127);
      for (size_t i = 0; i < 127; i++) {
        input_data_[i] = static_cast<int>(126 - i);
      }
    } else if (test_name == "generated") {
      const size_t k_size = 50000;
      input_data_.resize(k_size);
      // NOLINTNEXTLINE(cert-msc51-cpp)
      std::mt19937 gen(42);
      std::uniform_int_distribution<int> dist(-50000, 50000);
      for (size_t i = 0; i < k_size; i++) {
        input_data_[i] = dist(gen);
      }
    } else if (test_name == "mixed") {
      input_data_ = {91, 17, 83, 29, 5, 67, 41, 13, 97, 3, 59, 31, 7, 73, 19};
    } else if (test_name == "small") {
      input_data_ = {37, 11};
    } else if (test_name == "medium") {
      input_data_ = {47, 23, 71, 3, 59, 13, 89, 31, 7, 61, 19, 43, 79, 11, 53};
    } else if (test_name == "powers") {
      input_data_ = {64, 32, 16, 8, 4, 2, 1, 128, 256, 512};
    } else {
      input_data_ = {83, 17, 59, 7, 41, 97, 23, 71, 13, 89};
    }
  }

  bool CheckTestOutputData(OutType &output_data) final {
    if (input_data_.empty()) {
      return output_data.empty();
    }
    if (output_data.size() != input_data_.size()) {
      return false;
    }
    std::vector<int> expected = input_data_;
    std::ranges::sort(expected);
    return output_data == expected && std::ranges::is_sorted(output_data);
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_;
};

namespace {

TEST_P(VdovinAQuickSortMergeFuncTests, QuickSortMergeTest) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 14> kTestParam = {
    std::make_tuple(1, "empty"),    std::make_tuple(2, "single"),     std::make_tuple(3, "sorted"),
    std::make_tuple(4, "reversed"), std::make_tuple(5, "duplicates"), std::make_tuple(6, "negative"),
    std::make_tuple(7, "large"),    std::make_tuple(8, "generated"),  std::make_tuple(9, "mixed"),
    std::make_tuple(10, "small"),   std::make_tuple(11, "medium"),    std::make_tuple(12, "powers"),
    std::make_tuple(13, "basic"),   std::make_tuple(14, "basic")};

const auto kTestTasksList = std::tuple_cat(
    ppc::util::AddFuncTask<VdovinAQuickSortMergeMPI, InType>(kTestParam, PPC_SETTINGS_vdovin_a_quick_sort_merge),
    ppc::util::AddFuncTask<VdovinAQuickSortMergeSEQ, InType>(kTestParam, PPC_SETTINGS_vdovin_a_quick_sort_merge));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName = VdovinAQuickSortMergeFuncTests::PrintFuncTestName<VdovinAQuickSortMergeFuncTests>;

INSTANTIATE_TEST_SUITE_P(QuickSortMergeTests, VdovinAQuickSortMergeFuncTests, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace vdovin_a_quick_sort_merge
