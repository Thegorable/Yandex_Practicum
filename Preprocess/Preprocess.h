#pragma once
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

struct ErrorData {
    string unknown_file_;
    string source_file_;
    int line_ = 0;
};

enum class IncludeType { Local, Directoryes };

fs::path operator""_p(const char* data, std::size_t sz);

string GetFileContents(string file);

string GetFileContents(ifstream& stream);

string GetUnknownMsg(string file_name, string source_file, int line);

fs::path FindFileViaPath(const fs::path& path, const string& file_name);

fs::path FindFileViaPaths(const vector<fs::path>& dirs, const string& file_name);

struct TextIncludes {
    TextIncludes(IncludeType type, string&& full_include, string&& file_name) :
        type_(type),
        full_include_(move(full_include)),
        file_name_(move(file_name)) {}

    IncludeType type_ = IncludeType::Local;
    string full_include_;
    string file_name_;
    int pos = 0;
};

vector<TextIncludes> ParseIncludes(const string& str);

string ReplaceIncludes(const fs::path& file_path, const vector<fs::path>& dirs, ErrorData& error_data);

bool Preprocess(const fs::path& in_file, const fs::path& out_file, const vector<fs::path>& include_directories);