#include "Searcher_Unit_Tests.h"
#include <sstream>

ostream& operator << (ostream& os, QueryStatus status) {
    switch (status)
    {
    case QueryStatus::General:
        os << "General";
        break;
    case QueryStatus::One:
        os << "One";
        break;
    case QueryStatus::NotMatching:
        os << "NotMatching";
        break;
    case QueryStatus::OnlyMinus:
        os << "OnlyMinus";
        break;
    }

    return os;
}

ostream& operator << (ostream& os, MinusWordsStatus status) {
    switch (status)
    {
    case MinusWordsStatus::Empty:
        os << "Empty";
        break;
    case MinusWordsStatus::Some:
        os << "Some";
        break;
    case MinusWordsStatus::OneWorking:
        os << "OneWorking";
        break;
    case MinusWordsStatus::NotWorking:
        os << "NotWorking";
        break;
    case MinusWordsStatus::AllDocCovered:
        os << "AllDocCovered";
        break;
    }

    return os;
}

ostream& operator << (ostream& os, StopWordsStatus status) {
    switch (status)
    {
    case StopWordsStatus::Empty:
        os << "Empty"s;
        break;
    case StopWordsStatus::General:
        os << "General"s;
        break;
    case StopWordsStatus::OneWorking:
        os << "OneWorking"s;
        break;
    case StopWordsStatus::NotWorking:
        os << "NotWorking"s;
        break;
    case StopWordsStatus::EveryIsWorking:
        os << "EveryIsWorking"s;
        break;
    }

    return os;
}

SearchServer SearchServerTester::CreateSearchServer(const inputDocs& test_docs) {
    SearchServer server;
    server.SetStopWords(test_docs.raw_stop_words);

    for (size_t i = 0; i < test_docs.count_docs_; i++) {
        server.AddDocument(test_docs.ids[i], test_docs.docs[i], test_docs.status[i], test_docs.ratings_raw[i]);
    }

    return server;
}

inputDocs::inputDocs() {
    standart_doc_set_ = { "white cat and cool collar"s,
        "fluffy cat fluffy tail"s,
        "groomed dog beautiful eyes"s,
        "starling groomed evgeny"s,
        "long cat beautiful on dog"s,
        "laguna make laguna like cat"s,
        "tail fox black"s,
        "grommed"s};

    standart_ids_doc_set_ = { 3, 7, 12, -5, 0, 19, 60 };

    working_stop_words = { "and"s, "on"s "like"s};
    not_working_stop_words = { "in"s, "what"s "above"s};

    working_plus_query_words = { "cat"s, "white"s, "dog"s, "grommed"s };
    not_working_plus_query_words = { "padla"s, "wolf"s, "octupus"s };
    working_minus_query_words = { "-cat", "-cool", "-dog", "-tail", "-groomed"};
    not_working_minus_query_words = { "-shark", "-dirty"};

    count_docs_ = 5;

    status = {
    DocumentStatus::ACTUAL,
    DocumentStatus::ACTUAL,
    DocumentStatus::ACTUAL,
    DocumentStatus::ACTUAL,
    DocumentStatus::ACTUAL,
    DocumentStatus::ACTUAL, 
    DocumentStatus::ACTUAL};
}

string inputDocs::WordsToString(const vector<string>& words, bool is_first = true) {
    string new_string;
    for (auto& word : words) {
        if (!is_first) {
            new_string.push_back(' ');
        }
        else {
            is_first = false;
        }
        new_string += word;
    }

    return new_string;
}

void inputDocs::FillAddedDocsByCount(size_t count) {
    count_docs_ = count;
    for (int num = 0; num < count; num++) {
        docs.push_back(standart_doc_set_.at(num));
        ids.push_back(standart_ids_doc_set_.at(num));
    }
}

void inputDocs::FillAddedDocsGeneral() {
    FillAddedDocsByCount(count_docs_);
    statuses.added_docs_status = AddedDocstStatus::General;
}

void inputDocs::FillAddedDocsMoreFive() {
    FillAddedDocsByCount(standart_doc_set_.size());
    statuses.added_docs_status = AddedDocstStatus::MoreFive;
}

void inputDocs::FillAddedDocsOne() {
    FillAddedDocsByCount(1);
    statuses.added_docs_status = AddedDocstStatus::One;
}

void inputDocs::FillAddedDocsSame() {
    for (int num = 0; num < count_docs_; num++) {
        docs.push_back(standart_doc_set_.at(0));
        ids.push_back(standart_ids_doc_set_.at(num));
    }
    statuses.added_docs_status = AddedDocstStatus::AllSame;
}

