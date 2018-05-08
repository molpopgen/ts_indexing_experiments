#include <map>
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
    map<key_type, edge> input_left, output_right;
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
                    auto p2 = u(mt);
                    auto brk = pos(mt);
                    input_left.emplace(
                        std::make_tuple(0., -double(gen), p1, next_node),
                        edge{ p1, next_node, 0., brk });
                    input_left.emplace(
                        std::make_tuple(brk, -double(gen), p2, next_node),
                        edge{ p2, next_node, brk, 1. });
                    output_right.emplace(
                        std::make_tuple(brk, double(gen), p1, next_node),
                        edge{ p1, next_node, 0., brk });
                    output_right.emplace(
                        std::make_tuple(1., double(gen), p2, next_node),
                        edge{ p2, next_node, brk, 1. });
                    ++next_node;
                    input_left.emplace(
                        std::make_tuple(0., -double(gen), p2, next_node),
                        edge{ p2, next_node, 0., brk });
                    input_left.emplace(
                        std::make_tuple(brk, -double(gen), p1, next_node),
                        edge{ p1, next_node, brk, 1. });
                    output_right.emplace(
                        std::make_tuple(brk, double(gen), p2, next_node),
                        edge{ p2, next_node, 0., brk });
                    output_right.emplace(
                        std::make_tuple(1., double(gen), p1, next_node),
                        edge{ p1, next_node, brk, 1. });
                    ++next_node;
                }
                
			algT(input_left,output_right,next_node);
        }
}
