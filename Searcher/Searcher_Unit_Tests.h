#pragma once
#include "Searcher.h"

enum class StopWordsStatus { Empty, General, OneWorking, NotWorking, EveryIsWorking };
enum class MinusWordsStatus { Empty, Some, OneWorking, NotWorking, AllDocCovered };
enum class RatingDocsStatus{ General, AllZero, AllSame, AllNegative, AllPositive, AllEmpty };
enum class AddedDocstStatus{ General, MoreFive, One, AllSame, AllContainOneWord };
enum class QueryStatus{ General, One, NotMatching, OnlyMinus };

ostream& operator << (ostream& os, QueryStatus status);
ostream& operator << (ostream& os, MinusWordsStatus status);
ostream& operator << (ostream& os, StopWordsStatus status);

struct inputDocs {
    inputDocs();

    struct MainStatus { StopWordsStatus stop_status;
    MinusWordsStatus min_status;
    RatingDocsStatus rating_status;
    AddedDocstStatus added_docs_status;
    QueryStatus query_status; };

    vector<int> ids;
    vector<string> docs;
    vector <DocumentStatus> status;
    vector <vector<int>> ratings_raw;
    string raw_query;
    string raw_stop_words;

    MainStatus statuses;
    vector<Document> founded_docs;
    
    void FillAddedDocs(AddedDocstStatus status);
    void FillQuery(QueryStatus query_status, MinusWordsStatus minus_words_status);
    void FillStopWords(StopWordsStatus stop_words_status);
    void FillRatings(RatingDocsStatus status);

    int count_docs_;

private:
    void FillAddedDocsGeneral();
    void FillAddedDocsMoreFive();
    void FillAddedDocsOne();
    void FillAddedDocsSame();
    void FillAddedDocsByOneWord();

    void FillAddedDocsByCount(size_t count);
    
    void FillQueryGeneral(const MinusWordsStatus minus_words_status);
    void FillQuerySingleWord(const MinusWordsStatus minus_words_status);
    void FillQueryNotMatching(const MinusWordsStatus minus_words_status);
    void FillQueryOnlyMinus(const MinusWordsStatus minus_words_status);

    string WordsToString(const vector<string>& words, bool is_first);
    
    vector<string> standart_doc_set_;
    vector<int> standart_ids_doc_set_;

    vector<string> working_stop_words;
    vector<string> not_working_stop_words;

    vector<string> working_plus_query_words;
    vector<string> not_working_plus_query_words;
    vector<string> working_minus_query_words;
    vector<string> not_working_minus_query_words;

    vector<vector<int>> positive_ratings = { {0, 5, 2}, {5}, {1, 5, 3, 7}, {0}, {1}, {}, {3, 3} };
    vector<vector<int>> negative_ratings = { {-4, -9}, {-8}, {}, {-2, -1, -3, -6}, {-14}, {-1}, {-3, -3} };
};

class SearchServerTester {
public:
    void TestFindingAddedDoc();
    void TestExcludingStopWords();
    void TestExcludingDocsByMinusWords();
    void TestFindingDocsByWords();
    void TestRelevanceSorting();
    void TestRatingCalculate();
    void TestFilteringPredicate();
    void TestFindingDocsByStatus();
    void TestCaclulatingRelevance();

private:
    SearchServer CreateSearchServer(const inputDocs& test_docs);
};

void TestSearchServer();

template<typename T1>
void Assert(T1 value, 
    const string file = __FILE__, const string& func = __FUNCTION__, const int line = __LINE__, bool abort = true) {
    if (!value) {
        cerr << "file: " << file << '\n';
        cerr << "function: " << func << '\n';
        cerr << "Line: " << line << '\n';
        cerr << "Assertation failed!" << '\n';
        if (abort) { std::abort(); }
    }
}

template<typename T1>
void AssertHint(T1 value, const string& hint, 
    const string file = __FILE__, const string& func = __FUNCTION__, const int line = __LINE__, bool abort = true) {
    if (!value) {
        Assert(value, file, func, line, false);
        cerr << "hint: " << hint << '\n';
        if (abort) { std::abort(); }
    }
}

