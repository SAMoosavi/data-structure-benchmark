#include <benchmark/benchmark.h>
#include "plf_list.h"
#include "plf_colony.h"
#include <list>
#include "utils.h"
#include <vector>
#include <algorithm>
#include <string>
#include <deque>
#include <flat_set>
#include <boost/container/flat_set.hpp>
#include "unordered_dense.h"

enum class Op {
    InsertLast,
    Find,
    RemoveLast,
    RemoveByKey,
    Iterate
};

template<typename T>
struct Workload {
    std::vector<Op> ops;
    std::vector<T> values;
};

template<typename T>
Workload<T> make_workload(size_t n_ops, uint32_t seed = 42) {
    const std::vector<OpSpec<Op>> op_dist({
                                                  {Op::InsertLast,  40},
                                                  {Op::Find,        0},
                                                  {Op::RemoveByKey, 20},
                                                  {Op::RemoveLast,  20},
                                                  {Op::Iterate,     20}
                                          });

    Workload<T> w;
    const auto ops = generate_ops(op_dist, n_ops, seed);
    w.ops = ops;
    w.values.reserve(n_ops);

    for (size_t i = 0; i < n_ops; ++i) {
        if constexpr (std::is_integral_v<T>) {
            w.values.push_back(static_cast<T>(generate_number<size_t>(1, n_ops * 10, seed)));
        } else {
            w.values.push_back(generate_string(10, seed));
        }
    }

    return w;
}

template<typename List, typename T>
void run_list_benchmark(benchmark::State &state, const Workload<T> &w) {
    for (auto _: state) {
        List lst;

        for (size_t i = 0; i < w.ops.size(); ++i) {
            const T &val = w.values[i];

            switch (w.ops[i]) {
                case Op::InsertLast:
                    lst.push_back(val);
                    break;

                case Op::Find:
                    benchmark::DoNotOptimize(
                            std::find(lst.begin(), lst.end(), val));
                    break;

                case Op::RemoveLast:
                    if (!lst.empty())
                        lst.pop_back();
                    break;

                case Op::RemoveByKey: {
                    auto it = std::find(lst.begin(), lst.end(), val);
                    if (it != lst.end())
                        lst.erase(it);
                    break;
                }
                case Op::Iterate: {
                    for (auto const &it: lst)
                        benchmark::DoNotOptimize(it);
                }
                default:;
            }
        }

        benchmark::ClobberMemory();
    }
}

template<>
void run_list_benchmark<plf::colony<int>, int>(benchmark::State &state, const Workload<int> &w) {
    for (auto _: state) {
        plf::colony<int> lst;
        lst.reserve(w.ops.size());

        for (size_t i = 0; i < w.ops.size(); ++i) {
            const int &val = w.values[i];

            switch (w.ops[i]) {
                case Op::InsertLast:
                    lst.emplace(val);
                    break;

                case Op::Find:
                    benchmark::DoNotOptimize(
                            std::find(lst.begin(), lst.end(), val));
                    break;

                case Op::RemoveLast:
                    if (!lst.empty()) {
                        auto it = lst.end();
                        --it;
                        lst.erase(it);
                    }
                    break;

                case Op::RemoveByKey: {
                    auto it = std::find(lst.begin(), lst.end(), val);
                    if (it != lst.end())
                        lst.erase(it);
                    break;
                }
                case Op::Iterate: {
                    for (auto const &it: lst)
                        benchmark::DoNotOptimize(it);
                }
                default:;
            }
        }

        benchmark::ClobberMemory();
    }
}

template<>
void
run_list_benchmark<plf::colony<std::string>, std::string>(benchmark::State &state, const Workload<std::string> &w) {
    for (auto _: state) {
        plf::colony<std::string> lst;
        lst.reserve(w.ops.size());

        for (size_t i = 0; i < w.ops.size(); ++i) {
            const std::string &val = w.values[i];

            switch (w.ops[i]) {
                case Op::InsertLast:
                    lst.emplace(val);
                    break;

                case Op::Find:
                    benchmark::DoNotOptimize(
                            std::find(lst.begin(), lst.end(), val));
                    break;

                case Op::RemoveLast:
                    if (!lst.empty()) {
                        auto it = lst.end();
                        --it;
                        lst.erase(it);
                    }
                    break;

                case Op::RemoveByKey: {
                    auto it = std::find(lst.begin(), lst.end(), val);
                    if (it != lst.end())
                        lst.erase(it);
                    break;
                }
                case Op::Iterate: {
                    for (auto const &it: lst)
                        benchmark::DoNotOptimize(it);
                }
                default:;
            }
        }

        benchmark::ClobberMemory();
    }
}

