#include <benchmark/benchmark.h>
#include "plf_list.h"
#include "plf_colony.h"
#include <list>
#include "utils.h"
#include <vector>
#include <algorithm>
#include <string>

enum class Op
{
    InsertLast,
    Find,
    RemoveLast,
    RemoveByKey
};


template <typename T>
struct Workload
{
    std::vector<Op> ops;
    std::vector<T> values;
};

template <typename T>
Workload<T> make_workload(size_t n_ops, uint32_t seed = 42)
{
    const std::vector<OpSpec<Op>> op_dist({
        {Op::InsertLast, 40},
        {Op::Find, 20},
        {Op::RemoveByKey, 20},
        {Op::RemoveLast, 20},
    });

    Workload<T> w;
    const auto ops = generate_ops(op_dist, n_ops, seed);
    w.ops = ops;
    w.values.reserve(n_ops);

    for (size_t i = 0; i < n_ops; ++i)
    {
        if constexpr (std::is_integral_v<T>)
        {
            w.values.push_back(static_cast<T>(generate_number<size_t>(1, n_ops * 10, seed)));
        }
        else
        {
            w.values.push_back(generate_string(10, seed));
        }
    }

    return w;
}


template <typename List, typename T>
void run_list_benchmark(benchmark::State& state,
                        const Workload<T>& w)
{
    for (auto _ : state)
    {
        List lst;

        for (size_t i = 0; i < w.ops.size(); ++i)
        {
            const T& val = w.values[i];

            switch (w.ops[i])
            {
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

            case Op::RemoveByKey:
                {
                    auto it = std::find(lst.begin(), lst.end(), val);
                    if (it != lst.end())
                        lst.erase(it);
                    break;
                }
            default: ;
            }
        }

        benchmark::ClobberMemory();
    }
}

template <>
void run_list_benchmark<plf::colony<int>, int>(benchmark::State& state,
                                               const Workload<int>& w)
{
    for (auto _ : state)
    {
        plf::colony<int> lst;
        lst.reserve(w.ops.size());

        for (size_t i = 0; i < w.ops.size(); ++i)
        {
            const int& val = w.values[i];

            switch (w.ops[i])
            {
            case Op::InsertLast:
                lst.emplace(val);
                break;

            case Op::Find:
                benchmark::DoNotOptimize(
                    std::find(lst.begin(), lst.end(), val));
                break;

            case Op::RemoveLast:
                if (!lst.empty())
                    lst.erase(lst.end());
                break;

            case Op::RemoveByKey:
                {
                    auto it = std::find(lst.begin(), lst.end(), val);
                    if (it != lst.end())
                        lst.erase(it);
                    break;
                }
            default: ;
            }
        }

        benchmark::ClobberMemory();
    }
}

template <>
void run_list_benchmark<plf::colony<std::string>, std::string>(benchmark::State& state,
                                                               const Workload<std::string>& w)
{
    for (auto _ : state)
    {
        plf::colony<std::string> lst;

        for (size_t i = 0; i < w.ops.size(); ++i)
        {
            const std::string& val = w.values[i];

            switch (w.ops[i])
            {
            case Op::InsertLast:
                lst.emplace(val);
                break;

            case Op::Find:
                benchmark::DoNotOptimize(
                    std::find(lst.begin(), lst.end(), val));
                break;

            case Op::RemoveLast:
                if (!lst.empty())
                    lst.erase(lst.end());
                break;

            case Op::RemoveByKey:
                {
                    auto it = std::find(lst.begin(), lst.end(), val);
                    if (it != lst.end())
                        lst.erase(it);
                    break;
                }
            default: ;
            }
        }

        benchmark::ClobberMemory();
    }
}

static void BM_std_list_int(benchmark::State& state)
{
    const auto w = make_workload<int>(state.range(0));
    run_list_benchmark<std::list<int>>(state, w);
}

static void BM_plf_list_int(benchmark::State& state)
{
    const auto w = make_workload<int>(state.range(0));
    run_list_benchmark<plf::list<int>>(state, w);
}

static void BM_plf_colony_int(benchmark::State& state)
{
    const auto w = make_workload<int>(state.range(0));
    run_list_benchmark<plf::colony<int>>(state, w);
}

static void BM_std_list_string(benchmark::State& state)
{
    const auto w = make_workload<std::string>(state.range(0));
    run_list_benchmark<std::list<std::string>>(state, w);
}

static void BM_plf_list_string(benchmark::State& state)
{
    const auto w = make_workload<std::string>(state.range(0));
    run_list_benchmark<plf::list<std::string>>(state, w);
}

static void BM_plf_colony_string(benchmark::State& state)
{
    const auto w = make_workload<std::string>(state.range(0));
    run_list_benchmark<plf::colony<std::string>>(state, w);
}

BENCHMARK(BM_std_list_int)
    ->RangeMultiplier(2)
    ->Range(1 << 10, 1 << 17)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_plf_list_int)
    ->RangeMultiplier(2)
    ->Range(1 << 10, 1 << 17)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_plf_colony_int)
    ->RangeMultiplier(2)
    ->Range(1 << 10, 1 << 17)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_std_list_string)
    ->RangeMultiplier(2)
    ->Range(1 << 10, 1 << 17)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_plf_list_string)
    ->RangeMultiplier(2)
    ->Range(1 << 10, 1 << 17)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_plf_colony_string)
    ->RangeMultiplier(2)
    ->Range(1 << 10, 1 << 17)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
