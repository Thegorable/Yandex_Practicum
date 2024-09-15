#include "Searcher_Tests.h"

void TestAddDocuments(const inputDocs& test_docs) {

    SearchServer server;
    server.SetStopWords(test_docs.raw_stop_words);
    
    for (size_t i = 0; i < test_docs.docs_count_result; i++) {
        server.AddDocument(test_docs.ids[i], test_docs.doc[i], test_docs.status[i], test_docs.ratings[i]);
    }
    ASSERT_EQUAL(server.document_count_, test_docs.docs_count_result);

    for (const int id : test_docs.ids) {
        ASSERT(server.docs_rating.count(id));
        ASSERT_EQUAL(server.docs_rating.at(id), test_docs.docs_rating_result.at(id));
        ASSERT_EQUAL(server.docs_status.at(id), test_docs.docs_status_result.at(id));
    }
    ASSERT_EQUAL_EMPTY_CONTAINERS(server.stop_words_, test_docs.stop_words_result);
    
    if (server.stop_words_.empty() != test_docs.stop_words_result.empty()) {
        ASSERT_EQUAL_SIZE_CONTAINERS(server.stop_words_, test_docs.stop_words_result);
        ASSERT_EQUAL_ELEMENTS_CONTAINERS(server.stop_words_, test_docs.stop_words_result);
    }

    for (auto [word, id_frec] : test_docs.word_to_document_freqs_result) {
        string hint_word = "The word "s + word + " doesn't exists in server"s;
        ASSERT_HINT(server.word_to_document_freqs_.count(word), hint_word);
        ASSERT_EQUAL_SIZE_CONTAINERS(server.word_to_document_freqs_.at(word), id_frec);
        
        for (auto it_1 = server.word_to_document_freqs_.at(word).begin(), it_2 = id_frec.begin();
            it_1 != server.word_to_document_freqs_.at(word).end() && it_2 != id_frec.end();
            ++it_1, ++it_2) {
            
            const auto& elem_l = *it_1;
            const auto& elem_2 = *it_2;
            const double& freq_l = elem_l.second;
            const double& freq_r = elem_2.second;

            ASSERT_EQUAL(elem_l.first, elem_2.first);
            ASSERT_EQUAL_FLOAT(freq_l, freq_r);
        }
    }

}

void TestMinusWords(const inputDocs& test_docs) {
    SearchServer server;
    server.SetStopWords(test_docs.raw_stop_words);

    for (size_t i = 0; i < test_docs.docs_count_result; i++) {
        server.AddDocument(test_docs.ids[i], test_docs.doc[i], test_docs.status[i], test_docs.ratings[i]);
    }
}

void TestExcludeStopWordsFromAddedDocumentContent() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = { 1, 2, 3 };
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id);
    }

    {
        SearchServer server;
        server.SetStopWords("in the"s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        ASSERT_HINT(server.FindTopDocuments("in"s).empty(),
            "Stop words must be excluded from documents"s);
    }
}

void TestAllAddDocuments() {
    inputDocs test;
    test.FillGeneralTest(StopWordsStatus::GeneralStopWords);
    TestAddDocuments(test);
    test.FillGeneralTest(StopWordsStatus::NoStopWords);
    TestAddDocuments(test);
    test.FillGeneralTest(StopWordsStatus::NotWorkingStopWords);
    TestAddDocuments(test);
    test.FillGeneralTest(StopWordsStatus::OneStopWord);
    TestAddDocuments(test);
    test.FillGeneralTest(StopWordsStatus::EveryStopWordIsUsed);
    TestAddDocuments(test);
}

void TestSearchServer() {
    RUN_TEST(TestAllAddDocuments);
}

