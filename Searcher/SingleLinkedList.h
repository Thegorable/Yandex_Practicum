#include <cassert>
#include <cstddef>
#include <string>
#include <utility>
#include <iostream>
#include <stdarg.h>
#include <vector>
#include <algorithm>

template <typename Type>
class SingleLinkedList {
private:

    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value_(val)
            , next_node_ptr_(next) {
        }
        Type value_{};
        Node* next_node_ptr_ = nullptr;
    };

    template <typename value_Type>
    class BasicIterator {
        friend class SingleLinkedList;

        explicit BasicIterator(Node* node) : node_ptr_(node) {}
        explicit BasicIterator(const Node* node) : node_ptr_((Node*)node) {}

    public:

        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = value_Type*;
        using reference = value_Type&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other) noexcept : node_ptr_(other.node_ptr_) {}

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ptr_ == rhs.node_ptr_;
        }

        bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ptr_ != rhs.node_ptr_;
        }

        bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return node_ptr_ == rhs.node_ptr_;
        }

        bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return node_ptr_ != rhs.node_ptr_;
        }

        BasicIterator& operator++() noexcept {
            node_ptr_ = node_ptr_->next_node_ptr_;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            auto temp = node_ptr_;
            node_ptr_ = node_ptr_->next_node_ptr_;
            return BasicIterator(temp);
        }

        reference operator*() const noexcept {
            return node_ptr_->value_;
        }

        pointer operator->() const noexcept {
            return &(node_ptr_->value_);
        }

    private:
        Node* node_ptr_ = nullptr;

#ifdef _DEBUG
        friend void Test_SingleLinkedList_EraseAfter();
#endif 
    };

public:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    SingleLinkedList() : head_({}), size_(0) {}

    template<typename iterator>
    SingleLinkedList(iterator start, iterator stop) {
        if (start != stop) {
            PushFront(*start);
            auto self_it = begin();
            for (auto it = ++start; it != stop; it++) {
                InsertAfter(self_it, *it);
                self_it++;
            }
        }
    }

    SingleLinkedList(std::initializer_list<Type> value_s) : 
        SingleLinkedList(value_s.begin(), value_s.end()) {}

    SingleLinkedList(const SingleLinkedList& other) :
        SingleLinkedList(other.begin(), other.end()) {}

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        SingleLinkedList copy(rhs);
        this->swap(copy);
        return *this;
    }

    Iterator InsertAfter(ConstIterator it, const Type& value) {
        if (size_ && it.node_ptr_ != &head_) {
            Node* new_node_ptr = new Node(value, nullptr);
            InsertAfterExistingNode(new_node_ptr, it.node_ptr_);
            size_++;
            return Iterator( new_node_ptr );
        }

        PushFront(value);
        return(begin());
    }

    Iterator EraseAfter(ConstIterator it) noexcept { 
        if (it.node_ptr_ && it.node_ptr_->next_node_ptr_)
        {
            if (size_ && it.node_ptr_ != &head_) {
                Node* next_node = it.node_ptr_->next_node_ptr_->next_node_ptr_;
                delete it.node_ptr_->next_node_ptr_;
                it.node_ptr_->next_node_ptr_ = next_node;
                size_--;
                return Iterator(next_node);
            }

            PopFront();
            return(begin());
        }

        return end();
    }

    void swap(SingleLinkedList& other) noexcept {
        std::swap(head_.next_node_ptr_, other.head_.next_node_ptr_);
        std::swap(size_, other.size_);
    }

    void PushFront(const Type& value) {
        head_.next_node_ptr_ = new Node(value, head_.next_node_ptr_);
        size_++;
    }

    void PopFront() noexcept {
        if (size_) {
            Node* second = head_.next_node_ptr_->next_node_ptr_;
            delete head_.next_node_ptr_;
            head_.next_node_ptr_ = second;
            size_--;
        }
    }

    size_t GetSize() const noexcept {
        return size_;
    }

    bool IsEmpty() const noexcept {
        return !size_;
    }

    void Clear() noexcept {
        while (head_.next_node_ptr_ != nullptr) {
            Node* to_delete = head_.next_node_ptr_;
            head_.next_node_ptr_ = to_delete->next_node_ptr_;
            delete to_delete;
            to_delete = nullptr;
            size_--;
        }
    }

    Iterator begin() noexcept {
        return Iterator(head_.next_node_ptr_);
    }

    Iterator end() noexcept {
        return Iterator();
    }

    ConstIterator begin() const noexcept {
        return ConstIterator(head_.next_node_ptr_);
    }

    ConstIterator end() const noexcept {
        return ConstIterator();
    }

    ConstIterator cbegin() const noexcept {
        return ConstIterator(head_.next_node_ptr_);
    }

    ConstIterator cend() const noexcept {
        return ConstIterator();
    }

    Iterator before_begin() noexcept {
        return Iterator(&head_);
    }
    ConstIterator cbefore_begin() const noexcept {
        return ConstIterator(&head_);
    }
    ConstIterator before_begin() const noexcept {
        return ConstIterator(&head_);
    }

    ~SingleLinkedList() {
        Clear();
    }

