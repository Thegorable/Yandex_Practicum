#include<iostream>
#include<stdexcept>
#include<vector>
#include<string>

class Tower {
public:
    Tower(int disks_num);

    int GetDisksNum() const;

    void SetDisks(int disks_num);

    int GetDisk(int id) const;

    void AddToTop(int disk);

    int RemoveTopDisk();


private:
    std::vector<int> disks_;

    void FillTower(int disks_num);
};

Tower::Tower(int disks_num) {
    FillTower(disks_num);
}

int Tower::GetDisksNum() const {
    return static_cast<int>(disks_.size());
}

void Tower::SetDisks(int disks_num) {
    FillTower(disks_num);
}

int Tower::GetDisk(int id) const {
    if (id >= static_cast<int>(disks_.size())) {
        throw std::invalid_argument("The id disk " + std::to_string(id) + " doesn't exists in the tower");
    }
    return disks_[id];
}

void Tower::AddToTop(int disk) {
    int top_disk_num = static_cast<int>(disks_.size()) - 1;
    if (disks_.size() && disk >= disks_[top_disk_num]) {
        throw std::invalid_argument("Impossible to put big disk on small");
    }
    else {
        disks_.push_back(disk);
    }
}

int Tower::RemoveTopDisk() {
    if (!disks_.size()) {
        throw std::invalid_argument("Impossible to remove disk from empty tower");
    }
    int last_value = *(disks_.end() - 1);
    disks_.pop_back();
    return last_value;
}

void Tower::FillTower(int disks_num) {
    for (int i = disks_num; i > 0; i--) {
        disks_.push_back(i);
    }
}




void PrintTowers(const std::vector<Tower>& towers);

void PrintTowers(const std::vector<Tower>& towers) {
    int max_long = 0;

    for (const auto& tower : towers) {
        if (tower.GetDisksNum() > max_long) {
            max_long = tower.GetDisksNum();
        }
    }

    for (int i = max_long - 1; i >= 0; --i) {
        for (const auto& tower : towers) {
            if (i >= tower.GetDisksNum()) {
                std::cout << '-' << '\t';
            }
            else {
                std::cout << tower.GetDisk(i) << '\t';
            }
        }
        std::cout << '\n';
    }
    std::cout << "\n\n";
}

enum class MoveDirections { OneToTwo, OneToThree, TwoToThree, TwoToOne, ThreeToOne, ThreeToTwo };

std::pair<int, int> DirectionToNumber(MoveDirections dir);

std::pair<int, int> DirectionToNumber(MoveDirections dir) {
    switch (dir)
    {
    case MoveDirections::OneToTwo:
        return { 0, 1 };
        break;
    case MoveDirections::OneToThree:
        return { 0, 2 };
        break;
    case MoveDirections::TwoToThree:
        return { 1, 2 };
        break;
    case MoveDirections::TwoToOne:
        return { 1, 0 };
        break;
    case MoveDirections::ThreeToOne:
        return { 2, 0 };
        break;
    case MoveDirections::ThreeToTwo:
        return { 2, 1 };
        break;
    default:
        break;
    }

    return { 0, 0 };
}

const std::vector<MoveDirections> GetInsideDirections(MoveDirections dir);

const std::vector<MoveDirections> GetInsideDirections(MoveDirections dir) {
    switch (dir)
    {
    case MoveDirections::OneToTwo:
        return std::vector({ MoveDirections::OneToThree, MoveDirections::OneToTwo, MoveDirections::ThreeToTwo });
        break;
    case MoveDirections::OneToThree:
        return std::vector({ MoveDirections::OneToTwo, MoveDirections::OneToThree, MoveDirections::TwoToThree });
        break;
    case MoveDirections::TwoToThree:
        return std::vector({ MoveDirections::TwoToOne, MoveDirections::TwoToThree, MoveDirections::OneToThree });
        break;
    case MoveDirections::TwoToOne:
        return std::vector({ MoveDirections::TwoToThree, MoveDirections::TwoToOne, MoveDirections::ThreeToOne });
        break;
    case MoveDirections::ThreeToOne:
        return std::vector({ MoveDirections::ThreeToTwo, MoveDirections::ThreeToOne, MoveDirections::TwoToOne });
        break;
    case MoveDirections::ThreeToTwo:
        return std::vector({ MoveDirections::ThreeToOne, MoveDirections::ThreeToTwo, MoveDirections::OneToTwo });
        break;
    }

    return {};
}

void MoveDisk(std::vector<Tower>& towers, MoveDirections direction);

void MoveDisk(std::vector<Tower>& towers, MoveDirections direction) {
    if (towers.size() != 3) {
        throw std::invalid_argument("The towers count should be 3 to move disk");
    }
    int top_elem = towers[DirectionToNumber(direction).first].RemoveTopDisk();
    towers[DirectionToNumber(direction).second].AddToTop(top_elem);
}

void MoveDisks(std::vector<Tower>& towers, int disk_count, MoveDirections direction);

void MoveDisks(std::vector<Tower>& towers, int disk_count, MoveDirections direction) {
    const std::vector<MoveDirections> directions = GetInsideDirections(direction);

    switch (disk_count)
    {
    case 0:
        break;

    case 1:
        MoveDisk(towers, direction);
        break;

    case 2:
        for (const auto& direction : directions) {
            MoveDisk(towers, direction);
        }
        break;

    default:
    {
        bool is_odd = true;
        if (disk_count > 2) { disk_count--; }
        for (const auto& algo : directions) {
            if (is_odd) {
                MoveDisks(towers, disk_count, algo);
            }
            else {
                MoveDisks(towers, 1, algo);
            }
            is_odd = !is_odd;
        }
    }
    }
}

void SolveThreeHanoi(std::vector<Tower>& towers);

void SolveThreeHanoi(std::vector<Tower>& towers) {
    int disks_num = towers[0].GetDisksNum();
    MoveDisks(towers, disks_num, MoveDirections::OneToThree);
}