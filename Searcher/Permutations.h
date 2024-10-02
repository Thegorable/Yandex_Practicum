#pragma once
#include <vector>
#include <string>

template<typename it>
void Permutation(const it start, it cur_start, const it end, std::vector<std::string>& permutations) {
    auto cur_it = cur_start;
    while (cur_it != end) {
        swap(*cur_start, *cur_it);
        if (cur_start + 1 != end) {
            Permutation(start, cur_start + 1, end, permutations);
        }
        else {
            permutations.push_back(PrintRangeToString(start, end));
        }
        swap(*cur_start, *cur_it);
        cur_it++;
    }
}

template<typename it>
std::vector<std::string> GetPermutations(it start, it end) {
    vector<string> permutations;
    Permutation(start, start, end, permutations);
    return permutations;
}