void inputDocs::FillAddedDocsByOneWord() {
    string word;
    
    for (int num = 0; num < count_docs_; num++) {
        istringstream stream(standart_doc_set_.at(0));
        stream >> word;
        docs.push_back(word);
        ids.push_back(standart_ids_doc_set_.at(num));
    }
    statuses.added_docs_status = AddedDocstStatus::AllContainOneWord;
}

void inputDocs::FillAddedDocs(AddedDocstStatus status) {
    switch (status)
    {
    case AddedDocstStatus::General:
        FillAddedDocsGeneral();
        break;
    case AddedDocstStatus::MoreFive:
        FillAddedDocsMoreFive();
        break;
    case AddedDocstStatus::One:
        FillAddedDocsOne();
        break;
    case AddedDocstStatus::AllSame:
        FillAddedDocsSame();
        break;
    case AddedDocstStatus::AllContainOneWord:
        FillAddedDocsByOneWord();
        break;
    }
}

void inputDocs::FillQueryGeneral(MinusWordsStatus minus_words_status) {
    switch (minus_words_status)
    {
    case MinusWordsStatus::Empty:
        raw_query = WordsToString(working_plus_query_words);
        raw_query += WordsToString(not_working_plus_query_words, false);
        break;

    case MinusWordsStatus::Some:
        raw_query = WordsToString(working_plus_query_words);
        raw_query += WordsToString({ working_minus_query_words.at(0) , not_working_minus_query_words.at(0) }, false);
        break;

    case MinusWordsStatus::OneWorking:
        raw_query = WordsToString(working_plus_query_words);
        raw_query.push_back(' ');
        raw_query += (working_minus_query_words.at(0));
        break;

    case MinusWordsStatus::NotWorking:
        raw_query = WordsToString(working_plus_query_words);
        raw_query += WordsToString(not_working_minus_query_words);
        break;

    case MinusWordsStatus::AllDocCovered:
        raw_query = WordsToString(working_plus_query_words);
        raw_query += WordsToString(working_minus_query_words, false);
        raw_query += WordsToString(not_working_minus_query_words, false);
        break;
    }
}

void inputDocs::FillQuerySingleWord(MinusWordsStatus minus_words_status) {
    switch (minus_words_status)
    {
    case MinusWordsStatus::Empty:
        raw_query = working_plus_query_words.at(0);
        break;
    case MinusWordsStatus::Some: case MinusWordsStatus::OneWorking: case MinusWordsStatus::AllDocCovered:
        raw_query = working_minus_query_words.at(0);
        break;
    case MinusWordsStatus::NotWorking:
        raw_query = not_working_minus_query_words.at(0);
        break;
    }
}

void inputDocs::FillQueryNotMatching(MinusWordsStatus minus_words_status) {
    switch (minus_words_status)
    {
    case MinusWordsStatus::Empty:
        raw_query = WordsToString(not_working_plus_query_words);
        break;

    case MinusWordsStatus::Some:
        raw_query = WordsToString(not_working_plus_query_words);
        raw_query += WordsToString({ not_working_minus_query_words.at(0) }, false);
        break;

    case MinusWordsStatus::OneWorking: case MinusWordsStatus::NotWorking:
        raw_query = WordsToString(not_working_plus_query_words);
        raw_query.push_back(' ');
        raw_query += (not_working_minus_query_words.at(0));
        break;

    case MinusWordsStatus::AllDocCovered:
        raw_query = WordsToString(not_working_plus_query_words);
        raw_query += WordsToString(not_working_minus_query_words, false);
        break;
    }
}

void inputDocs::FillQueryOnlyMinus(MinusWordsStatus minus_words_status) {
    switch (minus_words_status)
    {
    case MinusWordsStatus::Empty:
        raw_query.clear();
        break;

    case MinusWordsStatus::Some:
        raw_query = WordsToString({ working_minus_query_words.at(0) , not_working_minus_query_words.at(0) }, false);
        break;

    case MinusWordsStatus::OneWorking:
        raw_query = (working_minus_query_words.at(0));
        break;

    case MinusWordsStatus::NotWorking:
        raw_query = WordsToString(not_working_minus_query_words);
        break;

    case MinusWordsStatus::AllDocCovered:
        raw_query = WordsToString(working_minus_query_words);
        raw_query += WordsToString(not_working_minus_query_words, false);
        break;
    }
}

void inputDocs::FillQuery(QueryStatus query_status, MinusWordsStatus minus_words_status) {
    switch (query_status)
    {
    case QueryStatus::General:
        FillQueryGeneral(minus_words_status);
        break;
    case QueryStatus::One:
        FillQuerySingleWord(minus_words_status);
        break;
    case QueryStatus::NotMatching:
        FillQueryNotMatching(minus_words_status);
        break;
    case QueryStatus::OnlyMinus:
        FillQueryOnlyMinus(minus_words_status);
        break;
    }
}