private:
    Node head_;
    size_t size_ = 0;

    void InsertAfterExistingNode(Node*& new_node_ptr, Node*& left_node_ptr) {
        if (left_node_ptr == nullptr) {
            throw std::invalid_argument("attempt to read nullptr");
        }
        Node* node_ptr_r = left_node_ptr->next_node_ptr_;
        left_node_ptr->next_node_ptr_ = new_node_ptr;
        new_node_ptr->next_node_ptr_ = node_ptr_r;
    }

#ifdef _DEBUG
    friend void Test_SingleLinkedList_Insert();
    friend void Test_SingleLinkedList_ListInitialize();
    friend void Test_SingleLinkedList_IteratorConstruct();
    friend void Test_SingleLinkedList_CopyConstruct();
    friend void Test_SingleLinkedList_method_swap();
    friend void Test_SingleLinkedList_global_swap();
    friend void Test_SingleLinkedList_Assignment();
    friend void Test_SingleLinkedList_PopFront();
    friend void Test_SingleLinkedList_EraseAfter();
#endif 

};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return (std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()));
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return (rhs < lhs);
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return (lhs < rhs) || (lhs == rhs);
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return (lhs > rhs) || (lhs == rhs);
}

#ifdef _DEBUG

void Test_SingleLinkedList_Insert() {
    SingleLinkedList<int> list;

    assert(!list.GetSize());
    assert(list.head_.next_node_ptr_ == nullptr);

    auto it = list.InsertAfter(list.begin(), 1);

    assert(list.GetSize() == 1);
    assert(list.head_.next_node_ptr_->value_ == 1);
    assert(list.head_.next_node_ptr_->value_ == *list.begin());
    assert(*it == 1);

    list.InsertAfter(list.begin(), 2);
    list.InsertAfter(list.begin(), 3);

    assert(list.GetSize() == 3);
    assert(list.head_.next_node_ptr_->value_ == 1);
    assert(list.head_.next_node_ptr_->next_node_ptr_->value_ == 3);
    assert(list.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ == 2);

    auto next_it = ++list.begin();
    list.InsertAfter(next_it, 4);
    assert(list.GetSize() == 4);
    assert(list.head_.next_node_ptr_->value_ == 1);
    assert(list.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ == 4);
    assert(list.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ == 2);

    auto last_it = list.begin();
    for (int i = 0; i < 3; i++) {
        last_it++;
    }
    list.InsertAfter(last_it, 5);
    assert(list.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ == 5);

    std::cout << "SingleLinkedList insert method passed succsessfully\n";
}

void Test_SingleLinkedList_ListInitialize() {
    {
        SingleLinkedList<int> list{ 1, 2, 4, 5, 6 };

        assert(list.GetSize() == 5);
        assert(list.head_.next_node_ptr_->value_ = 1);
        assert(list.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ = 4);
        assert(list.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ = 6);
    }
    {
        SingleLinkedList<int> list{};
        assert(!list.GetSize());
        assert(list.head_.next_node_ptr_ == nullptr);
    }

    std::cout << "SingleLinkedList list initilize constructor passed succsessfully\n";
}

void Test_SingleLinkedList_IteratorConstruct() {
    std::vector<int> v{ 1,2,3,4,5 };
    SingleLinkedList<int> l(v.begin(), v.end());

    assert(l.GetSize() == 5);
    assert(l.head_.next_node_ptr_->value_ == 1);
    assert(l.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ == 3);
    assert(l.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ == 5);

    std::cout << "SingleLinkedList contaier iterator constructor passed succsessfully\n";
}

