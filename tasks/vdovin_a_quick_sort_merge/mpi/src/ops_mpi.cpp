#include "vdovin_a_quick_sort_merge/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <cstddef>
#include <vector>

#include "vdovin_a_quick_sort_merge/common/include/common.hpp"

namespace vdovin_a_quick_sort_merge {

VdovinAQuickSortMergeMPI::VdovinAQuickSortMergeMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = {};
}

bool VdovinAQuickSortMergeMPI::ValidationImpl() {
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (!GetOutput().empty()) {
    return false;
  }
  if (rank == 0) {
    if (GetInput().size() > 1000000) {
      return false;
    }
    for (const auto &val : GetInput()) {
      if (val < -1000000 || val > 1000000) {
        return false;
      }
    }
  }
  return true;
}

bool VdovinAQuickSortMergeMPI::PreProcessingImpl() {
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  GetOutput() = GetInput();
  if (rank == 0) {
    if (GetOutput().size() != GetInput().size()) {
      return false;
    }
    if (!GetOutput().empty()) {
      for (size_t i = 0; i < GetOutput().size(); i++) {
        if (GetOutput()[i] != GetInput()[i]) {
          return false;
        }
      }
    }
  }
  return true;
}

namespace {

// NOLINTNEXTLINE(misc-no-recursion)
void QuickSort(std::vector<int> &arr, int left, int right) {
  if (left >= right) {
    return;
  }
  int pivot = arr[(left + right) / 2];
  int i = left;
  int j = right;
  while (i <= j) {
    while (arr[i] < pivot) {
      i++;
    }
    while (arr[j] > pivot) {
      j--;
    }
    if (i <= j) {
      std::swap(arr[i], arr[j]);
      i++;
      j--;
    }
  }
  QuickSort(arr, left, j);
  QuickSort(arr, i, right);
}

std::vector<int> Merge(const std::vector<int> &left, const std::vector<int> &right) {
  std::vector<int> result;
  result.reserve(left.size() + right.size());
  size_t i = 0;
  size_t j = 0;
  while (i < left.size() && j < right.size()) {
    if (left[i] <= right[j]) {
      result.push_back(left[i]);
      i++;
    } else {
      result.push_back(right[j]);
      j++;
    }
  }
  while (i < left.size()) {
    result.push_back(left[i]);
    i++;
  }
  while (j < right.size()) {
    result.push_back(right[j]);
    j++;
  }
  return result;
}

}  // namespace

bool VdovinAQuickSortMergeMPI::RunImpl() {
  int rank = 0;
  int size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (GetOutput().empty()) {
    MPI_Barrier(MPI_COMM_WORLD);
    return true;
  }

  int data_size = static_cast<int>(GetOutput().size());
  std::vector<int> sendcounts(size);
  std::vector<int> displs(size);
  int base_size = data_size / size;
  int remainder = data_size % size;

  for (int i = 0; i < size; i++) {
    sendcounts[i] = base_size + (i < remainder ? 1 : 0);
    displs[i] = (i == 0) ? 0 : displs[i - 1] + sendcounts[i - 1];
  }

  std::vector<int> local_data(sendcounts[rank]);

  if (rank == 0) {
    MPI_Scatterv(GetOutput().data(), sendcounts.data(), displs.data(), MPI_INT, local_data.data(), sendcounts[rank],
                 MPI_INT, 0, MPI_COMM_WORLD);
  } else {
    MPI_Scatterv(nullptr, sendcounts.data(), displs.data(), MPI_INT, local_data.data(), sendcounts[rank], MPI_INT, 0,
                 MPI_COMM_WORLD);
  }

  if (!local_data.empty()) {
    QuickSort(local_data, 0, static_cast<int>(local_data.size()) - 1);
  }

  if (rank == 0) {
    GetOutput() = local_data;
    for (int i = 1; i < size; i++) {
      std::vector<int> recv_data(sendcounts[i]);
      MPI_Recv(recv_data.data(), sendcounts[i], MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      GetOutput() = Merge(GetOutput(), recv_data);
    }
    for (int i = 1; i < size; i++) {
      MPI_Send(GetOutput().data(), static_cast<int>(GetOutput().size()), MPI_INT, i, 1, MPI_COMM_WORLD);
    }
  } else {
    MPI_Send(local_data.data(), static_cast<int>(local_data.size()), MPI_INT, 0, 0, MPI_COMM_WORLD);
    int recv_size = 0;
    MPI_Status status;
    MPI_Probe(0, 1, MPI_COMM_WORLD, &status);
    MPI_Get_count(&status, MPI_INT, &recv_size);
    GetOutput().resize(recv_size);
    MPI_Recv(GetOutput().data(), recv_size, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  MPI_Barrier(MPI_COMM_WORLD);
  return true;
}

bool VdovinAQuickSortMergeMPI::PostProcessingImpl() {
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (GetOutput().empty()) {
    return GetInput().empty();
  }
  if (!std::is_sorted(GetOutput().begin(), GetOutput().end())) {
    return false;
  }
  if (GetOutput().size() != GetInput().size()) {
    return false;
  }
  int sum_input = 0;
  int sum_output = 0;
  for (const auto &val : GetInput()) {
    sum_input += val;
  }
  for (const auto &val : GetOutput()) {
    sum_output += val;
  }
  return sum_input == sum_output;
}

}  // namespace vdovin_a_quick_sort_merge
