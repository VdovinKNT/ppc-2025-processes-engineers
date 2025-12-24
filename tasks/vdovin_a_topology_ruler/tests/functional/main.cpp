#include <gtest/gtest.h>

#include <array>
#include <cstddef>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"
#include "vdovin_a_topology_ruler/common/include/common.hpp"
#include "vdovin_a_topology_ruler/mpi/include/ops_mpi.hpp"
#include "vdovin_a_topology_ruler/seq/include/ops_seq.hpp"

namespace vdovin_a_topology_ruler {

class VdovinATopologyRulerFuncTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return std::to_string(std::get<0>(test_param)) + "_" + std::get<1>(test_param);
  }

 protected:
  void SetUp() override {
    TestType params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    int size = std::get<0>(params);
    input_data_.resize(size);
    for (int i = 0; i < size; ++i) {
      input_data_[i] = i + 1;
    }
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return input_data_ == output_data;
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_;
};

namespace {

TEST_P(VdovinATopologyRulerFuncTests, TopologyRulerTest) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 3> kTestParam = {std::make_tuple(10, "size_10"), std::make_tuple(100, "size_100"),
                                            std::make_tuple(1000, "size_1000")};

const auto kTestTasksList = std::tuple_cat(
    ppc::util::AddFuncTask<VdovinATopologyRulerMPI, InType>(kTestParam, PPC_SETTINGS_vdovin_a_topology_ruler),
    ppc::util::AddFuncTask<VdovinATopologyRulerSEQ, InType>(kTestParam, PPC_SETTINGS_vdovin_a_topology_ruler));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName = VdovinATopologyRulerFuncTests::PrintFuncTestName<VdovinATopologyRulerFuncTests>;

INSTANTIATE_TEST_SUITE_P(BasicTests, VdovinATopologyRulerFuncTests, kGtestValues, kPerfTestName);

}  // namespace

class VdovinATopologyRulerEdgeCaseTests : public ::testing::Test {
 protected:
  static void RunMPITest(const std::vector<int> &input, const std::vector<int> &expected) {
    auto task = std::make_shared<VdovinATopologyRulerMPI>(input);
    ASSERT_TRUE(task->Validation());
    ASSERT_TRUE(task->PreProcessing());
    ASSERT_TRUE(task->Run());
    ASSERT_TRUE(task->PostProcessing());
    EXPECT_EQ(task->GetOutput(), expected);
  }

  static void RunSEQTest(const std::vector<int> &input, const std::vector<int> &expected) {
    auto task = std::make_shared<VdovinATopologyRulerSEQ>(input);
    ASSERT_TRUE(task->Validation());
    ASSERT_TRUE(task->PreProcessing());
    ASSERT_TRUE(task->Run());
    ASSERT_TRUE(task->PostProcessing());
    EXPECT_EQ(task->GetOutput(), expected);
  }
};

TEST_F(VdovinATopologyRulerEdgeCaseTests, SingleElementMPI) {
  if (!ppc::util::IsUnderMpirun()) {
    GTEST_SKIP();
  }
  std::vector<int> input = {42};
  RunMPITest(input, input);
}

TEST_F(VdovinATopologyRulerEdgeCaseTests, SingleElementSEQ) {
  std::vector<int> input = {42};
  RunSEQTest(input, input);
}

TEST_F(VdovinATopologyRulerEdgeCaseTests, NegativeValuesMPI) {
  if (!ppc::util::IsUnderMpirun()) {
    GTEST_SKIP();
  }
  std::vector<int> input = {-5, -3, -1, 0, 1, 3, 5};
  RunMPITest(input, input);
}

TEST_F(VdovinATopologyRulerEdgeCaseTests, NegativeValuesSEQ) {
  std::vector<int> input = {-5, -3, -1, 0, 1, 3, 5};
  RunSEQTest(input, input);
}

TEST_F(VdovinATopologyRulerEdgeCaseTests, LargeDataMPI) {
  if (!ppc::util::IsUnderMpirun()) {
    GTEST_SKIP();
  }
  std::vector<int> input(5000);
  for (int i = 0; i < 5000; ++i) {
    input[i] = i;
  }
  RunMPITest(input, input);
}

TEST_F(VdovinATopologyRulerEdgeCaseTests, LargeDataSEQ) {
  std::vector<int> input(5000);
  for (int i = 0; i < 5000; ++i) {
    input[i] = i;
  }
  RunSEQTest(input, input);
}

TEST_F(VdovinATopologyRulerEdgeCaseTests, AllSameValuesMPI) {
  if (!ppc::util::IsUnderMpirun()) {
    GTEST_SKIP();
  }
  std::vector<int> input(100, 7);
  RunMPITest(input, input);
}

TEST_F(VdovinATopologyRulerEdgeCaseTests, AllSameValuesSEQ) {
  std::vector<int> input(100, 7);
  RunSEQTest(input, input);
}

TEST_F(VdovinATopologyRulerEdgeCaseTests, TwoElementsMPI) {
  if (!ppc::util::IsUnderMpirun()) {
    GTEST_SKIP();
  }
  std::vector<int> input = {1, 2};
  RunMPITest(input, input);
}

TEST_F(VdovinATopologyRulerEdgeCaseTests, TwoElementsSEQ) {
  std::vector<int> input = {1, 2};
  RunSEQTest(input, input);
}

TEST_F(VdovinATopologyRulerEdgeCaseTests, ValidationFailsOnEmptyInputMPI) {
  if (!ppc::util::IsUnderMpirun()) {
    GTEST_SKIP();
  }
  std::vector<int> empty_input;
  auto task = std::make_shared<VdovinATopologyRulerMPI>(empty_input);
  EXPECT_FALSE(task->Validation());
}

TEST_F(VdovinATopologyRulerEdgeCaseTests, ValidationFailsOnEmptyInputSEQ) {
  std::vector<int> empty_input;
  auto task = std::make_shared<VdovinATopologyRulerSEQ>(empty_input);
  EXPECT_FALSE(task->Validation());
}

TEST_F(VdovinATopologyRulerEdgeCaseTests, DescendingOrderMPI) {
  if (!ppc::util::IsUnderMpirun()) {
    GTEST_SKIP();
  }
  std::vector<int> input = {100, 90, 80, 70, 60, 50, 40, 30, 20, 10};
  RunMPITest(input, input);
}

TEST_F(VdovinATopologyRulerEdgeCaseTests, DescendingOrderSEQ) {
  std::vector<int> input = {100, 90, 80, 70, 60, 50, 40, 30, 20, 10};
  RunSEQTest(input, input);
}

TEST_F(VdovinATopologyRulerEdgeCaseTests, AlternatingValuesMPI) {
  if (!ppc::util::IsUnderMpirun()) {
    GTEST_SKIP();
  }
  std::vector<int> input = {1, -1, 2, -2, 3, -3, 4, -4, 5, -5};
  RunMPITest(input, input);
}

TEST_F(VdovinATopologyRulerEdgeCaseTests, AlternatingValuesSEQ) {
  std::vector<int> input = {1, -1, 2, -2, 3, -3, 4, -4, 5, -5};
  RunSEQTest(input, input);
}

}  // namespace vdovin_a_topology_ruler