void Test_SingleLinkedList_CopyConstruct() {
    {
        SingleLinkedList<int> l_1{ 1,2,3,4,5 };
        SingleLinkedList<int> l_2(l_1);

        assert(l_1.GetSize() == 5);
        assert(l_1.head_.next_node_ptr_->value_ == 1);
        assert(l_1.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ == 3);
        assert(l_1.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ == 5);

        assert(l_2.GetSize() == 5);
        assert(l_2.head_.next_node_ptr_->value_ == 1);
        assert(l_2.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ == 3);
        assert(l_2.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ == 5);

        assert(l_1.head_.next_node_ptr_->value_ == l_2.head_.next_node_ptr_->value_);
        assert(l_1.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ ==
            l_2.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_);

        assert(l_1.head_.next_node_ptr_ != l_2.head_.next_node_ptr_);
        assert(l_1.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_ !=
            l_2.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_);
    }
    
    {
        SingleLinkedList<int> l_1{ 1,2,3 };
        SingleLinkedList<int> l_2(l_1);

        assert(l_1.head_.next_node_ptr_->value_ == l_2.head_.next_node_ptr_->value_);
        assert(l_1.head_.next_node_ptr_->next_node_ptr_->value_ == l_2.head_.next_node_ptr_->next_node_ptr_->value_);
        assert(l_1.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ ==
            l_2.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_);
    }

    {
        SingleLinkedList<int> l_1{ 1,2,3 };
        SingleLinkedList<int> l_2(l_1);

        l_2.head_.next_node_ptr_->value_ = 5;
        assert(l_1.head_.next_node_ptr_->value_ == 1);
        l_2.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ = 10;
        assert(l_1.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ == 3);

        l_1.head_.next_node_ptr_->value_ = 50;
        assert(l_2.head_.next_node_ptr_->value_ == 5);
        l_1.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ = 100;
        assert(l_2.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ == 10);
    }

    {
        SingleLinkedList<int> l_1{};
        SingleLinkedList<int> l_2(l_1);

        assert(!l_2.GetSize());
        assert(l_2.head_.next_node_ptr_ == nullptr);
    }

    std::cout << "SingleLinkedList copy constructor passed succsessfully\n";
}

void Test_SingleLinkedList_method_swap() {
    SingleLinkedList<int> list_l{1,2,3,4,5};
    SingleLinkedList<int> list_r{8,9,10};

    int* one_ptr = &list_l.head_.next_node_ptr_->value_;
    int* three_ptr = &list_l.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_;
    int* five_ptr = &list_l.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->next_node_ptr_->next_node_ptr_->value_;
    
    int* eight_ptr = &list_r.head_.next_node_ptr_->value_;
    int* ten_ptr = &list_r.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_;

    list_l.swap(list_r);

    assert(list_l.GetSize() == 3);
    assert(list_r.GetSize() == 5);

    assert(*one_ptr == 1);
    assert(*three_ptr == 3);
    assert(*five_ptr == 5);
    assert(*eight_ptr == 8);
    assert(*ten_ptr == 10);

    assert(*list_l.begin() == 8);
    assert(*list_r.begin() == 1);

    assert(&list_l.head_.next_node_ptr_->value_ == eight_ptr);
    assert(list_l.head_.next_node_ptr_->value_ == 8);
    assert(&list_l.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ == ten_ptr);
    assert(list_l.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ == 10);
    
    
    assert(&list_r.head_.next_node_ptr_->value_ == one_ptr);
    assert(list_r.head_.next_node_ptr_->value_ == 1);
    assert(&list_r.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ == three_ptr);
    assert(list_r.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ == 3);
    assert(&list_r.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ == five_ptr);
    assert(list_r.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ == 5);

    std::cout << "SingleLinkedList swap method passed succsessfully\n";
}

