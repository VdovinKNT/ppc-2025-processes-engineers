#include "vdovin_a_topology_ruler/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <vector>

#include "vdovin_a_topology_ruler/common/include/common.hpp"

namespace vdovin_a_topology_ruler {

VdovinATopologyRulerMPI::VdovinATopologyRulerMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
}

bool VdovinATopologyRulerMPI::ValidationImpl() {
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    return !GetInput().empty();
  }
  return true;
}

bool VdovinATopologyRulerMPI::PreProcessingImpl() {
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    data_ = GetInput();
  }
  return true;
}

bool VdovinATopologyRulerMPI::RunImpl() {
  int rank = 0;
  int size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int data_size = 0;

  if (rank == 0) {
    data_size = static_cast<int>(data_.size());
  }

  MPI_Bcast(&data_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (rank != 0) {
    data_.resize(data_size);
  }

  if (size == 1) {
    GetOutput() = data_;
    return true;
  }

  if (rank == 0) {
    MPI_Send(data_.data(), data_size, MPI_INT, 1, 0, MPI_COMM_WORLD);
  } else {
    int left_neighbor = rank - 1;
    MPI_Recv(data_.data(), data_size, MPI_INT, left_neighbor, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    if (rank < size - 1) {
      int right_neighbor = rank + 1;
      MPI_Send(data_.data(), data_size, MPI_INT, right_neighbor, 0, MPI_COMM_WORLD);
    }
  }

  GetOutput() = data_;
  return true;
}

bool VdovinATopologyRulerMPI::PostProcessingImpl() {
  return true;
}

}  // namespace vdovin_a_topology_ruler
