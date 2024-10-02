#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include "Searcher.h"
#include <iterator>

using namespace std;

set<int>::const_iterator FindNearestElement(const set<int>& numbers, int border) {
    auto not_less = numbers.lower_bound(border);
    
    if (not_less == numbers.begin()) {
        return not_less;
    }

    auto prev_it = next(not_less, -1);
    if (not_less == numbers.end()) {
        return prev_it;
    }

    if ((border - *prev_it) > ((*not_less - *prev_it) / 2)) {
        return not_less;
    }

    return prev_it;
}

//template <typename it>
//pair<it, it> FindStartsWith(it range_begin, it range_end, char prefix) {
//    string prefix_s(1, prefix);
//    it left = lower_bound(range_begin, range_end, prefix_s,
//        [](const string& l, const string& r) {return l[0] < r[0]; });
//
//    it right = upper_bound(left, range_end, prefix_s,
//        [](const string& l, const string& r) {return l[0] < r[0]; });
//
//    return { left, right };
//}

template <typename it>
pair<it, it> FindStartsWith(it range_begin, it range_end, string prefix) {
    it left = lower_bound(range_begin, range_end, prefix,
        [](const string& element, const string& value) {return element.compare(0, value.size(), value) < 0; });

    it right = upper_bound(left, range_end, prefix,
        [](const string& value, const string& element) {return element.compare(0, value.size(), value) > 0; });

    return { left, right };
}

int main() {
    const vector<string> sorted_strings = { "moscow", "motovilikha", "murmansk" };
    const auto mo_result = FindStartsWith(begin(sorted_strings), end(sorted_strings), "mo");
    for (auto it = mo_result.first; it != mo_result.second; ++it) {
        cout << *it << " ";
    }
    cout << endl;
    const auto mt_result = FindStartsWith(begin(sorted_strings), end(sorted_strings), "mt");
    cout << (mt_result.first - begin(sorted_strings)) << " " << (mt_result.second - begin(sorted_strings)) << endl;
    const auto na_result = FindStartsWith(begin(sorted_strings), end(sorted_strings), "na");
    cout << (na_result.first - begin(sorted_strings)) << " " << (na_result.second - begin(sorted_strings)) << endl;
    return 0;

    //string a = "motovilikha"s;
    //int comp = a.compare(0, 2,"mo"s);
}