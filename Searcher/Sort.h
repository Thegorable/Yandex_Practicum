#pragma once

template <typename it>
void MergeSort(it range_begin, it range_end) {
    size_t size = distance(range_begin, range_end);

    if (size > 1) {
        vector sorted_array(range_begin, range_end);

        auto start_1 = sorted_array.begin();
        const auto end = sorted_array.end();
        auto start_2 = start_1 + (size / 2);
        const auto mid = start_2;

        if (size > 2) {
            MergeSort(start_1, start_2);
            MergeSort(start_2, end);
        }

        while (start_1 != mid && start_2 != end) {
            if (*start_1 > *start_2) {
                *range_begin++ = move(*start_2++);
            }
            else {
                *range_begin++ = move(*start_1++);
            }
        }

        if (start_1 == mid) {
            while (start_2 != end) {
                *range_begin++ = *(start_2++);
            }
        }
        else if (start_2 == end) {
            while (start_1 != mid) {
                *range_begin++ = *(start_1++);
            }
        }
    }
}