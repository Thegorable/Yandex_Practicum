#pragma once
#include"Preprocess.h"
#include <regex>

using namespace std;
namespace fs = std::filesystem;

fs::path operator""_p(const char* data, std::size_t sz) {
    return fs::path(data, data + sz);
}

string GetFileContents(string file) {
    ifstream stream(file);
    return { (istreambuf_iterator<char>(stream)), istreambuf_iterator<char>() };
}

string GetFileContents(ifstream& stream) {
    return { (istreambuf_iterator<char>(stream)), istreambuf_iterator<char>() };
}

string GetUnknownMsg(string file_name, string source_file, int line) {
    return "unknown include file " + file_name + " at file " + source_file + " at line "s + to_string(line);
}

fs::path FindFileViaPath(const fs::path& path, const string& file_name) {
    fs::path new_path = path / fs::path(file_name);
    if (fs::directory_entry(new_path).exists()) {
        return new_path;
    }
    
    for (const auto& dir_entry : fs::directory_iterator(path)) {
        if (dir_entry.is_directory()) {
            new_path = move(FindFileViaPath(dir_entry.path(), file_name));
            if (!new_path.empty()) {
                return new_path;
            }
        }
        if (dir_entry.path().filename().string() == file_name) {
            return dir_entry.path();
        }
    }
    return {};
}

fs::path FindFileViaPaths(const vector<fs::path>& dirs, const string& file_name) {
    fs::path file_path;
    for (const auto& dir : dirs) {
        if (!fs::directory_entry(dir).exists()) {
            return {};
        }

        file_path = FindFileViaPath(dir, file_name);
        if (!file_path.empty()) {
            return file_path;
        }
    }

    return file_path;
}

vector <TextIncludes> ParseIncludes(const string& str) {
    vector <TextIncludes> includes;
    regex include_reg(R"(#\s*include\s*(["|<])([^"<>]*)["|>])");

    auto it_begin = sregex_iterator(str.begin(), str.end(), include_reg);
    auto it_end = sregex_iterator();

    for (auto& it = it_begin; it != it_end; it++) {
        IncludeType cur_include_type;
        (*it)[1].str() == "<" ? cur_include_type = IncludeType::Directoryes : cur_include_type = IncludeType::Local;
        includes.emplace_back(TextIncludes{ cur_include_type, (*it)[0].str(), (*it)[2].str() });

        size_t position = it->position();
        includes.back().pos = count(str.begin(), str.begin() + position, '\n') + 1;
    }

    return includes;
}

string ReplaceIncludes(const fs::path& file_path, const vector<fs::path>& dirs, ErrorData& error_data) {
    string file_content = GetFileContents(file_path.string());
    vector <TextIncludes> includes_files = ParseIncludes(file_content);
    fs::path file_dir = file_path.parent_path();

    for (const auto& include : includes_files) {
        fs::path sub_file_path;

        if (include.type_ == IncludeType::Local) {
            sub_file_path = FindFileViaPath(file_dir, include.file_name_);
        }
        if (include.type_ == IncludeType::Directoryes || sub_file_path.empty()) {
            sub_file_path = FindFileViaPaths(dirs, include.file_name_);
        }

        string sub_file_content;
        if (!sub_file_path.empty()) {
            sub_file_content = ReplaceIncludes(sub_file_path, dirs, error_data);
        }
        else {
            error_data.unknown_file_ = include.file_name_;
            error_data.source_file_ = file_path.string();
            error_data.line_ = include.pos;
            regex replace_ex(include.full_include_ + R"((.|\s)*)");
            file_content = regex_replace(file_content, replace_ex, "");
            break;
        }

        regex replace_ex(include.full_include_);
        smatch m;

        if (regex_search(file_content, m, replace_ex)) {
            size_t pos = m.position();
            size_t size = m.length();

            if (file_content[pos + size] == '\n' && sub_file_content.back() == '\n') {
                sub_file_content.pop_back();
            }

            file_content.replace(pos, size, sub_file_content);
        }
        else {
            throw runtime_error("Usucsessful matching"s);
        }
    }

    return file_content;
}


bool Preprocess(const fs::path& in_file, const fs::path& out_file, const vector<fs::path>& include_directories) {
    ifstream in(in_file);
    if (!in.is_open()) {
        return false;
    }

    ErrorData error;
    string file_cont = ReplaceIncludes(in_file, include_directories, error);

    ofstream out(out_file);
    out << file_cont;
    out.close();

    if (error.line_) {
        cout << GetUnknownMsg(error.unknown_file_, error.source_file_, error.line_) << '\n';
        return false;
    }

    return true;
}