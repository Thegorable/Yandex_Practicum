#pragma once
#include <iostream>
#include <vector>

template <typename it>
class Page {
public:
    Page() = default;
    Page(it start, it end);
    it begin() const;
    it end() const;

private:
    it doc_start_it_;
    it doc_end_it_;
};

template <typename it>
std::ostream& operator<<(std::ostream& os, const Page<it>& page) {
    for (const auto& doc : page) {
        os << doc;
    }
    return os;
}

template <typename it>
class Paginator {
public:
    Paginator() = default;
    Paginator(it start, it end, size_t page_size);

    auto begin() const;
    auto end() const;
    size_t size() const;

    void MakePages(it start, it end, size_t page_size);

    ~Paginator() {};

private:
    it MoveIterator(it iterator, it end, size_t steps);

    std::vector<Page<it>> pages_;
};


template<typename it>
Paginator<it>::Paginator(it start, it end, size_t page_size) {
    MakePages(start, end, page_size);
}

template<typename it>
inline auto Paginator<it>::begin() const {
    return pages_.begin();
}

template<typename it>
inline auto Paginator<it>::end() const {
    return pages_.end();
}

template<typename it>
inline size_t Paginator<it>::size() const {
    return pages_.size();
}

template<typename it>
inline void Paginator<it>::MakePages(it start, it end, size_t page_size) {
    if (!pages_.empty()) {
        pages_.clear();
    }
    it end_page = MoveIterator(start, end, page_size);

    do {
        pages_.push_back(Page(start, end_page));
        start = end_page;
        end_page = MoveIterator(start, end, page_size);
    } while (start != end);
}

template<typename it>
inline it Paginator<it>::MoveIterator(it iterator, it end, size_t steps) {
    for (int step = 0; step < steps; step++) {
        if (iterator == end) {
            break;
        }
        iterator++;
    }

    return iterator;
}

template<typename it>
inline Page<it>::Page(it start, it end) :
    doc_start_it_(start), doc_end_it_(end) {}

template<typename it>
inline it Page<it>::begin() const {
    return doc_start_it_;
}

template<typename it>
inline it Page<it>::Page::end() const {
    return doc_end_it_;
}

template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}