void Test_SingleLinkedList_global_swap() {
    SingleLinkedList<int> list_l{ 1,2,3,4,5 };
    SingleLinkedList<int> list_r{ 8,9,10 };

    int* one_ptr = &list_l.head_.next_node_ptr_->value_;
    int* three_ptr = &list_l.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_;
    int* five_ptr = &list_l.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->next_node_ptr_->next_node_ptr_->value_;

    int* eight_ptr = &list_r.head_.next_node_ptr_->value_;
    int* ten_ptr = &list_r.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_;

    swap(list_l, list_r);

    assert(list_l.GetSize() == 3);
    assert(list_r.GetSize() == 5);

    assert(*one_ptr == 1);
    assert(*three_ptr == 3);
    assert(*five_ptr == 5);
    assert(*eight_ptr == 8);
    assert(*ten_ptr == 10);

    assert(*list_l.begin() == 8);
    assert(*list_r.begin() == 1);

    assert(&list_l.head_.next_node_ptr_->value_ == eight_ptr);
    assert(list_l.head_.next_node_ptr_->value_ == 8);
    assert(&list_l.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ == ten_ptr);
    assert(list_l.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ == 10);


    assert(&list_r.head_.next_node_ptr_->value_ == one_ptr);
    assert(list_r.head_.next_node_ptr_->value_ == 1);
    assert(&list_r.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ == three_ptr);
    assert(list_r.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ == 3);
    assert(&list_r.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ == five_ptr);
    assert(list_r.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ == 5);

    std::cout << "SingleLinkedList global swap function passed succsessfully\n";
}

void Test_SingleLinkedList_Compare_Equal() {
    {
        SingleLinkedList<int> l{ 1, 2, 3, 4, 5 };
        SingleLinkedList<int> r{ 1, 2, 3, 4, 5 };
        assert(l == r);
        assert(!(l != r));
    }

    {
        SingleLinkedList<int> l{};
        SingleLinkedList<int> r{};
        assert(l == r);
        assert(!(l != r));
    }

    {
        SingleLinkedList<int> l{ 1, 2, 3, 3, 5 };
        SingleLinkedList<int> r{ 1, 2, 3, 4, 5 };
        assert(!(l == r));
        assert(l != r);
    }

    {
        SingleLinkedList<int> l{ 1, 2, 3, 4};
        SingleLinkedList<int> r{ 1, 2, 3, 4, 5 };
        assert(!(l == r));
        assert(l != r);
    }

    {
        SingleLinkedList<int> l{0};
        SingleLinkedList<int> r{1};
        assert(!(l == r));
        assert(l != r);
    }

    std::cout << "SingleLinkedList compare operator == passed succsessfully\n";
}

void Test_SingleLinkedList_Assignment() {
    SingleLinkedList l{ 1,2,3 };

    int* one_ptr_l = &l.head_.next_node_ptr_->value_;
    int* two_ptr_l = &l.head_.next_node_ptr_->next_node_ptr_->value_;
    int* three_ptr_l = &l.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_;

    SingleLinkedList r = l;

    assert(r.GetSize() == 3);
    assert(l == r);

    int* one_ptr_r = &r.head_.next_node_ptr_->value_;
    int* two_ptr_r = &r.head_.next_node_ptr_->next_node_ptr_->value_;
    int* three_ptr_r = &r.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_;

    assert(l.head_.next_node_ptr_ != r.head_.next_node_ptr_);

    assert(one_ptr_l != one_ptr_r);
    assert(two_ptr_l != two_ptr_r);
    assert(three_ptr_l != three_ptr_r);

    assert(*one_ptr_l == *one_ptr_r);
    assert(*two_ptr_l == *two_ptr_r);
    assert(*three_ptr_l == *three_ptr_r);

    std::cout << "SingleLinkedList assignment operator = passed succsessfully\n";
}

