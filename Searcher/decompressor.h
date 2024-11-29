#pragma once
#include <fstream>
#include <iostream>
#include <string>

inline bool DecodeRLE(const std::string& src_name, const std::string& dst_name) {
    using namespace std;

    ifstream in(src_name, ios::binary);
    if (!in) {
        return false;
    }

    ofstream out(dst_name, ios::binary);
    if (!out) {
        return false;
    }

    do {
        char buff[1024];
        in.read(buff, sizeof buff);
        size_t read = in.gcount();

        for (size_t i = 0; i < read;) {
            unsigned char zero = buff[i++];
            int count = (zero >> 1) + 1;
            
            if (zero % 2) {
                string str_temp(count, buff[i]);
                out.write(str_temp.data(), count);
                i++;
            }
            else {
                out.write(&buff[i], count);
                i += count;
            }
        }
    } while (in);

    in.close();
    out.close();

    return true;
}