template<typename T1, typename T2, typename predicat>
void AssertEqualInt(const T1& a, const T2& b,
    const string& a_name, const string& b_name,
    const string file = __FILE__, const string& func = __FUNCTION__, const int line = __LINE__, bool abort = true) {

    if (a != b) {
        Assert(false, file, func, line, false);
        cerr << "The objects "s << a_name << "("s << a << ") "s << "and "s << b_name << "("s << b << ") are not equal\n"s;
        if (abort) { std::abort(); }
    }
}

template<typename T1, typename T2>
void AssertEqualFloat(const T1& a, const T2& b,
    const string& a_name, const string& b_name,
    const string file = __FILE__, const string& func = __FUNCTION__, const int line = __LINE__, bool abort = true) {
    
    if (abs(a - b) <= EPSILON) {
        Assert(false, file, func, line, false);
        cerr << "The objects "s << a_name << "("s << a << ") "s << "and "s << b_name << "("s << b << ") are not equal\n"s;
        if (abort) { std::abort(); }
    }
}

template<typename T1, typename T2>
void AssertEqualIntHint(const T1& a, const T2& b,
    const string& a_name, const string& b_name, const string& hint,
    const string file = __FILE__, const string& func = __FUNCTION__, const int line = __LINE__, bool abort = true) {

    if (a != b) {
        AssertEqualInt<T1, T2>(a, b, a_name, b_name, file, func, line, false);
        cerr << "hint: " << hint << '\n';
        if (abort) { std::abort(); }
    }
}

template<typename T1, typename T2>
void AssertEqualFloatHint(const T1& a, const T2& b,
    const string& a_name, const string& b_name, const string& hint,
    const string file = __FILE__, const string& func = __FUNCTION__, const int line = __LINE__, bool abort = true) {

    if (abs(a - b) <= EPSILON) {
        AssertEqualFloat<T1, T2>(a, b, a_name, b_name, file, func, line, false);
        cerr << "hint: " << hint << '\n';
        if (abort) { std::abort(); }
    }
}

template<typename T>
void AssertEqualSizeContainers(const T& container_1, const T& container_2,
    const string& a_name, const string& b_name,
    const string file = __FILE__, const string& func = __FUNCTION__, const int line = __LINE__, bool abort = true) {

    if (container_1.size() != container_2.size()) {
        Assert(false, file, func, line, false);
        cerr << "The containers "s << a_name << " and "s << b_name << " have differents sizes.";
        if (abort) { std::abort(); }
    }
}

template<typename T>
void AssertEqualEmptyContainers(const T& container_1, const T& container_2,
    const string& a_name, const string& b_name,
    const string file = __FILE__, const string& func = __FUNCTION__, const int line = __LINE__, bool abort = true) {

    if (container_1.empty() != container_2.empty()) {
        Assert(false, file, func, line, false);
        cerr << "One of the containers "s << a_name << " and "s << b_name << " is not empty.";
        if (abort) { std::abort(); }
    }
}

template<typename contType1, typename contType2>
void AssertEqualElementsContainers(const contType1& container_1, const contType2& container_2,
    const string& a_name, const string& b_name,
    const string file = __FILE__, const string& func = __FUNCTION__, const int line = __LINE__, bool abort = true) {

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
    cerr << test_name << " is runned SUCRESSFULLY"s << endl;
}

#define RUN_TEST(func) RunTestImpl(func, #func)
#define ASSERT(a) Assert((a));
#define ASSERT_HINT(a, hint) AssertHint((a), (hint), __FILE__, (__FUNCTION__), __LINE__, true);
#define ASSERT_EQUAL(a, b) AssertEqual((a), (b), (#a), (#b));
#define ASSERT_EQUAL_HINT(a, b, hint) AssertEqualHint((a), (b), (#a), (#b), __FILE__, (__FUNCTION__), __LINE__, hint = (hint), true);