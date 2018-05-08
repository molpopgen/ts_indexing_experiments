#ifndef UTIL_HPP__
#define UTIL_HPP__

#include <tuple>
#include <algorithm>
#include <vector>
#include <cassert>

using key_type = std::tuple<double, double, int, int>;

struct edge
{
    int p, c;
    double l, r;
};

template <typename index_container>
void
algT(const index_container& input_left, const index_container& output_right,
     const int maxnodes)
{
    auto j = input_left.begin(), jM = input_left.end(),
         k = output_right.begin(), kM = output_right.end();
    std::vector<int> parents(maxnodes, -1);

    double x = 0.0;
    while (j != jM || x < 1.0)
        {
            while (k != kM && k->second.r == x)
                {
                    assert(k->second.c < parents.size());
                    parents[k->second.c] = -1;
                    ++k;
                }
            while (j != jM && j->second.l == x)
                {
                    assert(j->second.c < parents.size());
                    assert(j->second.p < parents.size());
                    parents[k->second.c] = k->second.p;
                    ++j;
                }
            double right = 1.0;
            if (j != jM)
                {
                    right = std::min(j->second.l, right);
                }
            if (k != kM)
                {
                    right = std::min(right, k->second.r);
                }
            x = right;
        }
}

#endif
