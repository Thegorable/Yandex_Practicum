#pragma once
#include <new>
#include <vector>

using namespace std;

class Tentacle {
public:
    explicit Tentacle(int id) noexcept
        : id_(id) {
    }

    int GetId() const noexcept {
        return id_;
    }

    Tentacle* GetLinkedTentacle() const noexcept {
        return linked_tentacle_;
    }
    void LinkTo(Tentacle& tentacle) noexcept {
        linked_tentacle_ = &tentacle;
    }
    void Unlink() noexcept {
        linked_tentacle_ = nullptr;
    }

private:
    int id_ = 0;
    Tentacle* linked_tentacle_ = nullptr;
};

class Octopus {
public:
    Octopus()
        : Octopus(8) {
    }

    explicit Octopus(int num_tentacles) {
        ConstructTentacles(num_tentacles);
    }

    explicit Octopus(const Octopus& oct) {
        int size = oct.GetTentacleCount();
        ConstructTentacles(size);

        for (int id = 0; id < size; id++) {
            Tentacle* linked_t = oct.GetTentacle(id).GetLinkedTentacle();
            if (linked_t) {
                tentacles_[id]->LinkTo(*linked_t);
            }
        }
    }

    Octopus& operator =(const Octopus& oct_rhs) {
        if (oct_rhs.tentacles_ != this->tentacles_) {
            Octopus oct_copy(oct_rhs);
            this->tentacles_.swap(oct_copy.tentacles_);
        }
        return *this;
    }

    ~Octopus() {
        Cleanup();
    }

    Tentacle& AddTentacle() {
        Tentacle* t = nullptr;
        try {
            t = new Tentacle(static_cast<int> (tentacles_.size()) + 1);
            tentacles_.push_back(t);
            return *t;
        }
        catch (const std::bad_alloc&) {
            Cleanup();
            delete t;
            throw;
        }
    }

    void ConstructTentacles(int num_tentacles) {
        Tentacle* t = nullptr;
        try {
            for (int id = 1; id <= num_tentacles; ++id) {
                t = new Tentacle(id);
                tentacles_.push_back(t);
                t = nullptr;
            }
        }
        catch (const std::bad_alloc&) {
            Cleanup();
            delete t;
            throw;
        }
    }

    int GetTentacleCount() const noexcept {
        return static_cast<int>(tentacles_.size());
    }

    const Tentacle& GetTentacle(size_t index) const {
        return *tentacles_.at(index);
    }
    Tentacle& GetTentacle(size_t index) {
        return *tentacles_.at(index);
    }

private:
    void Cleanup() {
        for (Tentacle* t : tentacles_) {
            delete t;
        }
        tentacles_.clear();
    }

    std::vector<Tentacle*> tentacles_;
};