void Test_SingleLinkedList_Less() {
    {
        SingleLinkedList<int> l = {};
        SingleLinkedList<int> r = { 1, 2, 3 };
        assert(l < r);
    }

    {
        SingleLinkedList<int> l = { 1, 2, 3 };
        SingleLinkedList<int> r = {};
        assert(!(l < r));
    }

    {
        SingleLinkedList<int> l = {};
        SingleLinkedList<int> r = {};
        assert(!(l < r));
    }

    {
        SingleLinkedList l = { 1, 2, 3 };
        SingleLinkedList r = { 2, 3, 4 };
        assert(l < r);
    }

    {
        SingleLinkedList l = { 2, 3, 4 };
        SingleLinkedList r = { 1, 2, 3 };
        assert(!(l < r));
    }

    {
        SingleLinkedList l = { 1, 2, 3 };
        SingleLinkedList r = { 1, 2, 3, 4 };
        assert(l < r);
    }

    {
        SingleLinkedList l = { 1, 2, 3 };
        SingleLinkedList r = { 1, 2, 3 };
        assert(!(l < r));
    }

    {
        SingleLinkedList l = { 1, 2, 3 };
        SingleLinkedList r = { 1, 3, 2 };
        assert(l < r);
    }

    {
        SingleLinkedList l = { 1, 2, 3 };
        SingleLinkedList r = { 1, 2 };
        assert(!(l < r));
    }

    {
        SingleLinkedList l = { 5 };
        SingleLinkedList r = { 10 };
        assert(l < r);
        assert(!(r < l));
    }

    {
        SingleLinkedList l = { 5 };
        SingleLinkedList r = { 5 };
        assert(!(l < r));
        assert(!(r < l));
    }

    std::cout << "SingleLinkedList compare operator < passed succsessfully\n";
}

void Test_SingleLinkedList_More() {
    {
        SingleLinkedList<int> l = {};
        SingleLinkedList<int> r = { 1, 2, 3 };
        assert(!(l > r));
    }

    {
        SingleLinkedList<int> l = { 1, 2, 3 };
        SingleLinkedList<int> r = {};
        assert(l > r);
    }

    {
        SingleLinkedList<int> l = {};
        SingleLinkedList<int> r = {};
        assert(!(l > r));
    }

    {
        SingleLinkedList l = { 1, 2, 3 };
        SingleLinkedList r = { 2, 3, 4 };
        assert(!(l > r));
    }

    {
        SingleLinkedList l = { 2, 3, 4 };
        SingleLinkedList r = { 1, 2, 3 };
        assert(l > r);
    }

    {
        SingleLinkedList l = { 1, 2, 3 };
        SingleLinkedList r = { 1, 2, 3, 4 };
        assert(!(l > r));
    }

    {
        SingleLinkedList l = { 1, 2, 3 };
        SingleLinkedList r = { 1, 2, 3 };
        assert(!(l > r));
    }

    {
        SingleLinkedList l = { 1, 3, 2 };
        SingleLinkedList r = { 1, 2, 3 };
        assert(l > r);
    }

    {
        SingleLinkedList l = { 1, 2, 3 };
        SingleLinkedList r = { 1, 2 };
        assert(l > r);
    }

    {
        SingleLinkedList l = { 10 };
        SingleLinkedList r = { 5 };
        assert(l > r);
        assert(!(r > l));
    }

    {
        SingleLinkedList l = { 5 };
        SingleLinkedList r = { 5 };
        assert(!(l > r));
        assert(!(r > l));
    }
}

void Test_SingleLinkedList_PopFront() {
    {
        SingleLinkedList<int> empty_list;
        empty_list.PopFront();
        assert(empty_list.GetSize() == 0);
        assert(empty_list.head_.next_node_ptr_ == nullptr);
    }

    {
        SingleLinkedList<int> single_element_list{ 42 };
        single_element_list.PopFront();
        assert(single_element_list.GetSize() == 0);
        assert(single_element_list.head_.next_node_ptr_ == nullptr);
    }

    {
        SingleLinkedList<int> multi_element_list{ 1, 2, 3, 4, 5 };
        multi_element_list.PopFront();
        assert(multi_element_list.GetSize() == 4);
        assert(multi_element_list.head_.next_node_ptr_->value_ == 2);

        auto node = multi_element_list.head_.next_node_ptr_;
        assert(node->value_ == 2);
        node = node->next_node_ptr_;
        assert(node->value_ == 3);
        node = node->next_node_ptr_;
        assert(node->value_ == 4);
        node = node->next_node_ptr_;
        assert(node->value_ == 5);
        assert(node->next_node_ptr_ == nullptr);
    }

    {
        SingleLinkedList<int> repeated_pop_list{ 10, 20, 30 };
        repeated_pop_list.PopFront();
        assert(repeated_pop_list.GetSize() == 2);
        assert(repeated_pop_list.head_.next_node_ptr_->value_ == 20);

        repeated_pop_list.PopFront();
        assert(repeated_pop_list.GetSize() == 1);
        assert(repeated_pop_list.head_.next_node_ptr_->value_ == 30);

        repeated_pop_list.PopFront();
        assert(repeated_pop_list.GetSize() == 0);
        assert(repeated_pop_list.head_.next_node_ptr_ == nullptr);
    }

    std::cout << "SingleLinkedList PopFront passed successfully\n";
}

