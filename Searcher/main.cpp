#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;


int Fibonacci(int i) {
    if (i <= 1) { return i;}
    return Fibonacci(i - 1) + Fibonacci(i - 2);
}

bool IsPowOfTwo(int i) {
    if (i > 0 && !(i & (i - 1))) { return true; }
    return false;
}

static int steps = -1;

class Tower {
public:
    Tower(int disks_num) {
        FillTower(disks_num);
    }

    int GetDisksNum() const {
        return static_cast<int>(disks_.size());
    }

    void SetDisks(int disks_num) {
        FillTower(disks_num);
    }

    int GetDisk(int id) const {
        if (id >= static_cast<int>(disks_.size())) {
            throw invalid_argument("The id disk " + to_string(id) + " doesn't exists in the tower");
        }
        return disks_[id];
    }

    void AddToTop(int disk) {
        int top_disk_num = static_cast<int>(disks_.size()) - 1;
        if (disks_.size() && disk >= disks_[top_disk_num]) {
            throw invalid_argument("Impossible to put big disk on small"s);
        }
        else {
            disks_.push_back(disk);
        }
    }

    int RemoveTopDisk() {
        if (!disks_.size()) {
            throw invalid_argument("Impossible to remove disk from empty tower"s);
        }
        int last_value = *(disks_.end() - 1);
        disks_.pop_back();
        return last_value;
    }


private:
    vector<int> disks_;

    void FillTower(int disks_num) {
        for (int i = disks_num; i > 0; i--) {
            disks_.push_back(i);
        }
    }
};

void PrintTowers(const vector<Tower>& towers) {
    int max_long = 0;
    
    for (const auto& tower : towers) {
        if (tower.GetDisksNum() > max_long) {
            max_long = tower.GetDisksNum();
        }
    }

    ++steps;
    cout << "Step: " << steps << '\n';
    for (int i = max_long - 1; i >= 0; --i) {
        for (const auto& tower : towers) {
            if (i >= tower.GetDisksNum()) {
                cout << '-' << '\t';
            }
            else {
                cout << tower.GetDisk(i) << '\t';
            }
        }
        cout << '\n';
    }
    cout << "\n\n"s;
}

enum class MoveDirections {OneToTwo, OneToThree, TwoToThree, TwoToOne, ThreeToOne, ThreeToTwo};

pair<int, int> DirectionToNumber(MoveDirections dir) {
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

    return {0, 0};
}

const vector<MoveDirections> GetInsideDirections(MoveDirections dir) {
    switch (dir)
    {
    case MoveDirections::OneToTwo:
        return vector({ MoveDirections::OneToThree, MoveDirections::OneToTwo, MoveDirections::ThreeToTwo });
        break;
    case MoveDirections::OneToThree:
        return vector({ MoveDirections::OneToTwo, MoveDirections::OneToThree, MoveDirections::TwoToThree });
        break;
    case MoveDirections::TwoToThree:
        return vector({ MoveDirections::TwoToOne, MoveDirections::TwoToThree, MoveDirections::OneToThree });
        break;
    case MoveDirections::TwoToOne:
        return vector({ MoveDirections::TwoToThree, MoveDirections::TwoToOne, MoveDirections::ThreeToOne });
        break;
    case MoveDirections::ThreeToOne:
        return vector({ MoveDirections::ThreeToTwo, MoveDirections::ThreeToOne, MoveDirections::TwoToOne });
        break;
    case MoveDirections::ThreeToTwo:
        return vector({ MoveDirections::ThreeToOne, MoveDirections::ThreeToTwo, MoveDirections::OneToTwo });
        break;
    }

    return {};
}

void MoveDisk(vector<Tower>& towers, MoveDirections direction) {
    if (towers.size() != 3) {
        throw invalid_argument("The towers count should be 3 to move disk");
    }
    int top_elem = towers[DirectionToNumber(direction).first].RemoveTopDisk();
    towers[DirectionToNumber(direction).second].AddToTop(top_elem);
}

void MoveDisks(vector<Tower>& towers, int disk_count, MoveDirections direction) {
    const vector<MoveDirections> directions = GetInsideDirections(direction);

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


void SolveHanoi(vector<Tower>& towers) {
    int disks_num = towers[0].GetDisksNum();
    MoveDisks(towers, disks_num, MoveDirections::OneToThree);
}

int main() {
    int towers_num = 3;
    int disks_num = 6;
    vector<Tower> towers;
    for (int i = 0; i < towers_num; ++i) {
        towers.push_back(0);
    }
    towers[0].SetDisks(disks_num);
    
    SolveHanoi(towers);
}