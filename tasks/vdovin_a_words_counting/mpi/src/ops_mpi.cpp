#include "vdovin_a_words_counting/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <array>
#include <cstddef>
#include <string>
#include <vector>

#include "vdovin_a_words_counting/common/include/common.hpp"

namespace vdovin_a_words_counting {

static std::pair<int, std::array<char, 2>> CountWordsInRange(const std::string &input, std::size_t begin,
                                                             std::size_t end) {
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
  auto input = GetInput();
  if (input.empty()) {
    return false;
  }
  int rank = 0;
  int size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const std::size_t chunk = input.size() / static_cast<std::size_t>(size);
  const std::size_t begin = chunk * static_cast<std::size_t>(rank);
  std::size_t end = begin + chunk;
  if (rank == size - 1) {
    end = input.size();
  }

  std::array<char, 2> local_flags = {0, 0};
  std::vector<char> all_flags;
  if (rank == 0) {
    all_flags.resize(static_cast<std::size_t>(2) * static_cast<std::size_t>(size), 0);
  }

  auto [counter, flags] = CountWordsInRange(input, begin, end);
  local_flags = flags;

  MPI_Gather(local_flags.data(), 2, MPI_CHAR, (rank == 0 ? all_flags.data() : nullptr), 2, MPI_CHAR, 0, MPI_COMM_WORLD);

  int counter_sum = 0;
  MPI_Reduce(&counter, &counter_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    for (int i = 1; i < size; ++i) {
      const std::size_t prev_end_idx = static_cast<std::size_t>(i) * 2u - 1u;
      const std::size_t curr_begin_idx = prev_end_idx + 1u;
      if ((all_flags[prev_end_idx] == 1) && (all_flags[curr_begin_idx] == 1)) {
        --counter_sum;
      }
    }
  }

  MPI_Bcast(&counter_sum, 1, MPI_INT, 0, MPI_COMM_WORLD);
  GetOutput() = counter_sum;
  return true;
}

bool VdovinAWordsCountingMPI::PostProcessingImpl() {
  return true;
}

}  // namespace vdovin_a_words_counting
