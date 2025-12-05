# Подсчет количества слов в строке

- **Студент**: Вдовин Артемий Николаевич
- **Группа**: 3823Б1ПР4
- **Технология**: SEQ | MPI
- **Вариант**: 24

## 1. Введение
Обработка текстовых данных - одна из распространенных задач в программировании. Подсчет слов в строке является базовой операцией при анализе текста.

Цель работы - реализовать алгоритм подсчета слов в строке, распараллелить его при помощи технологии MPI для ускорения обработки больших текстов.

## 2. Постановка задачи
Дана строка символов.

Требуется подсчитать количество слов в строке. Словом считается последовательность символов, разделенная пробелами.

Тип входных данных:
```cpp
using InType = std::string;
```
Тип выходных данных:
```cpp
using OutType = int;
```
Ограничения:
- Строка должна быть непустой

## 3. Базовый алгоритм (последовательная версия)

Алгоритм подсчета слов в строке:
- Инициализировать счетчик слов и флаг нахождения внутри слова
- Пройти по каждому символу строки:
  - Если текущий символ - пробел и мы находимся внутри слова:
    - Увеличить счетчик слов
    - Сбросить флаг нахождения внутри слова
  - Если текущий символ не пробел:
    - Установить флаг нахождения внутри слова
- После завершения цикла, если флаг нахождения внутри слова установлен, увеличить счетчик

**Код алгоритма:**
```cpp
int counter = 0;
bool on_word = false;
for(std::size_t i = 0; i < input.size(); i++) {
  if(input[i] == ' ' && on_word) {
      counter++;
      on_word = false;
  } else if(input[i] != ' ') {
    on_word = true;
  }
}
if(on_word) {
  counter++;
}
```

**Характеристики:**

| Параметр                  | Значение |
|---------------------------|----------|
| Сложность по времени      | O(n)     |
| Сложность по памяти       | O(1)     |

## 4. Схема распараллеливания

Распараллеливание организовано по схеме распределения данных. Исходная строка разбивается на равные части между процессами.

**Распределение данных:**
```cpp
int rank = 0, size = 0;
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
MPI_Comm_size(MPI_COMM_WORLD, &size);

std::size_t chank = input.size() / size;
std::size_t begin = chank * rank;  
std::size_t end = begin + chank;
if(rank == size - 1) {
  end = input.size();
}
```

**Локальный подсчет слов:**
Каждый процесс подсчитывает слова на своем участке строки:
```cpp
int counter = 0;
bool on_word = false;
for(std::size_t i = begin; i < end; i++) {
  if(input[i] == ' ' && on_word) {
      counter++;
      on_word = false;
  } else if(input[i] != ' ') {
    on_word = true;
  }
}
```

**Корректировка границ:**
Для корректного учета слов, разорванных между процессами, используется массив флагов:
```cpp
std::vector<char> start_end_with_word(2, 0);
if(rank == 0) {
  start_end_with_word.resize(2 * size, 0);
} 

if(input[begin] != ' '){
  start_end_with_word[0] = 1;  // Начинается с слова
}
if(input[end - 1] != ' '){
  counter++;  
  start_end_with_word[1] = 1;  // Заканчивается словом
}
```

**Сбор и объединение результатов:**
```cpp
MPI_Gather(start_end_with_word.data(), 2, MPI_CHAR, start_end_with_word.data(), 2, MPI_CHAR, 0, MPI_COMM_WORLD);
int counter_sum = 0;
MPI_Reduce(&counter, &counter_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

// Корректировка двойного учета слов на границах
if(rank == 0) {
  for(int i = 1; i < size; i++) {
    if((start_end_with_word[i * 2 - 1] == 1) && (start_end_with_word[i * 2] == 1)){
      counter_sum--;
    }
  }
}
MPI_Bcast(&counter_sum, 1, MPI_INT, 0, MPI_COMM_WORLD);
```

### Схема работы программы
```
┌──────────────────────────────┐
│        Входная строка        │
└────────┬─────────────────────┘
         │ (разделение на части)
         ↓
 ┌─────────────────┬─────────────────┬─────────────────┐
 │  Процесс 0      │  Процесс 1      │  Процесс 2      │  
 │   часть 0       │   часть 1       │   часть 2       │
 └───────┬─────────┴─────────────────┴─────────────────┘
         │ (локальный подсчет слов)
         ↓
 ┌───────────────────┬─────────────────────┬────────────────────┐
 │  local_count(0)   │   local_count(1)    │   local_count(2)   │
 │  flags[0,1]       │   flags[2,3]        │   flags[4,5]       │
 └───────┬───────────┴─────────────────────┴────────────────────┘
         │ MPI_Gather + MPI_Reduce
         ↓
┌───────────────────────────────────────┐
│  Процесс 0 собирает counts и flags    │
└────────┬──────────────────────────────┘
         │ (корректировка границ)
         ↓
┌───────────────────────────────────────┐
│  Процесс 0 вычисляет общий результат  │
└────────┬──────────────────────────────┘
         │ MPI_Bcast
         ↓
┌─────────────────────────────────────────────────┐
│   Все процессы получают итоговый счетчик слов   │
└─────────────────────────────────────────────────┘
```

## 5. Детали реализации

**Структура проекта**
|          Файл          |                 Назначение                  |
|------------------------|---------------------------------------------|
| `common.hpp`           | Определение входных и выходных типов задачи |
| `ops_seq.hpp/.cpp`     |         Последовательная реализация         |
| `ops_mpi.hpp/.cpp`     |                MPI-реализация               |
| `functional/main.cpp`  |             Функциональные тесты            |
| `performance/main.cpp` |       Тестирование производительности       |

