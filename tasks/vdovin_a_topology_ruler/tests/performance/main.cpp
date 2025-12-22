#include <gtest/gtest.h>

#include "util/include/perf_test_util.hpp"
#include "vdovin_a_topology_ruler/common/include/common.hpp"
#include "vdovin_a_topology_ruler/mpi/include/ops_mpi.hpp"
#include "vdovin_a_topology_ruler/seq/include/ops_seq.hpp"

namespace vdovin_a_topology_ruler {

class VdovinATopologyRulerPerfTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
  static constexpr int kDataSize = 60000000;
  InType input_data_;

  void SetUp() override {
    input_data_.resize(kDataSize);
    for (int i = 0; i < kDataSize; ++i) {
      input_data_[i] = i;
    }
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return input_data_ == output_data;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(VdovinATopologyRulerPerfTests, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks = ppc::util::MakeAllPerfTasks<InType, VdovinATopologyRulerMPI, VdovinATopologyRulerSEQ>(
    PPC_SETTINGS_vdovin_a_topology_ruler);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = VdovinATopologyRulerPerfTests::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, VdovinATopologyRulerPerfTests, kGtestValues, kPerfTestName);

}  // namespace vdovin_a_topology_ruler
