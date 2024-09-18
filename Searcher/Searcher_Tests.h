#pragma once
#include <functional>

enum class StopWordsStatus { NoStopWords, GeneralStopWords, OneStopWord, NotWorkingStopWords, EveryStopWordIsUsed };
enum class MinusWordsStatus { NoMinusWords, SomeMinusWords, OneMinusWord, NotWorkingMinusWords, AllDocCoveredMinusWord };

struct inputDocs {
    vector<int> ids;
    vector<string> doc;
    vector <DocumentStatus> status;
    vector <vector<int>> ratings_raw;
    string raw_query;
    string raw_stop_words;

    int docs_count_result = 0;
    set<string> stop_words_result;
    map<string, map<int, double>> word_to_document_freqs_result;
    map<int, int> docs_rating_result;
    map<int, DocumentStatus> docs_status_result;
    set<string> plus_words;
    set<string> minus_words;

    void FillGeneralTest(const StopWordsStatus stop_wrods_status, const MinusWordsStatus minus_words_status);
};

template<typename T1>
void Assert(T1 value, const string file, const string& func, const int line, bool abort = false) {
    if (!value) {
        cerr << "file: " << file << '\n';
        cerr << "function: " << func << '\n';
        cerr << "Line: " << line << '\n';
        cerr << "Assertation failed!" << '\n';

        if (abort) {
            std::abort();
        }
    }
}

template<typename T1>
void AssertHint(T1 value, const string file, const string& func, const int line, const string& hint, bool abort = false) {
    if (!value) {
        Assert(value, file, func, line, false);
        cerr << "hint: " << hint << '\n';

        if (abort) {
            std::abort();
        }
    }
}

template<typename T1, typename T2, typename predicat>
void AssertEqual(const T1& a, const T2& b,
    const string& a_name, const string& b_name,
    const string& file, const string& func, const int line,
    bool abort,
    predicat comp) {

    if (!comp()) {
        Assert(false, file, func, line, false);
        cerr << "The objects "s << a_name << "("s << a << ") "s << "and "s << b_name << "("s << b << ") are not equal\n"s;

        if (abort) {
            std::abort();
        }
    }
}

template<typename T1, typename T2>
void AssertEqual(const T1& a, const T2& b,
    const string& a_name, const string& b_name,
    const string& file, const string& func, const int line,
    bool abort = false) {
    AssertEqual(a, b, a_name, b_name, file, func, line, abort, [&a, &b]() { return a == b; });
}

template<typename T1, typename T2>
void AssertEqualHint(const T1& a, const T2& b,
    const string& a_name, const string& b_name,
    const string& file, const string& func, const int line,
    const string& hint, bool abort = false) {

    if (a != b) {
        AssertEqual<T1, T2>(a, b, a_name, b_name, file, func, line, false);
        cerr << "hint: " << hint << '\n';

        if (abort) {
            std::abort();
        }
    }
}

template<typename T>
void AssertEqualSizeContainers(const T& container_1, const T& container_2,
    const string& a_name, const string& b_name,
    const string& file, const string& func, const int line,
    bool abort = false) {

    if (container_1.size() != container_2.size()) {
        Assert(false, file, func, line, false);
        cerr << "The containers "s << a_name << " and "s << b_name << " have differents sizes.";
        if (abort) { std::abort(); }
    }
}

template<typename T>
void AssertEqualEmptyContainers(const T& container_1, const T& container_2,
    const string& a_name, const string& b_name,
    const string& file, const string& func, const int line,
    bool abort = false) {

    if (container_1.empty() != container_2.empty()) {
        Assert(false, file, func, line, false);
        cerr << "One of the containers "s << a_name << " and "s << b_name << " is not empty.";
        if (abort) { std::abort(); }
    }
}

template<typename contType1, typename contType2>
void AssertEqualElementsContainers(const contType1& container_1, const contType2& container_2,
    const string& a_name, const string& b_name,
    const string& file, const string& func, const int line,
    bool abort = false) {

    for (auto it_1 = container_1.begin(), it_2 = container_2.begin();
        it_1 != container_1.end() && it_2 != container_2.end();
        ++it_1, ++it_2) {

        const auto& elem_a = *it_1;
        const auto& elem_b = *it_2;

        if (elem_a != elem_b) {
            Assert(false, file, func, line, false);
            cerr << "The element "s << "("s << elem_a << ") "s << " in container " << a_name;
            cerr << " and element "s << "("s << elem_b << ") "s << " in container " << b_name;
            cerr << " are not equal\n"s;
            if (abort) { std::abort(); }
        }
    }
}

template <typename TestFunc>
void RunTestImpl(const TestFunc& func, const string& test_name) {
    func();
    cerr << test_name << " OK"s << endl;
}

#define RUN_TEST(func) RunTestImpl(func, #func)
#define ASSERT(a) Assert((a), __FILE__, (__FUNCTION__), __LINE__, true);
#define ASSERT_HINT(a, hint) AssertHint((a), __FILE__, (__FUNCTION__), __LINE__, (hint), true);
#define ASSERT_EQUAL(a, b) AssertEqual((a), (b), (#a), (#b), __FILE__, (__FUNCTION__), __LINE__, true);
#define ASSERT_EQUAL_FLOAT(a, b) AssertEqual((a), (b), (#a), (#b), __FILE__, (__FUNCTION__), __LINE__, true, [&a, &b] () {return abs(a-b) <= EPSILON; });
#define ASSERT_EQUAL_HINT(a, b, hint) AssertEqualHint((a), (b), (#a), (#b), __FILE__, (__FUNCTION__), __LINE__, (hint), true);
#define ASSERT_EQUAL_EMPTY_CONTAINERS(a, b) AssertEqualEmptyContainers((a), (b), #a, #b, __FILE__, (__FUNCTION__), __LINE__, true);
#define ASSERT_EQUAL_SIZE_CONTAINERS(a, b) AssertEqualSizeContainers((a), (b), #a, #b, __FILE__, (__FUNCTION__), __LINE__, true);
#define ASSERT_EQUAL_ELEMENTS_CONTAINERS(a, b) AssertEqualElementsContainers((a), (b), #a, #b, __FILE__, (__FUNCTION__), __LINE__, true);

void TestAllAddDocuments();

void TestAddDocuments(const inputDocs& test_docs);

void TestParsingAllMinusWords();
void TestMinusWordsExcludingDoc(const inputDocs& test_docs);

void TestParsingMinusWords(const inputDocs& test_docs);

void TestExcludeStopWordsFromAddedDocumentContent();

void TestSearchServer();