void Test_SingleLinkedList_EraseAfter() {
    
    {
        SingleLinkedList<int> list{ 1, 2, 3, 4, 5 };
        auto it = list.begin();
        auto next_it = list.EraseAfter(it);
        assert(next_it.node_ptr_ == list.head_.next_node_ptr_->next_node_ptr_);
        assert(list.GetSize() == 4);
        assert(list.head_.next_node_ptr_->value_ == 1);
        assert(list.head_.next_node_ptr_->next_node_ptr_->value_ == 3);
    }

    
    {
        SingleLinkedList<int> list{ 10, 20, 30 };
        auto it = list.begin();
        auto next_it = list.EraseAfter(it);
        assert(next_it.node_ptr_ == list.head_.next_node_ptr_->next_node_ptr_);
        assert(list.GetSize() == 2);
        assert(list.head_.next_node_ptr_->value_ == 10);
        assert(list.head_.next_node_ptr_->next_node_ptr_->value_ == 30);
    }

    
    {
        SingleLinkedList<int> list{ 7, 8 };
        auto it = list.begin();
        ++it;
        auto next_it = list.EraseAfter(it);
        assert(next_it.node_ptr_ == nullptr);
        assert(list.GetSize() == 2);
        assert(list.head_.next_node_ptr_->value_ == 7);
        assert(list.head_.next_node_ptr_->next_node_ptr_->value_ == 8);
    }

    
    {
        SingleLinkedList<int> list{ 42, 84 };
        auto it = list.begin();
        auto next_it = list.EraseAfter(it);
        assert(next_it.node_ptr_ == nullptr);
        assert(list.GetSize() == 1);
        assert(list.head_.next_node_ptr_->value_ == 42);
        assert(list.head_.next_node_ptr_->next_node_ptr_ == nullptr);
    }

    
    {
        SingleLinkedList<int> list;
        auto it = list.begin();
        auto next_it = list.EraseAfter(it);
        assert(next_it.node_ptr_ == nullptr);
        assert(list.GetSize() == 0);
        assert(list.head_.next_node_ptr_ == nullptr);
    }

    
    {
        SingleLinkedList<int> list{ 1, 2, 3, 4, 5 };
        auto it = list.begin();
        it = list.EraseAfter(it);
        assert(it.node_ptr_ == list.head_.next_node_ptr_->next_node_ptr_);
        assert(list.GetSize() == 4);
        it = list.EraseAfter(it);
        assert(it.node_ptr_ == list.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_);
        assert(list.GetSize() == 3);
        it = list.EraseAfter(it);
        assert(it.node_ptr_ == nullptr);
        assert(list.GetSize() == 3);
        assert(list.head_.next_node_ptr_->value_ == 1);
        assert(list.head_.next_node_ptr_->next_node_ptr_->next_node_ptr_->value_ == 5);
    }

    std::cout << "SingleLinkedList EraseAfter passed successfully\n";
}

