#include <algorithm>
#include <cassert>
#include <vector>

using namespace std;

template <typename T>
class PtrVector {
public:
    PtrVector() = default;

    PtrVector(const PtrVector& other) {
        items_.reserve(other.size());
        for (const auto& ptr : other.GetItems()) {
            if (ptr) {
                items_.push_back(new T(*ptr));
            }
            else {
                items_.push_back(nullptr);
            }
        }
    }

    ~PtrVector() {
        for (auto& ptr : items_) {
            delete ptr;
        }
        items_.clear();
    }

    vector<T*>& GetItems() noexcept {
        return items_;
    }

    vector<T*> const& GetItems() const noexcept {
        return items_;
    }

    inline size_t size() const noexcept {
        return items_.size();
    }

private:
    vector<T*> items_;
};


int main() {
    struct DeletionSpy {
        explicit DeletionSpy(bool& is_deleted)
            : is_deleted_(is_deleted) {
        }
        ~DeletionSpy() {
            is_deleted_ = true;
        }
        bool& is_deleted_;
    };

    {
        bool spy1_is_deleted = false;
        DeletionSpy* ptr1 = new DeletionSpy(spy1_is_deleted);
        {
            PtrVector<DeletionSpy> ptr_vector;
            ptr_vector.GetItems().push_back(ptr1);
            assert(!spy1_is_deleted);

            const auto& const_ptr_vector_ref(ptr_vector);
            assert(&const_ptr_vector_ref.GetItems() == &ptr_vector.GetItems());
        }
        assert(spy1_is_deleted);
    }

    struct CopyingSpy {
        explicit CopyingSpy(int& copy_count)
            : copy_count_(copy_count) {
        }
        CopyingSpy(const CopyingSpy& rhs)
            : copy_count_(rhs.copy_count_)
        {
            ++copy_count_;
        }
        int& copy_count_;
    };

    {
        vector<int> copy_counters(10);

        PtrVector<CopyingSpy> ptr_vector;
        for (auto& counter : copy_counters) {
            ptr_vector.GetItems().push_back(new CopyingSpy(counter));
        }
        ptr_vector.GetItems().push_back(nullptr);

        auto ptr_vector_copy(ptr_vector);
        assert(ptr_vector_copy.GetItems().size() == ptr_vector.GetItems().size());

        assert(ptr_vector_copy.GetItems() != ptr_vector.GetItems());
        assert(ptr_vector_copy.GetItems().back() == nullptr);
        assert(all_of(copy_counters.begin(), copy_counters.end(), [](int counter) {
            return counter == 1;
            }));
    }

    return 0;
}