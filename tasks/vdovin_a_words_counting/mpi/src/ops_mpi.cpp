#include "vdovin_a_words_counting/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <array>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "vdovin_a_words_counting/common/include/common.hpp"

namespace vdovin_a_words_counting {

namespace {
std::pair<int, std::array<char, 2>> CountWordsInRange(const std::string &input, std::size_t begin, std::size_t end) {
  int counter = 0;
  bool on_word = false;
  for (std::size_t i = begin; i < end; ++i) {
    if (input[i] == ' ' && on_word) {
      ++counter;
      on_word = false;
    } else if (input[i] != ' ') {
      on_word = true;
    }
  }

  std::array<char, 2> flags = {0, 0};
  if (!input.empty() && input[begin] != ' ') {
    flags[0] = 1;
  }
  if (!input.empty() && input[end - 1] != ' ') {
    ++counter;
    flags[1] = 1;
  }

  return {counter, flags};
}

void ComputeSendCountsAndDispls(const std::string &input, int size, std::vector<int> &send_counts,
                                std::vector<int> &send_displs) {
  const std::size_t chunk = input.size() / static_cast<std::size_t>(size);
  for (int i = 0; i < size; ++i) {
    send_displs[i] = static_cast<int>(chunk * static_cast<std::size_t>(i));
    if (i == size - 1) {
      send_counts[i] = static_cast<int>(input.size()) - send_displs[i];
    } else {
      send_counts[i] = static_cast<int>(chunk);
    }
  }
}

void ProcessBoundaryFlagMatching(const std::vector<char> &all_flags, int size, int &counter_sum) {
  for (int i = 1; i < size; ++i) {
    const std::size_t prev_end_idx = (static_cast<std::size_t>(i) * 2U) - 1U;
    const std::size_t curr_begin_idx = prev_end_idx + 1U;
    if ((all_flags[prev_end_idx] == 1) && (all_flags[curr_begin_idx] == 1)) {
      --counter_sum;
    }
  }
}
}  // namespace

VdovinAWordsCountingMPI::VdovinAWordsCountingMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool VdovinAWordsCountingMPI::ValidationImpl() {
  return (!GetInput().empty()) && (GetOutput() == 0);
}

bool VdovinAWordsCountingMPI::PreProcessingImpl() {
  return true;
}

bool VdovinAWordsCountingMPI::RunImpl() {
  int rank = 0;
  int size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  std::string input;
  std::vector<int> send_counts(size, 0);
  std::vector<int> send_displs(size, 0);

  if (rank == 0) {
    input = GetInput();
    if (input.empty()) {
      return false;
    }
    ComputeSendCountsAndDispls(input, size, send_counts, send_displs);
  }

  MPI_Bcast(send_counts.data(), size, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(send_displs.data(), size, MPI_INT, 0, MPI_COMM_WORLD);

  int local_size = send_counts[rank];
  std::vector<char> local_data(local_size);

  std::vector<char> input_chars;
  if (rank == 0) {
    input_chars.assign(input.begin(), input.end());
  }

  MPI_Scatterv(input_chars.empty() && rank == 0 ? nullptr : input_chars.data(), send_counts.data(), send_displs.data(),
               MPI_CHAR, local_data.data(), local_size, MPI_CHAR, 0, MPI_COMM_WORLD);

  std::string local_input(local_data.begin(), local_data.end());

  auto [counter, flags] = CountWordsInRange(local_input, 0, local_input.size());
  std::array<char, 2> local_flags = flags;

  std::vector<char> all_flags;
  if (rank == 0) {
    all_flags.resize(static_cast<std::size_t>(2) * static_cast<std::size_t>(size), 0);
  }

  MPI_Gather(local_flags.data(), 2, MPI_CHAR, (rank == 0 ? all_flags.data() : nullptr), 2, MPI_CHAR, 0, MPI_COMM_WORLD);

  int counter_sum = 0;
  MPI_Reduce(&counter, &counter_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    ProcessBoundaryFlagMatching(all_flags, size, counter_sum);
    GetOutput() = counter_sum;
  }

  MPI_Bcast(&counter_sum, 1, MPI_INT, 0, MPI_COMM_WORLD);
  GetOutput() = counter_sum;
  return true;
}

bool VdovinAWordsCountingMPI::PostProcessingImpl() {
  return true;
}

}  // namespace vdovin_a_words_counting
