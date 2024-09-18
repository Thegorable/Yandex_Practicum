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
        const auto found_docs = server.FindTopDocuments("in"s);
        ASSERT_EQUAL(found_docs.size(), (size_t)1);
        const Document& doc = found_docs[0];
        ASSERT_EQUAL(doc.id, id);
    }
    {
        SearchServer server;
        server.SetStopWords("in the"s);
        server.AddDocument(id, content, DocumentStatus::ACTUAL, ratings);
        ASSERT_HINT(server.FindTopDocuments("in"s).empty(), "Stop words must be excluded from documents"s);
    }
}
void TestExcludeDocumentsWithMinusWords() {
    SearchServer server;
    server.AddDocument(0, "dog in house"s, DocumentStatus::ACTUAL, { 1 });
    server.AddDocument(1, "dog in the street"s, DocumentStatus::ACTUAL, { 1 });
    {
        const auto found_docs = server.FindTopDocuments("dog"s);
        ASSERT_EQUAL(found_docs.size(), 2u);
    }
    {
        const auto found_docs = server.FindTopDocuments("dog -house"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        ASSERT_EQUAL(found_docs[0].id, 1);
    }
    {
        const auto found_docs = server.FindTopDocuments("dog -street"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        ASSERT_EQUAL(found_docs[0].id, 0);
    }
}
void TestExcludeAllMatchedWordsIfMinusWordExists() {
    SearchServer server;
    server.AddDocument(0, "big fat bunny walk in the city"s, DocumentStatus::ACTUAL, { 1 });
    {
        const auto [words, status] = server.MatchDocument("walk bunny"s, 0);
        ASSERT_EQUAL(count(words.begin(), words.end(), "bunny"s), 1);
        ASSERT_EQUAL(count(words.begin(), words.end(), "walk"s), 1);
    }
    {
        const auto [words, status] = server.MatchDocument("bunny -fat"s, 0);
        ASSERT(words.empty());
    }
}
void TestSortingByRelevance() {
    SearchServer server;
    server.AddDocument(0, "white cat collar cat"s, DocumentStatus::ACTUAL, { 1 });
    server.AddDocument(1, "fluffy cat fluffy cat tail"s, DocumentStatus::ACTUAL, { 1 });
    server.AddDocument(2, "groomed dog beautiful cat eyes"s, DocumentStatus::ACTUAL, { 1 });
    server.AddDocument(3, "starling groomed evgeny"s, DocumentStatus::ACTUAL, { 1 });
    {
        const auto docs = server.FindTopDocuments("cat"s);
        ASSERT_EQUAL(docs.size(), 3u);
        ASSERT(docs.front().relevance > docs.back().relevance);
        for (size_t i = 1; i < docs.size(); ++i) {
            ASSERT(docs[i - 1].relevance >= docs[i].relevance);
        }
    }
}
void TestRatingAveraging() {
    SearchServer server;
    server.AddDocument(0, "cat"s, DocumentStatus::ACTUAL, { -5, 50, 15 });
    const auto docs = server.FindTopDocuments("cat"s);
    ASSERT_EQUAL(docs.size(), 1u);
    ASSERT_EQUAL(docs.front().rating, (-5 + 50 + 15) / 3);
}
void TestDocumentsAreFilteredByPredicat() {
    SearchServer server;
    server.AddDocument(0, "groomed dog"s, DocumentStatus::ACTUAL, { 1 });
    server.AddDocument(1, "scary dog"s, DocumentStatus::ACTUAL, { 1 });
    ASSERT_EQUAL(server.FindTopDocuments("dog"s).size(), 2u);
    const auto docs = server.FindTopDocuments("dog"s, [](const int& doc_id, const DocumentStatus& status, const int& rating) {
        return doc_id == 0; });
    ASSERT_EQUAL(docs.size(), 1u);
    ASSERT(docs.front().id == 0);
}
void TestFilteringByStatus() {
    SearchServer server;
    server.AddDocument(0, "groomed dog"s, DocumentStatus::BANNED, { 1 });
    server.AddDocument(1, "scary dog"s, DocumentStatus::ACTUAL, { 1 });
    server.AddDocument(2, "angry dog"s, DocumentStatus::IRRELEVANT, { 1 });
    server.AddDocument(3, "dirty dog"s, DocumentStatus::REMOVED, { 1 });
    {
        const auto docs = server.FindTopDocuments("dog"s, DocumentStatus::ACTUAL);
        ASSERT_EQUAL(docs.size(), 1u);
        ASSERT_EQUAL(docs.front().id, 1);
    }
    {
        const auto docs = server.FindTopDocuments("dog"s, DocumentStatus::BANNED);
        ASSERT_EQUAL(docs.size(), 1u);
        ASSERT_EQUAL(docs.front().id, 0);
    }
    {
        const auto docs = server.FindTopDocuments("dog"s, DocumentStatus::IRRELEVANT);
        ASSERT_EQUAL(docs.size(), 1u);
        ASSERT_EQUAL(docs.front().id, 2);
    }
    {
        const auto docs = server.FindTopDocuments("dog"s, DocumentStatus::REMOVED);
        ASSERT_EQUAL(docs.size(), 1u);
        ASSERT_EQUAL(docs.front().id, 3);
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
        const auto docs = server.FindTopDocuments("monkey"s);
        ASSERT(docs.empty());
    }
    {
        const auto docs = server.FindTopDocuments("cat"s);
        ASSERT_EQUAL(docs.size(), 1u);
        ASSERT_EQUAL(docs[0].id, 0);
        ASSERT(EqualFloat(docs[0].relevance, (log(server.GetDocumentCount() * 1.0 / 1.0) * 1.0)));
    }
    {
        const auto docs = server.FindTopDocuments("fox"s);
        ASSERT_EQUAL(docs.size(), 1u);
        ASSERT_EQUAL(docs[0].id, 2);
        ASSERT(EqualFloat(docs[0].relevance, (log(server.GetDocumentCount() * 1.0 / 1) * (1.0 / 4))));
    }
    {
        const auto docs = server.FindTopDocuments("dog parrot"s);
        ASSERT_EQUAL(docs.size(), 2u);
        ASSERT_EQUAL(docs[0].id, 1);
        ASSERT(EqualFloat(docs[0].relevance, ((log(server.GetDocumentCount() / 1.0) + log(server.GetDocumentCount() / 2.0)) * 1.0 / 2.0)));
    }
    {
        const auto docs = server.FindTopDocuments("cat dog fox"s);
        ASSERT_EQUAL(docs.size(), 3u);
        ASSERT_EQUAL(docs[0].id, 0);
        ASSERT(EqualFloat(docs[0].relevance, (log(server.GetDocumentCount() / 1.0))));
        ASSERT_EQUAL(docs[1].id, 1);
        ASSERT(EqualFloat(docs[1].relevance, (log(server.GetDocumentCount() / 1.0) * 1.0 / 2.0)));
        ASSERT_EQUAL(docs[2].id, 2);
        ASSERT(EqualFloat(docs[2].relevance, (log(server.GetDocumentCount() / 1.0) * 1.0 / 4.0)));
    }
    {
        const auto docs = server.FindTopDocuments("sea shark"s);
        ASSERT_EQUAL(docs.size(), 2u);
        ASSERT_EQUAL(docs[0].id, 3);
        ASSERT_EQUAL(docs[1].id, 2);
        ASSERT(EqualFloat(docs[0].relevance, ((log(server.GetDocumentCount() / 1.0) + log(server.GetDocumentCount() / 2.0)) * (1.0 / 3.0))));
        ASSERT(EqualFloat(docs[1].relevance, ((log(server.GetDocumentCount() / 2.0)) * (1.0 / 4.0))));
    }
}
void TestMatchingDocuments() {
    SearchServer server;
    server.SetStopWords("a the and"s);
    server.AddDocument(0, "a quick brown fox jumps over the lazy dog"s, DocumentStatus::BANNED, { 1, 2, 3 });
    const auto [words, status] = server.MatchDocument("a lazy cat and the brown dog"s, 0);
    set<string> matched_words;
    for (const auto& word : words) {
        matched_words.insert(word);
    }
    const set<string> expected_matched_words = { "lazy"s, "dog"s, "brown"s };

    ASSERT_EQUAL(matched_words.size(), expected_matched_words.size());
    for (auto it_1 = matched_words.begin(), it_2 = expected_matched_words.begin();
        it_1 != matched_words.end(), it_2 != expected_matched_words.end();
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

void TestSearchServer() {
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