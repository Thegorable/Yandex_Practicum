#pragma once
#include "search_server.h"

template<typename T1>
void Assert(T1 value,
    const std::string file = __FILE__, const std::string& func = __FUNCTION__, const int line = __LINE__, bool abort = true) {
    if (!value) {
        std::cerr << "file: " << file << '\n';
        std::cerr << "function: " << func << '\n';
        std::cerr << "Line: " << line << '\n';
        std::cerr << "Assertation failed!" << '\n';
        if (abort) { std::abort(); }
    }
}

template<typename T1>
void AssertHint(T1 value, const std::string& hint,
    const std::string file = __FILE__, const std::string& func = __FUNCTION__, const int line = __LINE__, bool abort = true) {
    if (!value) {
        Assert(value, file, func, line, false);
        std::cerr << "hint: " << hint << '\n';
        if (abort) { std::abort(); }
    }
}

template<typename T1, typename T2>
void AssertEqualInt(const T1& a, const T2& b,
    const std::string& a_name, const std::string& b_name,
    const std::string file, const std::string& func, const int line, bool abort) {

    if (a != b) {
        Assert(false, file, func, line, false);
        std::cerr << "The objects " << a_name << "(" << a << ") " << "and " << b_name << "(" << b << ") are not equal\n";
        if (abort) { std::abort(); }
    }
}

template<typename T1, typename T2>
void AssertEqualFloat(const T1& a, const T2& b,
    const std::string& a_name, const std::string& b_name,
    const std::string file = __FILE__, const std::string& func = __FUNCTION__, const int line = __LINE__, bool abort = true) {

    if (abs(a - b) <= EPSILON) {
        Assert(false, file, func, line, false);
        std::cerr << "The objects " << a_name << "(" << a << ") " << "and " << b_name << "(" << b << ") are not equal\n";
        if (abort) { std::abort(); }
    }
}

template<typename T1, typename T2>
void AssertEqualIntHint(const T1& a, const T2& b,
    const std::string& a_name, const std::string& b_name, const std::string& hint,
    const std::string file = __FILE__, const std::string& func = __FUNCTION__, const int line = __LINE__, bool abort = true) {

    if (a != b) {
        AssertEqualInt<T1, T2>(a, b, a_name, b_name, file, func, line, false);
        std::cerr << "hint: " << hint << '\n';
        if (abort) { std::abort(); }
    }
}

template<typename T1, typename T2>
void AssertEqualFloatHint(const T1& a, const T2& b,
    const std::string& a_name, const std::string& b_name, const std::string& hint,
    const std::string file = __FILE__, const std::string& func = __FUNCTION__, const int line = __LINE__, bool abort = true) {

    if (abs(a - b) <= EPSILON) {
        AssertEqualFloat<T1, T2>(a, b, a_name, b_name, file, func, line, false);
        std::cerr << "hint: " << hint << '\n';
        if (abort) { std::abort(); }
    }
}

template<typename T>
void AssertEqualSizeContainers(const T& container_1, const T& container_2,
    const std::string& a_name, const std::string& b_name,
    const std::string file = __FILE__, const std::string& func = __FUNCTION__, const int line = __LINE__, bool abort = true) {

    if (container_1.size() != container_2.size()) {
        Assert(false, file, func, line, false);
        std::cerr << "The containers " << a_name << " and " << b_name << " have differents sizes.";
        if (abort) { std::abort(); }
    }
}

template<typename T>
void AssertEqualEmptyContainers(const T& container_1, const T& container_2,
    const std::string& a_name, const std::string& b_name,
    const std::string file = __FILE__, const std::string& func = __FUNCTION__, const int line = __LINE__, bool abort = true) {

    if (container_1.empty() != container_2.empty()) {
        Assert(false, file, func, line, false);
        std::cerr << "One of the containers " << a_name << " and " << b_name << " is not empty.";
        if (abort) { std::abort(); }
    }
}

template<typename contType1, typename contType2>
void AssertEqualElementsContainers(const contType1& container_1, const contType2& container_2,
    const std::string& a_name, const std::string& b_name,
    const std::string file = __FILE__, const std::string& func = __FUNCTION__, const int line = __LINE__, bool abort = true) {

    for (auto it_1 = container_1.begin(), it_2 = container_2.begin();
        it_1 != container_1.end() && it_2 != container_2.end();
        ++it_1, ++it_2) {

        const auto& elem_a = *it_1;
        const auto& elem_b = *it_2;

        if (elem_a != elem_b) {
            Assert(false, file, func, line, false);
            std::cerr << "The element " << "(" << elem_a << ") " << " in container " << a_name;
            std::cerr << " and element " << "(" << elem_b << ") " << " in container " << b_name;
            std::cerr << " are not equal\n";
            if (abort) { std::abort(); }
        }
    }
}

template <typename TestFunc>
void RunTestImpl(const TestFunc& func, const std::string& test_name) {
    func();
    std::cerr << test_name << " is runned SUCRESSFULLY" << std::endl;
}

#define RUN_TEST(func) RunTestImpl(func, #func)
#define ASSERT(a) Assert((a), __FILE__, (__FUNCTION__), __LINE__, true);
#define ASSERT_HINT(a, hint) AssertHint((a), (hint), __FILE__, (__FUNCTION__), __LINE__, true);
#define ASSERT_EQUAL(a, b) AssertEqualInt((a), (b), (#a), (#b), __FILE__, (__FUNCTION__), __LINE__, true);
#define ASSERT_EQUAL_HINT(a, b, hint) AssertEqualHint((a), (b), (#a), (#b), __FILE__, (__FUNCTION__), __LINE__, hint = (hint), true);

template<class function>
void AssertExeptionHintNegative(const function& func, const std::string& hint) {
    try {
        func();
        ASSERT_HINT(false, hint);
    }
    catch (...) {}
}

template<class function>
void AssertExeptionHintPositive(const function& func, const std::string& hint) {
    try {
        func();
    }
    catch (...) {
        ASSERT_HINT(false, hint);
    }
}

bool EqualFloat(double a, double b);
void TestExcludeStopWords();
void TestExcludeDocumentsWithMinusWords();
void TestExcludeAllMatchedWordsIfMinusWordExists();
void TestSortingByRelevance();
void TestRatingAveraging();
void TestDocumentsAreFilteredByPredicat();
void TestFilteringByStatus();
void TestRelevanceCalculation();
void TestMatchingDocuments();
void TestGettingDocumentCount();

//Additive functions tests
void TestStringContaintSpecSymbols();
void TestIsCharsAreDoubleMinus();
void TestIsNotCharsAfterMinus();

// Exeption Tests
void TestStopWordsQueryNotContainSpecSymbols();
void TestAddDocUniqueIdDoc();
void TestAddDocWithPositiveIdDoc();
void TestAddDocContainingSpecSymbol();
void TestClearRawFindDocs();
void TestClearRawMatchDocs();
void TestGettigIdByNaturalIndex();

void TestAdditiveFunctions();
void TestSearchServer();
void TestSearchServerExeptions();