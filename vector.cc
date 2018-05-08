#include <vector>
#include <iostream>
#include <algorithm>
#include <tuple>
#include <random>
#include <cstdlib>
#include "util.hpp"

using namespace std;

int
main(int argc, char** argv)
{
    using pair_type = pair<key_type, edge>;
    vector<pair_type> input_left, output_right;
    input_left.reserve(10000);
    size_t last_size = 0;

    int N = atoi(argv[1]);
    int ngens = atoi(argv[2]);
    int next_node = 2 * N;
    mt19937 mt(42);
    uniform_real_distribution<> pos(0., 1.);
    for (int gen = 0; gen < ngens; ++gen)
        {
            uniform_int_distribution<> u(next_node - 2 * N, next_node);
            for (int ind = 0; ind < N; ++ind)
                {
                    auto p1 = u(mt);
                    auto brk = pos(mt);
                    input_left.emplace_back(
                        std::make_tuple(0., -double(gen), p1, next_node),
                        edge{ p1, next_node, 0., brk });
                    output_right.emplace_back(
                        std::make_tuple(brk, double(gen), p1, next_node),
                        edge{ p1, next_node, 0., brk });
                    p1 = u(mt);
                    ++next_node;
                    input_left.emplace_back(
                        std::make_tuple(brk, -double(gen), p1, next_node),
                        edge{ p1, next_node, brk, 1. });
                    output_right.emplace_back(
                        std::make_tuple(1., double(gen), p1, next_node),
                        edge{ p1, next_node, brk, 1. });
                    ++next_node;
                }
            sort(input_left.begin(), input_left.end(),
                 [](const pair_type& a, const pair_type& b) {
                     return a.first < b.first;
                 });
            sort(output_right.begin(), output_right.end(),
                 [](const pair_type& a, const pair_type& b) {
                     return a.first < b.first;
                 });
            algT(input_left, output_right, next_node);
            last_size = input_left.size();
        }
}
