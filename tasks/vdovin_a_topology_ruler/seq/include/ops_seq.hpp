#pragma once

#include <vector>

#include "task/include/task.hpp"
#include "vdovin_a_topology_ruler/common/include/common.hpp"

namespace vdovin_a_topology_ruler {

class VdovinATopologyRulerSEQ : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kSEQ;
  }
  explicit VdovinATopologyRulerSEQ(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;

  std::vector<int> data_;
};

}  // namespace vdovin_a_topology_ruler