### common.hpp
```cpp
#pragma once

#include <string>
#include <tuple>

#include "task/include/task.hpp"

namespace vdovin_a_words_counting {

using InType = std::string;
using OutType = int;
using TestType = std::tuple<std::string, int, int>;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace vdovin_a_words_counting
```

### ops_seq.hpp
```cpp
#pragma once

#include "vdovin_a_words_counting/common/include/common.hpp"
#include "task/include/task.hpp"

namespace vdovin_a_words_counting {

class VdovinAWordsCountingSEQ : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kSEQ;
  }
  explicit VdovinAWordsCountingSEQ(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace vdovin_a_words_counting
```

### ops_seq.cpp
```cpp
#include "vdovin_a_words_counting/seq/include/ops_seq.hpp"

#include <numeric>
#include <vector>

#include "vdovin_a_words_counting/common/include/common.hpp"
#include "util/include/util.hpp"

namespace vdovin_a_words_counting {

VdovinAWordsCountingSEQ::VdovinAWordsCountingSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool VdovinAWordsCountingSEQ::ValidationImpl() {
  return (!GetInput().empty()) && (GetOutput() == 0);
}

bool VdovinAWordsCountingSEQ::PreProcessingImpl() {
  return true;
}

bool VdovinAWordsCountingSEQ::RunImpl() {
  auto input = GetInput();
  if (input.empty()) {
    return false;
  }
  
  int counter = 0;
  bool on_word = false;
  for(std::size_t i = 0; i < input.size(); i++) {
    if(input[i] == ' ' && on_word) {
        counter++;
        on_word = false;
    } else if(input[i] != ' ') {
      on_word = true;
    }
  }
  if(on_word) {
    counter++;
  }
  GetOutput() = counter;
  return true;
}

bool VdovinAWordsCountingSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace vdovin_a_words_counting
```

### ops_mpi.hpp
```cpp
#pragma once

#include "vdovin_a_words_counting/common/include/common.hpp"
#include "task/include/task.hpp"

namespace vdovin_a_words_counting {

class VdovinAWordsCountingMPI : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }
  explicit VdovinAWordsCountingMPI(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace vdovin_a_words_counting
```

### ops_mpi.cpp
```cpp
#include "vdovin_a_words_counting/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <iostream>
#include <string>
#include <vector>

#include "vdovin_a_words_counting/common/include/common.hpp"

namespace vdovin_a_words_counting {

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
  int rank = 0, size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  std::size_t chank = input.size() / size;
  std::size_t begin = chank * rank;  
  std::size_t end = begin + chank;
  if(rank == size - 1) {
    end = input.size();
  }

  std::vector<char> start_end_with_word(2, 0);
  if(rank == 0) {
    start_end_with_word.resize(2 * size, 0);
  } 

  int counter = 0;
  bool on_word = false;
  for(std::size_t i = begin; i < end; i++) {
    if(input[i] == ' ' && on_word) {
        counter++;
        on_word = false;
    } else if(input[i] != ' ') {
      on_word = true;
    }
  }

  if(input[begin] != ' '){
    start_end_with_word[0] = 1;
  }
  if(input[end - 1] != ' '){
    counter++;  
    start_end_with_word[1] = 1;
  }
  
  MPI_Gather(start_end_with_word.data(), 2, MPI_CHAR, start_end_with_word.data(), 2, MPI_CHAR, 0, MPI_COMM_WORLD);
  int counter_sum = 0;
  MPI_Reduce(&counter, &counter_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  
  if(rank == 0) {
    for(int i = 1; i < size; i++) {
      if((start_end_with_word[i * 2 - 1] == 1) && (start_end_with_word[i * 2] == 1)){
        counter_sum--;
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
```

## 6. Экспериментальная среда

|  Компонент |               Значение                       |
|------------|----------------------------------------------|
|     CPU    |           Apple M2 (8 cores)                 |
|     RAM    |                 16 GB                        |
|     ОС     | OS: Ubuntu 24.04 (DevContainer / macOs)      |
| Компилятор | GCC 13.3.0 (g++), C++20, CMake, Release      |
|     MPI    |        mpirun (Open MPI) 4.1.6               |

Тестовые данные генерируются случайным образом:
- Слова состоят из букв 'a'-'z'
- Длина слов: 1-10 символов
- Слова разделяются пробелами

## 7. Результаты и обсуждение

### 7.1 Корректность
Функциональные тесты проверяют корректность подсчета слов на различных наборах данных:
- 1 слово
- 7 слов  
- 1000 слов

Обе реализации (`SEQ`, `MPI`) прошли все тесты успешно.

### 7.2 Производительность
Тестирование производительности проводилось на строке, содержащей 1000 слов.

| Процессов | Время, с | Ускорение | Эффективность |
|-----------|----------|-----------|---------------|
| 1 (SEQ)   | 0.0012   | 1.00      | N/A           |
| 2         | 0.0008   | 1.50      | 75%           |
| 4         | 0.0005   | 2.40      | 60%           |

## 8. Заключение

В ходе выполнения работы:
- Реализован алгоритм подсчета слов в строке
- Разработана MPI-версия алгоритма с коррекцией граничных условий
- Создана система тестирования для проверки корректности
- Проведены измерения производительности

Алгоритм демонстрирует хорошее ускорение при увеличении количества процессов, хотя эффективность снижается из-за накладных расходов на коммуникацию и коррекцию границ.

## 9. Источники
1. Open MPI Documentation
2. MPI Standard
```
