#include <gtest/gtest.h>

#include <algorithm>
#include <cstddef>
#include <vector>

#include "util/include/perf_test_util.hpp"
#include "vdovin_a_quick_sort_merge/common/include/common.hpp"
#include "vdovin_a_quick_sort_merge/mpi/include/ops_mpi.hpp"
#include "vdovin_a_quick_sort_merge/seq/include/ops_seq.hpp"

namespace vdovin_a_quick_sort_merge {

class VdovinAQuickSortMergePerfTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
  const size_t kArraySize_ = 1000000;
  InType input_data_;

  void SetUp() override {
    input_data_.resize(kArraySize_);
    for (size_t i = 0; i < kArraySize_; i++) {
      input_data_[i] = static_cast<int>(kArraySize_ - i);
    }
  }

  bool CheckTestOutputData(OutType &output_data) final {
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
};

TEST_P(VdovinAQuickSortMergePerfTests, TaskRun) {
  ExecuteTest(GetParam());
}

TEST_P(VdovinAQuickSortMergePerfTests, TaskPipeline) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks = ppc::util::MakeAllPerfTasks<InType, VdovinAQuickSortMergeMPI, VdovinAQuickSortMergeSEQ>(
    PPC_SETTINGS_vdovin_a_quick_sort_merge);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = VdovinAQuickSortMergePerfTests::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, VdovinAQuickSortMergePerfTests, kGtestValues, kPerfTestName);

}  // namespace vdovin_a_quick_sort_merge
