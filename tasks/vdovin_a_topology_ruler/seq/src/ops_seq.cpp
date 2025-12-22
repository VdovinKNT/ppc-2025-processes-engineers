#include "vdovin_a_topology_ruler/seq/include/ops_seq.hpp"

#include <vector>

#include "vdovin_a_topology_ruler/common/include/common.hpp"

namespace vdovin_a_topology_ruler {

VdovinATopologyRulerSEQ::VdovinATopologyRulerSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
}

bool VdovinATopologyRulerSEQ::ValidationImpl() {
  return !GetInput().empty();
}

bool VdovinATopologyRulerSEQ::PreProcessingImpl() {
  data_ = GetInput();
  return true;
}

bool VdovinATopologyRulerSEQ::RunImpl() {
  GetOutput() = data_;
  return true;
}

bool VdovinATopologyRulerSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace vdovin_a_topology_ruler