void Test4() {
    struct DeletionSpy {
        ~DeletionSpy() {
            if (deletion_counter_ptr) {
                ++(*deletion_counter_ptr);
            }
        }
        int* deletion_counter_ptr = nullptr;
    };

    
    {
        SingleLinkedList<int> numbers{ 3, 14, 15, 92, 6 };
        numbers.PopFront();
        assert((numbers == SingleLinkedList<int>{14, 15, 92, 6}));

        SingleLinkedList<DeletionSpy> list;
        list.PushFront(DeletionSpy{});
        int deletion_counter = 0;
        list.begin()->deletion_counter_ptr = &deletion_counter;
        assert(deletion_counter == 0);
        list.PopFront();
        assert(deletion_counter == 1);
    }

    
    {
        SingleLinkedList<int> empty_list;
        const auto& const_empty_list = empty_list;
        assert(empty_list.before_begin() == empty_list.cbefore_begin());
        assert(++empty_list.before_begin() == empty_list.begin());
        assert(++empty_list.cbefore_begin() == const_empty_list.begin());

        SingleLinkedList<int> numbers{ 1, 2, 3, 4 };
        const auto& const_numbers = numbers;
        assert(numbers.before_begin() == numbers.cbefore_begin());
        assert(++numbers.before_begin() == numbers.begin());
        assert(++numbers.cbefore_begin() == const_numbers.begin());
    }

    
    {  
        {
            SingleLinkedList<int> lst;
            const auto inserted_item_pos = lst.InsertAfter(lst.before_begin(), 123);
            assert((lst == SingleLinkedList<int>{123}));
            assert(inserted_item_pos == lst.begin());
            assert(*inserted_item_pos == 123);
        }

        
        {
            SingleLinkedList<int> lst{ 1, 2, 3 };
            auto inserted_item_pos = lst.InsertAfter(lst.before_begin(), 123);

            assert(inserted_item_pos == lst.begin());
            assert(inserted_item_pos != lst.end());
            assert(*inserted_item_pos == 123);
            assert((lst == SingleLinkedList<int>{123, 1, 2, 3}));

            inserted_item_pos = lst.InsertAfter(lst.begin(), 555);
            assert(++SingleLinkedList<int>::Iterator(lst.begin()) == inserted_item_pos);
            assert(*inserted_item_pos == 555);
            assert((lst == SingleLinkedList<int>{123, 555, 1, 2, 3}));
        };
    }

    
    struct ThrowOnCopy {
        ThrowOnCopy() = default;
        explicit ThrowOnCopy(int& copy_counter) noexcept
            : countdown_ptr(&copy_counter) {
        }
        ThrowOnCopy(const ThrowOnCopy& other)
            : countdown_ptr(other.countdown_ptr)  
        {
            if (countdown_ptr) {
                if (*countdown_ptr == 0) {
                    throw std::bad_alloc();
                }
                else {
                    --(*countdown_ptr);
                }
            }
        }
        
        ThrowOnCopy& operator=(const ThrowOnCopy& rhs) = delete;
        
        
        int* countdown_ptr = nullptr;
    };

    
    {
        bool exception_was_thrown = false;
        for (int max_copy_counter = 10; max_copy_counter >= 0; --max_copy_counter) {
            SingleLinkedList<ThrowOnCopy> list{ ThrowOnCopy{}, ThrowOnCopy{}, ThrowOnCopy{} };
            try {
                int copy_counter = max_copy_counter;
                list.InsertAfter(list.cbegin(), ThrowOnCopy(copy_counter));
                assert(list.GetSize() == 4u);
            }
            catch (const std::bad_alloc&) {
                exception_was_thrown = true;
                assert(list.GetSize() == 3u);
                break;
            }
        }
        assert(exception_was_thrown);
    }

    {
        {
            SingleLinkedList<int> lst{ 1, 2, 3, 4 };
            const auto& const_lst = lst;
            const auto item_after_erased = lst.EraseAfter(const_lst.cbefore_begin());
            assert((lst == SingleLinkedList<int>{2, 3, 4}));
            assert(item_after_erased == lst.begin());
        }
        {
            SingleLinkedList<int> lst{ 1, 2, 3, 4 };
            const auto item_after_erased = lst.EraseAfter(lst.cbegin());
            assert((lst == SingleLinkedList<int>{1, 3, 4}));
            assert(item_after_erased == (++lst.begin()));
        }
        {
            SingleLinkedList<int> lst{ 1, 2, 3, 4 };
            const auto item_after_erased = lst.EraseAfter(++(++lst.cbegin()));
            assert((lst == SingleLinkedList<int>{1, 2, 3}));
            assert(item_after_erased == lst.end());
        }
        {
            SingleLinkedList<DeletionSpy> list{ DeletionSpy{}, DeletionSpy{}, DeletionSpy{} };
            auto after_begin = ++list.begin();
            int deletion_counter = 0;
            after_begin->deletion_counter_ptr = &deletion_counter;
            assert(deletion_counter == 0u);
            list.EraseAfter(list.cbegin());
            assert(deletion_counter == 1u);
        }
    }
}

#endif