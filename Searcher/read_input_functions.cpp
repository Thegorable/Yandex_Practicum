#include <iostream>
#include "read_input_functions.h"

using namespace std;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result;
    cin >> result;
    ReadLine();
    return result;
}

vector<int> ReadNumbers(int count) {
    vector<int> numbers;
    int num = 0;
    for (int i = 0; i < count; i++) {
        cin >> num;
        numbers.push_back(num);
    }

    cin >> ws;
    return numbers;
}