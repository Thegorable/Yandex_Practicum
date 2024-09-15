#pragma once
#ifndef __STD__LIBS__
#define __STD__LIBS__

#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <cmath>
#include <tuple>
#include <numeric>

#endif // __STD__LIBS__

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;
const double EPSILON = 1e-6;

template<typename T>
bool IsEqualFloat(const T& a, const T& b) {
    return a == b;
}

struct Document {
    int id;
    double relevance;
    int rating;
};

enum class DocumentStatus {
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED
};

ostream& operator << (ostream& os, const DocumentStatus& status);

string ReadLine();

int ReadLineWithNumber();

vector<string> SplitIntoWords(const string& text);

vector<int> ReadNumbers(int count);

struct inputDocs;

class SearchServer {
public:
    void SetStopWords(const string& text);

    void AddDocument(int doc_id,
        const string& document,
        DocumentStatus status,
        const vector<int>& ratings);

    template <typename DocumentPredicate>
    vector<Document> FindTopDocuments(const string& raw_query, DocumentPredicate document_predicate) const;

    vector<Document> FindTopDocuments(const string& raw_query, const DocumentStatus status = DocumentStatus::ACTUAL) const;

    tuple<vector<string>, DocumentStatus> MatchDocument(const string& raw_query, int document_id) const;

    int GetDocumentCount();

private:
    int document_count_ = 0;
    set<string> stop_words_;
    map<string, map<int, double>> word_to_document_freqs_;
    map<int, int> docs_rating;
    map<int, DocumentStatus> docs_status;

    struct QueryWord {
        string data;
        bool is_minus;
        bool is_stop;
    };

    struct Query {
        set<string> plus_words;
        set<string> minus_words;
    };

    template <typename predicat>
    vector<Document> FindAllDocuments(const Query& query, predicat comp) const;
    
    static int ComputeAverageRating(const vector<int>& ratings);

    bool IsStopWord(const string& word) const;

    vector<string> SplitIntoWordsNoStop(const string& text) const;

    QueryWord ParseQueryWord(string text) const;

    Query ParseQuery(const string& text) const;

    double ComputeWordInverseDocumentFreq(const string& word) const;

    friend void TestAddDocuments(const inputDocs& test_docs);
};

extern ostream& operator << (ostream& os, const std::pair<const int, const double> id_freq);

SearchServer CreateSearchServer();