#include <algorithm> // std::sort
#include <chrono>    // std::chrono::steady_clock, std::chrono::duration
#include <fstream>   // std::ifstream, std::ofstream
#include <iostream>  // std::cout
#include <iterator>  // std::random_access_iterator (concept)
#include <sstream>   // std::istringstream
#include <string>    // std::string, std::getline
#include <tuple>     // std::tie
#include <vector>    // std::vector

#include <cstdlib> // std::system

#include <matplot/matplot.h> // matplot::plot, ...

/**
 * @brief Строка из датасета
 *
 * Представляет собой структуру, содержащую информацию
 * о военнослужащем (ФИО, должность, подразделение, зарплата)
 */
struct Soldier {
  std::string full_name; ///< ФИО
  std::string job;       ///< Должность
  std::string unit;      ///< Подразделение
  int salary;            ///< Зарплата

  Soldier() = default;
  Soldier(std::string f, std::string j, std::string u, int s)
      : full_name(f), job(j), unit(u), salary(s) {}
};

/**
 * @brief Перегрузка оператора "<" для сравнения объектов Soldier
 *
 * Сначала сравниваются подразделения, затем ФИО, затем зарплата.
 */
bool operator<(const Soldier &a, const Soldier &b) {
  return std::tie(a.unit, a.full_name, a.salary) <
         std::tie(b.unit, b.full_name, b.salary);
}

/**
 * @brief Перегрузка оператора "<" для сравнения объектов @see Soldier
 *
 * Сначала сравниваются подразделения, затем ФИО, затем зарплата.
 */
bool operator>(const Soldier &a, const Soldier &b) {
  return std::tie(a.unit, a.full_name, a.salary) >
         std::tie(b.unit, b.full_name, b.salary);
}

/**
 * @brief Перегрузка оператора ">" для сравнения объектов @see Soldier
 *
 * Сначала сравниваются подразделения, затем ФИО, затем зарплата.
 */
bool operator<=(const Soldier &a, const Soldier &b) {
  return std::tie(a.unit, a.full_name, a.salary) <=
         std::tie(b.unit, b.full_name, b.salary);
}

/**
 * @brief Перегрузка оператора ">=" для сравнения объектов @see Soldier
 *
 * Сначала сравниваются подразделения, затем ФИО, затем зарплата.
 */
bool operator>=(const Soldier &a, const Soldier &b) {
  return std::tie(a.unit, a.full_name, a.salary) >=
         std::tie(b.unit, b.full_name, b.salary);
}

/**
 * @brief Разделить строку по разделителю
 *
 * Функция позволяет разделить переданную строку (std::string)
 * по разделителю (по умолчанию - пробел). Используется для
 * считывания датасетов (.csv файл)
 *
 * @param str строка, которую нужно разделить
 * @param del разделитель
 * @return Разделенную строку, представленную в виде вектора строк
 */
std::vector<std::string> split(const std::string &str, char del = ' ') {
  std::vector<std::string> out;
  std::istringstream is(str);
  std::string t;
  while (std::getline(is, t, del)) {
    out.push_back(t);
  }
  return out;
}

/**
 * @brief Считать датасет военнослужащих
 * @param filename Имя датасета (например, "dataset_1.csv")
 * @return Вектор объектов
 */
std::vector<Soldier> read_csv(const std::string &filename) {
  std::vector<Soldier> data;
  data.reserve(150000);
  std::ifstream ifile;
  std::string line;

  ifile.open(filename);
  if (!ifile.is_open()) {
    std::cerr << "read_csv: Couldn't open file\n";
  }

  while (std::getline(ifile, line)) {
    std::vector<std::string> fields_v = split(line, ',');
    Soldier obj(fields_v[0], fields_v[1], fields_v[2], std::stoi(fields_v[3]));
    data.emplace_back(obj);
  }

  return data;
}

