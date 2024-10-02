#pragma once
#include <algorithm>

using namespace std;

template<typename iterator>
void PrintRange(const iterator& it_l, const iterator& it_r) {
    bool is_first = true;
    for (auto it = it_l; it != it_r; ++it) {
        if (!is_first) {
            cout << ' ';
        }
        else {
            is_first = false;
        }
        cout << *it;
    }
    cout << '\n';
}

template <typename It>
string PrintRangeToString(It range_begin, It range_end) {
    ostringstream out;
    for (It it = range_begin; it != range_end; ++it) {
        out << *it << " "s;
    }
    out << endl;
    return out.str();
}

template<typename container, class element>
void FindAndPrint(const container& cont, const element& elem) {
    const auto turn_it = find_if(cont.begin(), cont.end(), [&elem](const element& elem_) { return elem == elem_; });
    PrintRange(cont.begin(), turn_it);
    PrintRange(turn_it, cont.end());
}