template<typename List, typename T>
void run_tree_benchmark(benchmark::State &state, const Workload<T> &w) {
    for (auto _: state) {
        List lst;

        for (size_t i = 0; i < w.ops.size(); ++i) {
            const T &val = w.values[i];

            switch (w.ops[i]) {
                case Op::InsertLast:
                    lst.emplace(val);
                    break;

                case Op::Find:
                    benchmark::DoNotOptimize(
                            lst.find(val));
                    break;

                case Op::RemoveLast:
                    if (!lst.empty()) {
                        auto it = lst.end();
                        --it;
                        lst.erase(it);
                    }
                    break;

                case Op::RemoveByKey: {
                    lst.erase(val);
                    break;
                }
                case Op::Iterate: {
                    for (auto const &it: lst)
                        benchmark::DoNotOptimize(it);
                }
                default:;
            }
        }

        benchmark::ClobberMemory();
    }
}

template<typename List, typename T>
static void BM_Container_list(benchmark::State &state) {
    const auto w = make_workload<T>(state.range(0));
    run_list_benchmark<List, T>(state, w);
}

template<typename List, typename T>
static void BM_Container_set(benchmark::State &state) {
    const auto w = make_workload<T>(state.range(0));
    run_tree_benchmark<List, T>(state, w);
}

// Structure definitions for cleaner registration
struct ContainerInfo {
    std::string name;
    std::function<void(benchmark::State &)> bench_func_int;
    std::function<void(benchmark::State &)> bench_func_string;
};

// Benchmark registration with hierarchical naming
int main(int argc, char **argv) {
    benchmark::Initialize(&argc, argv);

    // Configuration: sizes, types, and structures
    const std::vector<int> sizes = {1 << 20};

    const std::vector<std::string> types = {"int", "string"};

    const std::vector<ContainerInfo> containers = {
            {"std::list",
                    BM_Container_list<std::list<int>, int>,
                    BM_Container_list<std::list<std::string>, std::string>},
            {"std::deque",
                    BM_Container_list<std::deque<int>, int>,
                    BM_Container_list<std::deque<std::string>, std::string>},
            {"std::vector",
                    BM_Container_list<std::vector<int>, int>,
                    BM_Container_list<std::vector<std::string>, std::string>},
            {"plf::list",
                    BM_Container_list<plf::list<int>, int>,
                    BM_Container_list<plf::list<std::string>, std::string>},
            {"plf::colony",
                    BM_Container_list<plf::colony<int>, int>,
                    BM_Container_list<plf::colony<std::string>, std::string>},
            {"std::set",
                    BM_Container_set<std::set<int>, int>,
                    BM_Container_set<std::set<std::string>, std::string>},
            {"std::flat_set",
                    BM_Container_set<std::flat_set<int>, int>,
                    BM_Container_set<std::flat_set<std::string>, std::string>},
            {"boost::container::flat_set",
                    BM_Container_set<boost::container::flat_set<int>, int>,
                    BM_Container_set<boost::container::flat_set<std::string>, std::string>},
            {"ankerl::unordered_dense::set",
                    BM_Container_set<ankerl::unordered_dense::set<int>, int>,
                    BM_Container_set<ankerl::unordered_dense::set<std::string>, std::string>},
    };

    // Register all combinations: type -> size -> structure
    for (const auto &type: types) {
        for (int size: sizes) {
            std::string prefix = "type:" + type + "/size:" + std::to_string(size) + "/";

            for (const auto &container: containers) {
                std::string name = prefix + "structure:" + container.name;

                // Select the appropriate benchmark function based on type
                auto bench_func = (type == "int")
                                  ? container.bench_func_int
                                  : container.bench_func_string;

                benchmark::RegisterBenchmark(name, bench_func)
                        ->Arg(size)
                        ->Unit(benchmark::kMicrosecond);
            }
        }
    }

    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
    return 0;
}