/**
 * @brief Записать вектор данных в .csv файл
 * @param filename имя файла
 * @param data вектор объектов
 */
void write_csv(std::string filename, const std::vector<Soldier> &data) {
  std::ofstream ofile(filename);
  if (!ofile.is_open()) {
    std::cerr << "read_csv: Couldn't open file\n";
  }
  for (const auto &v : data) {
    ofile << v.full_name << ',' << v.job << ',' << v.unit << ',' << v.salary
          << '\n';
  }
}

/**
 * @brief Перегрузка оператора<< для вывода контейнера
 */
template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &vec) {
  int width{50};
  for (const auto &v : vec) {
    std::cout << v << ' ';
  }
  std::cout << '\n';
  return os;
}

/**
 * @brief Сортировка вставкой
 * @param first итератор на начало контейнера
 * @param last итератор на конец контейнера
 * @param comp фукнция сравнения
 */
template <std::random_access_iterator RandomAccessIterator, class Compare>
void insertion_sort(RandomAccessIterator first, RandomAccessIterator last,
                    Compare comp) {
  for (auto i = first + 1; i < last; ++i) {
    auto t = *i;
    for (auto j = i - 1; j >= first; --j) {
      if (comp(t, *j)) {
        std::iter_swap(j, j + 1);
      }
    }
  }
}
// 4,5,1,3,2
// 1,4,5,3,2
// 1,3,4,5,2
// 1,2,3,4,5

/**
 * @brief Шейкер-сортировка
 * @param first итератор на начало контейнера
 * @param last итератор на конец контейнера
 * @param comp фукнция сравнения
 */
template <std::random_access_iterator RandomAccessIterator, class Compare>
void shaker_sort(RandomAccessIterator first, RandomAccessIterator last,
                 Compare comp) {
  auto left_bound = first;
  auto right_bound = last - 1;
  bool no_swaps = true;

  while (left_bound <= right_bound) {
    for (auto i = left_bound; i < right_bound; ++i) {
      if (comp(*(i + 1), *i)) {
        std::iter_swap(i + 1, i);
        no_swaps = false;
      }
    }
    ++left_bound;
    if (no_swaps)
      break;

    for (auto i = right_bound; i >= left_bound; --i) {
      if (comp(*i, *(i - 1))) {
        std::iter_swap(i, i - 1);
      }
    }
    --right_bound;
    if (no_swaps)
      break;
  }
}

// 5,4,1,3,2
//

/**
 * @brief  Функция для слияния двух отсортированных массивов при сортировке
 * слиянием
 * @param l_first итератор на начало 1-ого контейнера
 * @param l_last итератор на конец 1-ого контейнера
 * @param r_first итератор на начало 2-ого контейнера
 * @param r_last итератор на конец 2-ого контейнера
 * @param comp функция сравнения
 */
template <std::random_access_iterator RandomAccessIterator, class Compare>
void merge(RandomAccessIterator l_first, RandomAccessIterator l_last,
           RandomAccessIterator r_first, RandomAccessIterator r_last,
           Compare comp) {
  std::vector result(l_first, l_last);
  result.clear();

  auto i = l_first, j = r_first;
  while (i < l_last and j < r_last) {
    if (comp(*i, *j)) {
      result.push_back(*i);
      ++i;
    } else {
      result.push_back(*j);
      ++j;
    }
  }

  for (; i < l_last; ++i) {
    result.push_back(*i);
  }

  for (; j < r_last; ++j) {
    result.push_back(*j);
  }

  std::copy(result.begin(), result.end(), l_first);
}

/**
 * @brief Сортировка слиянием
 * @param first итератор на начало контейнера
 * @param last итератор на конец контейнера
 * @param comp фукнция сравнения
 */
template <std::random_access_iterator RandomAccessIterator, class Compare>
void merge_sort(RandomAccessIterator first, RandomAccessIterator last,
                Compare comp) {
  if (first + 1 == last) {
    return;
  }

  long mid = std::distance(first, last) / 2;

  merge_sort(first, first + mid, comp);
  merge_sort(first + mid, last, comp);

  return merge(first, first + mid, first + mid, last, comp);
}