void inputDocs::FillStopWords(StopWordsStatus stop_words_status) {
    switch (stop_words_status)
    {
    case StopWordsStatus::Empty:
        raw_stop_words.clear();
        break;

    case StopWordsStatus::General:
        raw_stop_words = WordsToString(working_stop_words);
        raw_stop_words += WordsToString(not_working_stop_words, false);
        break;

    case StopWordsStatus::OneWorking:
        raw_stop_words = working_stop_words.at(0);
        break;

    case StopWordsStatus::NotWorking:
        raw_stop_words = WordsToString(not_working_stop_words);
        break;

    case StopWordsStatus::EveryIsWorking:
        raw_stop_words = WordsToString(working_stop_words);
        break;
    }
}

void inputDocs::FillRatings(RatingDocsStatus status) {
    switch (status)
    {
    case RatingDocsStatus::General:
        for (int num = 0; num < count_docs_; num++) {
            (num % 2) ? ratings_raw.push_back(positive_ratings[num]) : ratings_raw.push_back(negative_ratings[num]);
        }
        break;

    case RatingDocsStatus::AllZero:
        for (int num = 0; num < count_docs_; num++) {
            ratings_raw.push_back({ 0 });
        }
        break;

    case RatingDocsStatus::AllSame:
        for (int num = 0; num < count_docs_; num++) {
            ratings_raw.push_back(positive_ratings.at(0));
        }
        break;

    case RatingDocsStatus::AllNegative:
        ratings_raw = negative_ratings;
        break;

    case RatingDocsStatus::AllPositive:
        ratings_raw = positive_ratings;
        break;

    case RatingDocsStatus::AllEmpty:
        for (auto& rating : ratings_raw) {
            rating.clear();
        }
        break;
    }
}

void SearchServerTester::TestExcludingStopWords() {
    inputDocs test_docs;
    test_docs.FillAddedDocs(AddedDocstStatus::General);
    test_docs.FillQuery(QueryStatus::General, MinusWordsStatus::Empty);
    test_docs.FillRatings(RatingDocsStatus::General);

    for (int status = static_cast<int>(StopWordsStatus::Empty); status <= static_cast<int>(StopWordsStatus::EveryIsWorking); status++) {

        test_docs.FillStopWords(static_cast<StopWordsStatus>(status));
        SearchServer server = CreateSearchServer(test_docs);
        vector<Document> docs = server.FindTopDocuments(test_docs.raw_query);

        cout << "StopWords status: \t" << static_cast<StopWordsStatus>(status) << '\n';
        for (const auto& doc : docs) {
            cout << "ID: "s << doc.id << '\t' << " Rating: "s << doc.rating << '\t' << " Relevance: " << doc.relevance << '\n';
        }
        if (docs.empty()) {
            cout << "Empty \n"s;
        }
        cout << '\n';
    }
}

void SearchServerTester::TestExcludingDocsByMinusWords() {
    inputDocs test_docs;
    test_docs.FillAddedDocs(AddedDocstStatus::General);
    test_docs.FillStopWords(StopWordsStatus::General);
    test_docs.FillRatings(RatingDocsStatus::General);

    for (int status_query = static_cast<int>(QueryStatus::General); status_query <= static_cast<int>(QueryStatus::OnlyMinus); status_query++) {
        for (int status = static_cast<int>(MinusWordsStatus::Empty); status <= static_cast<int>(MinusWordsStatus::AllDocCovered); status++) {
            
            test_docs.FillQuery(static_cast<QueryStatus>(status_query), static_cast<MinusWordsStatus>(status));
            SearchServer server = CreateSearchServer(test_docs);
            vector<Document> docs = server.FindTopDocuments(test_docs.raw_query);

            cout << "Query status: \t" << static_cast<QueryStatus>(status_query) << "\t MinusStatus: \t" << static_cast<MinusWordsStatus>(status) << '\n';
            for (const auto& doc : docs) {
                cout << "ID: "s << doc.id << '\t' << " Rating: "s << doc.rating << '\t' << " Relevance: " << doc.relevance << '\n';
            }
            if (docs.empty()) {
                cout << "Empty \n"s;
            }
            cout << '\n';
        }
    }
}

void TestSearchServer() {
    SearchServerTester tester;
    RunTestImpl([&tester]() {tester.TestExcludingStopWords(); }, "TestExcludingStopWords");
}