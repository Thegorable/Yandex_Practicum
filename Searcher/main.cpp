#include <cassert>
#include <cstdlib>
#include<algorithm>

template <typename Type>
class ArrayPtr {
public:
    
    ArrayPtr() = default;

    explicit ArrayPtr(size_t size) {
        if (size) {
            raw_ptr_ = new Type[size];
        }
    }

    explicit ArrayPtr(Type* raw_ptr) noexcept : raw_ptr_(raw_ptr) {}
 
    ArrayPtr(const ArrayPtr&) = delete;

    ArrayPtr& operator=(const ArrayPtr&) = delete;
  
    [[nodiscard]] Type* Release() noexcept {
        Type* temp = raw_ptr_;
        raw_ptr_ = nullptr;
        return temp;
    }

    Type& operator[](size_t index) noexcept {
        return raw_ptr_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        return raw_ptr_[index];
    }

    explicit operator bool() const {
        return (bool)raw_ptr_;
    }

    Type* Get() const noexcept {
        return raw_ptr_;
    }

    void swap(ArrayPtr& other) noexcept {
        std::swap(other.raw_ptr_, raw_ptr_);
    }

    ~ArrayPtr() {
        delete[] raw_ptr_;
        raw_ptr_ = nullptr;
    }

private:
    Type* raw_ptr_ = nullptr;
};

int main() {
    ArrayPtr<int> numbers(10);
    const auto& const_numbers = numbers;

    numbers[2] = 42;
    assert(const_numbers[2] == 42);
    assert(&const_numbers[2] == &numbers[2]);

    assert(numbers.Get() == &numbers[0]);

    ArrayPtr<int> numbers_2(5);
    numbers_2[2] = 43;

    numbers.swap(numbers_2);

    assert(numbers_2[2] == 42);
    assert(numbers[2] == 43);
}