#include "vdovin_a_quick_sort_merge/seq/include/ops_seq.hpp"

#include <algorithm>
#include <cstddef>
#include <vector>

#include "vdovin_a_quick_sort_merge/common/include/common.hpp"

namespace vdovin_a_quick_sort_merge {

VdovinAQuickSortMergeSEQ::VdovinAQuickSortMergeSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = {};
}

bool VdovinAQuickSortMergeSEQ::ValidationImpl() {
  if (!GetOutput().empty()) {
    return false;
  }
  if (GetInput().size() > 1000000) {
    return false;
  }
  return std::all_of(GetInput().begin(), GetInput().end(), [](int val) { return val >= -1000000 && val <= 1000000; });
}

bool VdovinAQuickSortMergeSEQ::PreProcessingImpl() {
  GetOutput() = GetInput();
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

bool VdovinAQuickSortMergeSEQ::RunImpl() {
  if (GetOutput().empty()) {
    return true;
  }
  size_t size = GetOutput().size();
  if (size == 1) {
    return true;
  }
  size_t mid = size / 2;
  std::vector<int> left(GetOutput().begin(), GetOutput().begin() + static_cast<ptrdiff_t>(mid));
  std::vector<int> right(GetOutput().begin() + static_cast<ptrdiff_t>(mid), GetOutput().end());
  QuickSort(left, 0, static_cast<int>(left.size()) - 1);
  QuickSort(right, 0, static_cast<int>(right.size()) - 1);
  GetOutput() = Merge(left, right);
  return true;
}

bool VdovinAQuickSortMergeSEQ::PostProcessingImpl() {
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