/**
 * @brief Функция для замера времени работы сортировок
 * @param j число датасетов для сортивроки
 * @param algo какой алгоритм использовать
 * @return пара векторов значений (x,y), x - размеры датасетов, y -
 * соотвествущее время сортивки (в сек.) для выборанного алгоритма
 */
std::pair<std::vector<double>, std::vector<double>> get_time(int j,
                                                             std::string algo) {
  std::vector<double> x, y;
  for (int i{1}; i <= j; ++i) {
    auto data = read_csv("./data/in/dataset_" + std::to_string(i) + ".csv");
    const auto start{std::chrono::steady_clock::now()};

    if (algo == "insertion_sort")
      insertion_sort(data.begin(), data.end(), std::less<Soldier>());
    else if (algo == "shaker_sort")
      shaker_sort(data.begin(), data.end(), std::less<Soldier>());
    else if (algo == "merge_sort")
      merge_sort(data.begin(), data.end(), std::less<Soldier>());
    else if (algo == "std::sort")
      std::sort(data.begin(), data.end(), std::less<Soldier>());

    const auto finish{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds{finish - start};
    x.push_back(data.size());
    y.push_back(elapsed_seconds.count());

    write_csv("data/out/insertion/dataset_" + std::to_string(i) + ".csv", data);

    // std::cout << "insertion_sort: iter=" << i << " done\n";

    std::cout << algo + ": dataset_n=" << i << " size=" << data.size()
              << " time=" << y.at(i - 1) << "\n";
  }

  return {x, y};
}

/**
 * @brief основная функция программы
 *
 * Считывание данных из датасетов, замер времени различных сортировок,
 * запись отсортированных данных, постройка графиков
 */
int main() {

  std::system("rm -rf data/out/ && mkdir data/out/ data/out/insertion/ "
              "data/out/shaker/ data/out/merge/ data/out/sort/ data/out/plots/ "
              "data/out/plots/svg/ data/out/plots/jpg/");

  auto insertion_time{get_time(6, "insertion_sort")};

  auto shaker_time{get_time(6, "shaker_sort")};

  auto merge_time{get_time(15, "merge_sort")};

  auto stdsort_time{get_time(15, "std::sort")};

  auto y1 = insertion_time.second;
  auto y2 = shaker_time.second;
  auto y3 = merge_time.second;
  auto y4 = stdsort_time.second;
  auto x = stdsort_time.first;

  matplot::plot(x, y1, "-o", x, y2, "-o", x, y3, "-o", x, y4, "-o");
  matplot::title("Insertion vs shaker vs merge vs std::sort");
  matplot::xlabel("Dataset size");
  matplot::ylabel("Time to sort (s)");
  matplot::legend({"insertion", "shaker", "merge", "std::sort"});
  matplot::save("data/out/plots/svg/all.svg");
  matplot::save("data/out/plots/jpg/all.jpg");

  matplot::plot(x, y1, "-o", x, y2, "-o");
  matplot::title("Insertion vs shaker");
  matplot::xlabel("Dataset size");
  matplot::ylabel("Time to sort (s)");
  matplot::legend({"insertion", "shaker"});
  matplot::save("data/out/plots/svg/insertion_shaker.svg");
  matplot::save("data/out/plots/jpg/insertion_shaker.jpg");

  matplot::plot(x, y3, "-o", x, y4, "-o");
  matplot::title("merge vs std::sort");
  matplot::xlabel("Dataset size");
  matplot::ylabel("Time to sort (s)");
  matplot::legend({"merge", "std::sort"});
  matplot::save("data/out/plots/svg/merge_stdsort.svg");
  matplot::save("data/out/plots/jpg/merge_stdsort.jpg");

  return 0;
}
