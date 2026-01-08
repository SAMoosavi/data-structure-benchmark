//
// Created by user on 1/8/26.
//

#ifndef MYBENCHMARK_UTILS_H
#define MYBENCHMARK_UTILS_H

#include <vector>
#include <random>
#include <algorithm>
#include <stdexcept>
#include <cmath>

template <typename Op>
struct OpSpec
{
    Op op;
    int percent; // must sum to 100
};

template <typename Op>
std::vector<Op>
generate_ops(const std::vector<OpSpec<Op>>& specs,
             size_t total_ops,
             const uint32_t seed = 42)
{
    if (specs.empty())
        throw std::invalid_argument("specs is empty");

    int sum = 0;
    for (const auto& s : specs)
        sum += s.percent;

    if (sum != 100)
        throw std::invalid_argument("percent sum must be 100");

    std::vector<Op> ops;
    ops.reserve(total_ops);

    size_t assigned = 0;
    for (size_t i = 0; i < specs.size(); ++i)
    {
        size_t count =
            (i + 1 == specs.size())
                ? (total_ops - assigned)
                : static_cast<size_t>(
                    std::llround(specs[i].percent * total_ops / 100.0));

        assigned += count;
        ops.insert(ops.end(), count, specs[i].op);
    }

    std::mt19937 rng(seed);
    std::shuffle(ops.begin(), ops.end(), rng);

    return ops;
}

template <typename T>
T generate_number(T min, T max, const uint32_t seed = 42)
{
    std::mt19937 rng(seed);
    static_assert(std::is_integral_v<T>);
    std::uniform_int_distribution<T> dist(min, max);
    return dist(rng);
}

inline std::string generate_string(const std::size_t len, const uint32_t seed = 42)
{
    constexpr std::string_view alphabet =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";
    std::mt19937 rng(seed);
    std::uniform_int_distribution<std::size_t> dist(
        0, alphabet.size() - 1);

    std::string s;
    s.resize(len);

    for (auto& c : s)
        c = alphabet[dist(rng)];

    return s;
}

#endif //MYBENCHMARK_UTILS_H
