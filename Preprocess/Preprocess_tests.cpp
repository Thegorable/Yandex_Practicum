#pragma once
#include"Preprocess_tests.h"
#include"Preprocess.h"
#include"regex"
#include"cassert"

void MakeTestDirs() {
    error_code err;
    filesystem::remove_all("sources"_p, err);
    filesystem::create_directories("sources"_p / "include2"_p / "lib"_p, err);
    filesystem::create_directories("sources"_p / "include1"_p, err);
    filesystem::create_directories("sources"_p / "dir1"_p / "subdir"_p, err);

    {
        ofstream file("sources/a.cpp");
        file << "// this comment before include\n"
            "#include \"dir1/b.h\"\n"
            "// text between b.h and c.h\n"
            "#include \"dir1/d.h\"\n"
            "\n"
            "int SayHello() {\n"
            "    cout << \"hello, world!\" << endl;\n"
            "#   include<dummy.txt>\n"
            "}\n"s;
    }
    {
        ofstream file("sources/dir1/b.h");
        file << "// text from b.h before include\n"
            "#include \"subdir/c.h\"\n"
            "// text from b.h after include"s;
    }
    {
        ofstream file("sources/dir1/subdir/c.h");
        file << "// text from c.h before include\n"
            "#include <std1.h>\n"
            "// text from c.h after include\n"s;
    }
    {
        ofstream file("sources/dir1/d.h");
        file << "// text from d.h before include\n"
            "#include \"lib/std2.h\"\n"
            "// text from d.h after include\n"s;
    }
    {
        ofstream file("sources/include1/std1.h");
        file << "// std1\n"s;
    }
    {
        ofstream file("sources/include2/lib/std2.h");
        file << "// std2\n"s;
    }
}

void TestGetIncludeFileNames() {
    string str = "// this comment before include\n"
        "#include \"dir1/b.h\"\n"
        "#include \"dir1/g.h\"\n"
        "// text between b.h and c.h\n"
        "#include \"dir1/d.h\"\n"
        "\n"
        "int SayHello() {\n"
        "    cout << \"hello, world!\" << endl;\n"
        "#   include<dummy.txt>\n"
        "#   include<dummy_2.txt>\n"
        "}\n"s;

    ParseIncludes(str);
}

void TestParsing() {
    string str = "cout << \"hello, world!\" << endl;\n"
        "#   include<dummy.txt>\n"
        "#   include<dummy_2.txt>\n"
        "}\n"s;

    regex ex(R"(#   include<dummy.txt>(.|\s)*)");
    string new_str = regex_replace(str, ex, "");
    cout << "Before: \n\n" << str << "\n\n" << "After: \n\n" << new_str;
}

void TestFindFileViaPath() {
    fs::path path("sources"s);
    fs::path find_path_c_h = FindFileViaPath(path, "c.h"s);
    fs::path find_path_std2_h = FindFileViaPath(path, "std2.h"s);
    fs::path find_path_a_cpp = FindFileViaPath(path, "a.cpp"s);
    fs::path find_path_b_h = FindFileViaPath(path, "b.h"s);

    cout << find_path_c_h.string() << '\n';
    cout << find_path_std2_h.string() << '\n';
    cout << find_path_a_cpp.string() << '\n';
    cout << find_path_b_h.string() << '\n';
}

void TestReplaceIncludes() {
    fs::path file_path = "sources"_p / "a.cpp"_p;
    vector<fs::path> include_directories = { "sources"_p / "include1"_p,"sources"_p / "include2"_p };
    ErrorData error;
    string path_include = ReplaceIncludes(file_path, include_directories, error);

    cout << path_include;
    if (error.line_) {
        cout << GetUnknownMsg(error.unknown_file_, error.source_file_, error.line_) << '\n';
    }
}

void MainTest() {
    MakeTestDirs();

    assert((!Preprocess("sources"_p / "a.cpp"_p,
        "sources"_p / "a.in"_p,
        { "sources"_p / "include1"_p,"sources"_p / "include2"_p })));

    ostringstream test_out;
    test_out << "// this comment before include\n"
        "// text from b.h before include\n"
        "// text from c.h before include\n"
        "// std1\n"
        "// text from c.h after include\n"
        "// text from b.h after include\n"
        "// text between b.h and c.h\n"
        "// text from d.h before include\n"
        "// std2\n"
        "// text from d.h after include\n"
        "\n"
        "int SayHello() {\n"
        "    cout << \"hello, world!\" << endl;\n"s;

    assert(GetFileContents("sources/a.in"s) == test_out.str());
}

void TestNoIncludes() {
    MakeTestDirs();

    {
        ofstream file("sources/a.cpp");
        file << "// this comment before include\n"
            "// text between b.h and c.h\n"
            "\n"
            "int SayHello() {\n"
            "    cout << \"hello, world!\" << endl;\n"
            "}\n"s;
    }

    assert((Preprocess("sources"_p / "a.cpp"_p,
        "sources"_p / "a.in"_p,
        { "sources"_p / "include1"_p,"sources"_p / "include2"_p })));

    ostringstream test_out;
    test_out << "// this comment before include\n"
        "// text between b.h and c.h\n"
        "\n"
        "int SayHello() {\n"
        "    cout << \"hello, world!\" << endl;\n"
        "}\n"s;

    assert(GetFileContents("sources/a.in"s) == test_out.str());
}

void TestNoOpenFile() {
    MakeTestDirs();

    assert(!(Preprocess("sources"_p / "g.cpp"_p,
        "sources"_p / "a.in"_p,
        { "sources"_p / "include1"_p,"sources"_p / "include2"_p })));
}

void TestNoExistingIncludeDirs() {
    MakeTestDirs();

    {
        ofstream file("sources/a.cpp");
        file << "// this comment before include\n"
            "#include \"dir1/g.h\"\n"
            "// text between b.h and c.h\n"
            "#include \"dir1/d.h\"\n"
            "\n"
            "int SayHello() {\n"
            "    cout << \"hello, world!\" << endl;\n"
            "#   include<dummy.txt>\n"
            "}\n"s;
    }

    assert(!(Preprocess("sources"_p / "a.cpp"_p,
        "sources"_p / "a.in"_p,
        { "sources"_p / "include_dir"_p,"sources"_p / "include_dir_2"_p })));

    ostringstream test_out;
    test_out << "// this comment before include\n";

    assert(GetFileContents("sources/a.in"s) == test_out.str());
}

void TestErrorIncludes() {
    MakeTestDirs();

    {
        ofstream file("sources/dir1/subdir/tester.cpp");
        file << "// this comment before include\n"
            "#include \"std1.h\"\n"
            "// text between b.h and c.h\n"
            "\n"
            "int SayHello() {\n"
            "#include \"lost_file\"\n"
            "cout << \"hello, world!\" << endl;\n"
            "}\n"s;

        file.close();
    }

    assert((!Preprocess("sources"_p / "dir1"_p / "subdir"_p / "tester.cpp"_p,
        "sources"_p / "a.in"_p,
        { "sources"_p / "include1"_p,"sources"_p / "include2"_p })));

    ostringstream test_out;
    test_out << "// this comment before include\n"
        "// std1\n"
        "// text between b.h and c.h\n"
        "\n"
        "int SayHello() {\n"s;

    assert(GetFileContents("sources/a.in"s) == test_out.str());
}