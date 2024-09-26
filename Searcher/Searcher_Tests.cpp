#pragma once
#include "Searcher_Tests.h"
#include "Searcher.h"

void TestExcludeStopWords() {
    const int id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = { 4, 5, -6 };
    {
        SearchServer server;
        server.AddDocument(id, content, DocumentStatus::ACTUAL, ratings);
        vector<Document> found_docs = server.FindTopDocuments("in"s);
        ASSERT_EQUAL(found_docs.size(), (size_t)1);
        const Document& doc = found_docs[0];
        ASSERT_EQUAL(doc.id, id);
    }
    {
        SearchServer server;
        server.SetStopWords("in the"s);
        server.AddDocument(id, content, DocumentStatus::ACTUAL, ratings);
        auto found_docs = server.FindTopDocuments("in"s);
        ASSERT_HINT(found_docs.empty(), "Stop words must be excluded from documents"s);
    }
}
void TestExcludeDocumentsWithMinusWords() {
    SearchServer server;
    server.AddDocument(0, "dog in house"s, DocumentStatus::ACTUAL, { 1 });
    server.AddDocument(1, "dog in the street"s, DocumentStatus::ACTUAL, { 1 });
    {
        auto found_docs = server.FindTopDocuments("dog"s);
        ASSERT_EQUAL(found_docs.size(), 2u);
    }
    {
        auto found_docs = server.FindTopDocuments("dog -house"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        ASSERT_EQUAL(found_docs[0].id, 1);
    }
    {
        auto found_docs = server.FindTopDocuments("dog -street"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        ASSERT_EQUAL(found_docs[0].id, 0);
    }
}
void TestExcludeAllMatchedWordsIfMinusWordExists() {
    SearchServer server;
    server.AddDocument(0, "big fat bunny walk in the city"s, DocumentStatus::ACTUAL, { 1 });
    {
        auto matched_docs = server.MatchDocument("walk bunny"s, 0);
        auto& words = get<0>(matched_docs);
        auto& status = get<1>(matched_docs);
        ASSERT_EQUAL(count(words.begin(), words.end(), "bunny"s), 1);
        ASSERT_EQUAL(count(words.begin(), words.end(), "walk"s), 1);
    }
    {
        auto matched_docs = server.MatchDocument("bunny -fat"s, 0);
        ASSERT(get<0>(matched_docs).empty());
    }
}
void TestSortingByRelevance() {
    SearchServer server;
    server.AddDocument(0, "white cat collar cat"s, DocumentStatus::ACTUAL, { 1 });
    server.AddDocument(1, "fluffy cat fluffy cat tail"s, DocumentStatus::ACTUAL, { 1 });
    server.AddDocument(2, "groomed dog beautiful cat eyes"s, DocumentStatus::ACTUAL, { 1 });
    server.AddDocument(3, "starling groomed evgeny"s, DocumentStatus::ACTUAL, { 1 });
    {
        auto found_docs = server.FindTopDocuments("cat"s);
        ASSERT_EQUAL(found_docs.size(), 3u);
        ASSERT(found_docs.front().relevance > found_docs.back().relevance);
        for (size_t i = 1; i < found_docs.size(); ++i) {
            ASSERT(found_docs[i - 1].relevance >= found_docs[i].relevance);
        }
    }
}
void TestRatingAveraging() {
    SearchServer server;
    server.AddDocument(0, "cat"s, DocumentStatus::ACTUAL, { -5, 50, 15 });
    auto found_docs = server.FindTopDocuments("cat"s);
    ASSERT_EQUAL(found_docs.size(), 1u);
    ASSERT_EQUAL(found_docs.front().rating, (-5 + 50 + 15) / 3);
}
void TestDocumentsAreFilteredByPredicat() {
    SearchServer server;
    server.AddDocument(0, "groomed dog"s, DocumentStatus::ACTUAL, { 1 });
    server.AddDocument(1, "scary dog"s, DocumentStatus::ACTUAL, { 1 });
    auto found_docs = server.FindTopDocuments("dog"s);
    ASSERT_EQUAL(found_docs.size(), 2u);
    found_docs = server.FindTopDocuments("dog"s, [](const int& doc_id, const DocumentStatus& status, const int& rating) {
        return doc_id == 0; });
    ASSERT_EQUAL(found_docs.size(), 1u);
    ASSERT(found_docs.front().id == 0);
}
void TestFilteringByStatus() {
    SearchServer server;
    server.AddDocument(0, "groomed dog"s, DocumentStatus::BANNED, { 1 });
    server.AddDocument(1, "scary dog"s, DocumentStatus::ACTUAL, { 1 });
    server.AddDocument(2, "angry dog"s, DocumentStatus::IRRELEVANT, { 1 });
    server.AddDocument(3, "dirty dog"s, DocumentStatus::REMOVED, { 1 });
    {
        auto found_docs = server.FindTopDocuments("dog"s, DocumentStatus::ACTUAL);
        ASSERT_EQUAL(found_docs.size(), 1u);
        ASSERT_EQUAL(found_docs.front().id, 1);
    }
    {
        auto found_docs = server.FindTopDocuments("dog"s, DocumentStatus::BANNED);
        ASSERT_EQUAL(found_docs.size(), 1u);
        ASSERT_EQUAL(found_docs.front().id, 0);
    }
    {
        auto found_docs = server.FindTopDocuments("dog"s, DocumentStatus::IRRELEVANT);
        ASSERT_EQUAL(found_docs.size(), 1u);
        ASSERT_EQUAL(found_docs.front().id, 2);
    }
    {
        auto found_docs = server.FindTopDocuments("dog"s, DocumentStatus::REMOVED);
        ASSERT_EQUAL(found_docs.size(), 1u);
        ASSERT_EQUAL(found_docs.front().id, 3);
    }
}
bool EqualFloat(double a, double b) {
    return abs(a - b) < 1e-6;
}
void TestRelevanceCalculation() {
    SearchServer server;
    server.AddDocument(0, "cat"s, DocumentStatus::ACTUAL, { 1 });
    server.AddDocument(1, "dog parrot"s, DocumentStatus::ACTUAL, { 1 });
    server.AddDocument(2, "parrot fox bunny shark"s, DocumentStatus::ACTUAL, { 1 });
    server.AddDocument(3, "money sea shark"s, DocumentStatus::ACTUAL, { 1 });
    {
        auto found_docs = server.FindTopDocuments("monkey"s);
        ASSERT(found_docs.empty());
    }
    {
        auto found_docs = server.FindTopDocuments("cat"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        ASSERT_EQUAL(found_docs[0].id, 0);
        ASSERT(EqualFloat(found_docs[0].relevance, (log(server.GetDocumentCount() * 1.0 / 1.0) * 1.0)));
    }
    {
        auto found_docs = server.FindTopDocuments("fox"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        ASSERT_EQUAL(found_docs[0].id, 2);
        ASSERT(EqualFloat(found_docs[0].relevance, (log(server.GetDocumentCount() * 1.0 / 1) * (1.0 / 4))));
    }
    {
        auto found_docs = server.FindTopDocuments("dog parrot"s);
        ASSERT_EQUAL(found_docs.size(), 2u);
        ASSERT_EQUAL(found_docs[0].id, 1);
        ASSERT(EqualFloat(found_docs[0].relevance, ((log(server.GetDocumentCount() / 1.0) + log(server.GetDocumentCount() / 2.0)) * 1.0 / 2.0)));
    }
    {
        auto found_docs = server.FindTopDocuments("cat dog fox"s);
        ASSERT_EQUAL(found_docs.size(), 3u);
        ASSERT_EQUAL(found_docs[0].id, 0);
        ASSERT(EqualFloat(found_docs[0].relevance, (log(server.GetDocumentCount() / 1.0))));
        ASSERT_EQUAL(found_docs[1].id, 1);
        ASSERT(EqualFloat(found_docs[1].relevance, (log(server.GetDocumentCount() / 1.0) * 1.0 / 2.0)));
        ASSERT_EQUAL(found_docs[2].id, 2);
        ASSERT(EqualFloat(found_docs[2].relevance, (log(server.GetDocumentCount() / 1.0) * 1.0 / 4.0)));
    }
    {
        auto found_docs = server.FindTopDocuments("sea shark"s);
        ASSERT_EQUAL(found_docs.size(), 2u);
        ASSERT_EQUAL(found_docs[0].id, 3);
        ASSERT_EQUAL(found_docs[1].id, 2);
        ASSERT(EqualFloat(found_docs[0].relevance, ((log(server.GetDocumentCount() / 1.0) + log(server.GetDocumentCount() / 2.0)) * (1.0 / 3.0))));
        ASSERT(EqualFloat(found_docs[1].relevance, ((log(server.GetDocumentCount() / 2.0)) * (1.0 / 4.0))));
    }
}
void TestMatchingDocuments() {
    SearchServer server;
    server.SetStopWords("a the and"s);
    server.AddDocument(0, "a quick brown fox jumps over the lazy dog"s, DocumentStatus::BANNED, { 1, 2, 3 });
    auto matched_docs = server.MatchDocument("a lazy cat and the brown dog"s, 0);
    auto& words = get<0>(matched_docs);
    auto& status = get<1>(matched_docs);
    set<string> matched_words;
    for (const auto& word : words) {
        matched_words.insert(word);
    }
    const set<string> expected_matched_words = { "lazy"s, "dog"s, "brown"s };

    ASSERT_EQUAL(matched_words.size(), expected_matched_words.size());
    for (auto it_1 = matched_words.begin(), it_2 = expected_matched_words.begin();
        it_1 != matched_words.end() && it_2 != expected_matched_words.end();
        it_1++, it_2++) {
        ASSERT_EQUAL(*it_1, *it_2);
    }
    ASSERT_EQUAL(status, DocumentStatus::BANNED);
}
void TestGettingDocumentCount() {
    SearchServer server;
    ASSERT_EQUAL(server.GetDocumentCount(), 0);
    server.AddDocument(0, "bla bla bla"s, DocumentStatus::ACTUAL, { 1 });
    ASSERT_EQUAL(server.GetDocumentCount(), 1);
    server.AddDocument(2, "ho ho ho ho"s, DocumentStatus::ACTUAL, { 1 });
    ASSERT_EQUAL(server.GetDocumentCount(), 2);
}

void TestStringContaintSpecSymbols() {
    ASSERT(IsNotContainSpecSymbols("Clear String"));
    ASSERT(IsNotContainSpecSymbols(""));

    char spec_char_1 = 0;
    char spec_char_2 = 10;
    char spec_char_3 = 20;
    char spec_char_4 = 30;
    ASSERT(!IsNotContainSpecSymbols(spec_char_1 + "Dirty String"s));
    ASSERT(!IsNotContainSpecSymbols("Dirty String"s + spec_char_2));
    ASSERT(!IsNotContainSpecSymbols("Dirty"s + spec_char_3 + "String"s));
    ASSERT(!IsNotContainSpecSymbols(spec_char_4 + "Dirty"s + spec_char_3 + "String"s + spec_char_1));
}

void TestIsCharsAreDoubleMinus() {
    ASSERT(!IsCharsAreDoubleMinus("a"));
    ASSERT(!IsCharsAreDoubleMinus("ab"));
    ASSERT(!IsCharsAreDoubleMinus("-"));
    ASSERT(!IsCharsAreDoubleMinus("-a"));
    ASSERT(!IsCharsAreDoubleMinus("a-"));
    
    ASSERT(IsCharsAreDoubleMinus("--"));
}

void TestIsNotCharsAfterMinus() {
    ASSERT(!IsNotCharsAfterMinus("-a"));
    ASSERT(!IsNotCharsAfterMinus("a"));
    ASSERT(!IsNotCharsAfterMinus("\n"));
    
    ASSERT(IsNotCharsAfterMinus("-"));
    ASSERT(IsNotCharsAfterMinus("- "));
}

void TestStopWordsQueryNotContainSpecSymbols() {
    string msg = "Filter stop words isn't work properly";
    AssertExeptionHintPositive([]() {SearchServer search_server("and in on"s); }, msg);
    AssertExeptionHintPositive([]() {SearchServer search_server(" "s); }, msg);
    AssertExeptionHintPositive([]() {SearchServer search_server(""s); }, msg);

    AssertExeptionHintPositive([]() {SearchServer search_server(vector<string>{"and"s, "in", "on"s}); }, msg);
    AssertExeptionHintPositive([]() {SearchServer search_server(vector<string>{"and"s}); }, msg);
    AssertExeptionHintPositive([]() {SearchServer search_server(vector<string>{" "s}); }, msg);
    AssertExeptionHintPositive([]() {SearchServer search_server(vector<string>{}); }, msg);
    
    AssertExeptionHintNegative([]() {SearchServer search_server("\nand in on"s); }, msg);
    AssertExeptionHintNegative([]() {SearchServer search_server("and \0 in on"s); }, msg);
    AssertExeptionHintNegative([]() {SearchServer search_server("and in on\n"s); }, msg);
    AssertExeptionHintNegative([]() {SearchServer search_server("and in on"s + (char)31); }, msg);

    AssertExeptionHintNegative([]() {SearchServer search_server(vector<string>{"\nand"s, "in"s, "on"s}); }, msg);
    AssertExeptionHintNegative([]() {SearchServer search_server(vector<string>{"and"s, "in\0"s, "on"s}); }, msg);
    AssertExeptionHintNegative([]() {SearchServer search_server(vector<string>{"and"s, "in"s, "on\t"s}); }, msg);
    
}

void TestAddDocUniqueIdDoc() {
    SearchServer search_server("and in on"s);
    search_server.AddDocument(1, "fluffy cat fluffy tail"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
    try {
        search_server.AddDocument(1, "fluffy dog and cool collar"s, DocumentStatus::ACTUAL, { 1, 2 });
        ASSERT_HINT(false, "The document with not unique id is added"s);
    }
    catch (std::invalid_argument& error) {
        cout << error.what() << '\n';
        cout << "Document isn't added because it contains not unique id"s << '\n';
    }
}

void TestAddDocWithPositiveIdDoc() {
    SearchServer search_server("and in on"s);
    try {
        search_server.AddDocument(-1, "fluffy dog and cool collar"s, DocumentStatus::ACTUAL, { 1, 2 });
        ASSERT_HINT(false, "The document with negative id is added"s);
    }
    catch (std::invalid_argument& error) {
        cout << error.what() << '\n';
        cout << "Document isn't added because it's id is negative"s << endl;
    }
}

void TestAddDocContainingSpecSymbol() {
    SearchServer search_server("and in on"s);
    try {
        search_server.AddDocument(3, "big dog star\x12ling"s, DocumentStatus::ACTUAL, { 1, 3, 2 });
        ASSERT_HINT(false, "The document with special symbols is added"s);
    }
    catch (std::invalid_argument& error) {
        cout << error.what() << '\n';
        cout << "Document isn't added because it contains special symbols"s << endl;
    }
}

void TestClearRawFindDocs() {
    const string msg_pos = "Correct query is not passed"s;
    
    {
        SearchServer serv("and in on"s);
        serv.AddDocument(1, "fluffy cat fluffy tail"s, DocumentStatus::ACTUAL, { 1 });
        const string msg_neg = "Double minus in query is passed"s;
        AssertExeptionHintNegative([serv]() {serv.FindTopDocuments("--cat fluffy"s); }, msg_neg);
        AssertExeptionHintNegative([serv]() {serv.FindTopDocuments("cat --fluffy"s); }, msg_neg);
        AssertExeptionHintNegative([serv]() {serv.FindTopDocuments("cat fluffy--"s); }, msg_neg);
        AssertExeptionHintPositive([serv]() {serv.FindTopDocuments("-cat fluffy"s); }, msg_pos);
        AssertExeptionHintPositive([serv]() {serv.FindTopDocuments("cat fluffy"s); }, msg_pos);
        AssertExeptionHintPositive([serv]() {serv.FindTopDocuments("fluffy cat"s); }, msg_pos);
    }
    
    {
        SearchServer serv("and in on"s);
        serv.AddDocument(1, "fluffy cat fluffy tail"s, DocumentStatus::ACTUAL, { 1 });
        const string msg_neg = "Empty minus in query is passed"s;
        AssertExeptionHintNegative([serv]() {serv.FindTopDocuments("fluffy cat-"s); }, msg_neg);
        AssertExeptionHintNegative([serv]() {serv.FindTopDocuments("fluffy- cat"s); }, msg_neg);
    }

    {
        SearchServer serv("and in on"s);
        serv.AddDocument(1, "fluffy cat fluffy tail"s, DocumentStatus::ACTUAL, { 1 });
        const string msg_neg = "Special symbol is passed"s;
        AssertExeptionHintNegative([serv]() {serv.FindTopDocuments("cat\n fluffy"s); }, msg_neg);
        AssertExeptionHintNegative([serv]() {serv.FindTopDocuments("\ncat fluffy"s); }, msg_neg);
        AssertExeptionHintNegative([serv]() {serv.FindTopDocuments("cat fluffy\n"s); }, msg_neg);
    }
}

void TestClearRawMatchDocs() {
    const string msg_pos = "Correct query is not passed"s;

    {
        SearchServer serv("and in on"s);
        serv.AddDocument(1, "fluffy cat fluffy tail"s, DocumentStatus::ACTUAL, { 1 });
        const string msg_neg = "Double minus in query is passed"s;
        AssertExeptionHintNegative([serv]() {serv.MatchDocument("--cat fluffy"s, 1); }, msg_neg);
        AssertExeptionHintNegative([serv]() {serv.MatchDocument("cat --fluffy"s, 1); }, msg_neg);
        AssertExeptionHintNegative([serv]() {serv.MatchDocument("cat fluffy--"s, 1); }, msg_neg);
        AssertExeptionHintPositive([serv]() {serv.MatchDocument("-cat fluffy"s, 1); }, msg_pos);
        AssertExeptionHintPositive([serv]() {serv.MatchDocument("cat fluffy"s, 1); }, msg_pos);
        AssertExeptionHintPositive([serv]() {serv.MatchDocument("fluffy cat"s, 1); }, msg_pos);
    }

    {
        SearchServer serv("and in on"s);
        serv.AddDocument(1, "fluffy cat fluffy tail"s, DocumentStatus::ACTUAL, { 1 });
        const string msg_neg = "Empty minus in query is passed"s;
        AssertExeptionHintNegative([serv]() {serv.MatchDocument("fluffy cat-"s, 1); }, msg_neg);
        AssertExeptionHintNegative([serv]() {serv.MatchDocument("fluffy- cat"s, 1); }, msg_neg);
    }

    {
        SearchServer serv("and in on"s);
        serv.AddDocument(1, "fluffy cat fluffy tail"s, DocumentStatus::ACTUAL, { 1 });
        const string msg_neg = "Special symbol is passed"s;
        AssertExeptionHintNegative([serv]() {serv.MatchDocument("cat\n fluffy"s, 1); }, msg_neg);
        AssertExeptionHintNegative([serv]() {serv.MatchDocument("\ncat fluffy"s, 1); }, msg_neg);
        AssertExeptionHintNegative([serv]() {serv.MatchDocument("cat fluffy\n"s, 1); }, msg_neg);
    }
}

void TestGettigIdByNaturalIndex() {
    const string msg_neg = "Correct getting id by natual index isn't work"s;
    const string msg_pos = "Incorrect getting id by natual is passed"s;
    
    SearchServer serv("and"s);
    serv.AddDocument(1, "fluffy"s, DocumentStatus::ACTUAL, { 1 });
    serv.AddDocument(2, "tail"s, DocumentStatus::ACTUAL, { 1 });
    AssertExeptionHintPositive([serv]() {serv.GetDocumentId(0); }, msg_neg);
    AssertExeptionHintPositive([serv]() {serv.GetDocumentId(1); }, msg_neg);
    AssertExeptionHintNegative([serv]() {serv.GetDocumentId(2); }, msg_neg);
    
    serv.AddDocument(3, "-tail"s, DocumentStatus::ACTUAL, { 1 });
    AssertExeptionHintPositive([serv]() {serv.GetDocumentId(2); }, msg_neg);
    AssertExeptionHintNegative([serv]() {serv.GetDocumentId(3); }, msg_neg);
    AssertExeptionHintNegative([serv]() {serv.GetDocumentId(-1); }, msg_neg);
}

void TestAdditiveFunctions() {
    TestStringContaintSpecSymbols();
    TestIsCharsAreDoubleMinus();
    TestIsNotCharsAfterMinus();
}

void TestSearchServer() {
    RUN_TEST(TestStringContaintSpecSymbols);
    RUN_TEST(TestExcludeStopWords);
    RUN_TEST(TestExcludeDocumentsWithMinusWords);
    RUN_TEST(TestExcludeAllMatchedWordsIfMinusWordExists);
    RUN_TEST(TestSortingByRelevance);
    RUN_TEST(TestRatingAveraging);
    RUN_TEST(TestDocumentsAreFilteredByPredicat);
    RUN_TEST(TestFilteringByStatus);
    RUN_TEST(TestRelevanceCalculation);
    RUN_TEST(TestMatchingDocuments);
    RUN_TEST(TestGettingDocumentCount);
}

void TestSearchServerExeptions() { 
    RUN_TEST(TestStopWordsQueryNotContainSpecSymbols);
    RUN_TEST(TestClearRawFindDocs);
    RUN_TEST(TestClearRawMatchDocs);
    RUN_TEST(TestAddDocUniqueIdDoc);
    RUN_TEST(TestAddDocWithPositiveIdDoc);
    RUN_TEST(TestAddDocContainingSpecSymbol);
    RUN_TEST(TestGettigIdByNaturalIndex);
}