void inputDocs::FillGeneralTest(const StopWordsStatus stop_wrods_status) {
    ids = { 3, 7, 12, -5, 0 };
    doc = { "white cat and cool collar"s,
        "fluffy cat fluffy tail"s,
        "groomed dog beautiful eyes"s,
        "groomed starling evgeny"s,
        "long cat beautiful dog" };
    ratings = { {},
        {3, 5, 8, 4},
        {11, 5, 0, -2, 1},
        {-4, -2, 1, 5, 8},
        {0} };
    status = {
    DocumentStatus::ACTUAL,
    DocumentStatus::ACTUAL,
    DocumentStatus::ACTUAL,
    DocumentStatus::ACTUAL,
    DocumentStatus::BANNED };
    raw_query = "fluffy groomed cat"s;
    
    switch (stop_wrods_status) {
    case StopWordsStatus::GeneralStopWords:
        raw_stop_words = "in and on groomed"s;
        stop_words_result = { "in"s, "and"s, "on"s, "groomed"s};
        break;

    case StopWordsStatus::OneStopWord:
        raw_stop_words = "and"s;
        stop_words_result = {"and"};
        break;

    case StopWordsStatus::NotWorkingStopWords:
        raw_stop_words = "bingo laba inkognito lorg"s;
        stop_words_result = { "bingo"s, "laba"s, "inkognito"s, "lorg"s };
        break;

    case StopWordsStatus::EveryStopWordIsUsed:
        raw_stop_words = "fluffy and dog"s;
        stop_words_result = { "fluffy"s, "and"s, "dog"s };
        break;

    case StopWordsStatus::NoStopWords:
        raw_stop_words = ""s;
        stop_words_result = {};
        break;
    }

    
    docs_count_result = 5;
    docs_rating_result = { {3,0}, {7,5}, {12, 3}, {-5, 1}, {0, 0} };
    docs_status_result = {
        {3, DocumentStatus::ACTUAL},
        {7, DocumentStatus::ACTUAL},
        {12,DocumentStatus::ACTUAL},
        { -5, DocumentStatus::ACTUAL},
        {0, DocumentStatus::BANNED} };
    word_to_document_freqs_result = {
    { "white",    { { 3, 0.200000 } } },
    { "cat",      { { 3, 0.200000 }, { 7, 0.2500000 }, { 0, 0.2500000 } } },
    { "and",     { { 3, 0.200000 } } },
    { "cool",     { { 3, 0.200000 } } },
    { "collar",   { { 3, 0.200000 } } },
    { "fluffy",   { { 7, 0.500000 } } },
    { "tail",     { { 7, 0.2500000 } } },
    { "groomed",  { { 12, 0.2500000 }, { -5, 0.3333333 } } },
    { "dog",      { { 12, 0.2500000 }, { 0, 0.2500000 } } },
    { "beautiful",{ { 12, 0.2500000 }, { 0, 0.2500000 } } },
    { "eyes",     { { 12, 0.2500000 } } },
    { "starling",{ { -5, 0.3333333 } } },
    { "evgeny",   { { -5, 0.3333333 } } },
    { "long",     { { 0, 0.2500000 } } }
    };

    switch (stop_wrods_status) {
    case StopWordsStatus::GeneralStopWords:
        word_to_document_freqs_result.erase("and"s);
        word_to_document_freqs_result.erase("groomed"s);
        word_to_document_freqs_result["white"][3] = 0.250000;
        word_to_document_freqs_result["cat"][3] = 0.250000;
        word_to_document_freqs_result["cool"][3] = 0.250000;
        word_to_document_freqs_result["collar"][3] = 0.250000;
        word_to_document_freqs_result["dog"][12] = 0.333333;
        word_to_document_freqs_result["beautiful"][12] = 0.333333;
        word_to_document_freqs_result["eyes"][12] = 0.333333;
        word_to_document_freqs_result["starling"][-5] = 0.500000;
        word_to_document_freqs_result["evgeny"][-5] = 0.500000;
        break;

    case StopWordsStatus::OneStopWord:
        word_to_document_freqs_result.erase("and"s);
        word_to_document_freqs_result["white"][3] = 0.250000;
        word_to_document_freqs_result["cat"][3] = 0.250000;
        word_to_document_freqs_result["cool"][3] = 0.250000;
        word_to_document_freqs_result["collar"][3] = 0.250000;
        break;

    case StopWordsStatus::EveryStopWordIsUsed:
        word_to_document_freqs_result.erase("and"s);
        word_to_document_freqs_result.erase("fluffy"s);
        word_to_document_freqs_result.erase("dog"s);
        word_to_document_freqs_result["white"][3] = 0.250000;
        word_to_document_freqs_result["cat"][3] = 0.250000;
        word_to_document_freqs_result["cool"][3] = 0.250000;
        word_to_document_freqs_result["collar"][3] = 0.250000;
        word_to_document_freqs_result["cat"][7] = 0.500000;
        word_to_document_freqs_result["tail"][7] = 0.500000;
        word_to_document_freqs_result["groomed"][12] = 0.333333;
        word_to_document_freqs_result["beautiful"][12] = 0.333333;
        word_to_document_freqs_result["eyes"][12] = 0.333333;
        word_to_document_freqs_result["long"][0] = 0.333333;
        word_to_document_freqs_result["cat"][0] = 0.333333;
        word_to_document_freqs_result["beautiful"][0] = 0.333333;
        break;
    }
}