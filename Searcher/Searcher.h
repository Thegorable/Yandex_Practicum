#pragma once
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
#include <optional>
#include <stdexcept>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;
const double EPSILON = 1e-6;

struct Document {
    Document() = default;

    Document(int doc_id, double doc_relevance = 0, int doc_rating = 0) :
        id(doc_id), 
        relevance(doc_relevance), 
        rating(doc_rating) {}

    int id = 0;
    double relevance = 0;
    int rating = 0;
};

enum class DocumentStatus {
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED
};

ostream& operator << (ostream& output, const DocumentStatus& status);

string ReadLine();

int ReadLineWithNumber();

vector<string> SplitIntoWords(const string& text);

vector<int> ReadNumbers(int count);

bool IsCharSpecSymbol(char c);

bool IsNotContainSpecSymbols(const string& text);

bool IsCharsAreDoubleMinus(const char* c);

bool IsNotCharsAfterMinus(const char* c);

void PrintDocument(const Document& document);

struct inputDocs;

class SearchServer {
public:
    SearchServer() = default;

    template<class container>
    SearchServer(const container& cont);

    SearchServer(const string& text);
    
    void SetStopWords(const string& text);

    void AddDocument(int doc_id,
        const string& document,
        DocumentStatus status,
        const vector<int>& ratings);

    template <typename DocumentPredicate>
    vector<Document> FindTopDocuments(const string& raw_query,
        DocumentPredicate document_predicate) const;
    
    vector<Document> FindTopDocuments(const string& raw_query,
        const DocumentStatus status = DocumentStatus::ACTUAL) const;

    tuple<vector<string>, DocumentStatus> MatchDocument(const string& raw_query,
        int document_id) const;

    int GetDocumentCount();

    int GetDocumentId(int index) const; // Написать тест

private:
    int document_count_ = 0;
    set<string> stop_words_;
    map<string, map<int, double>> word_to_document_freqs_;
    map<int, int> docs_rating;
    map<int, DocumentStatus> docs_status;
    vector<int> ids;

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

    bool IsClearRawQuery(const string& raw_query) const;
};

template<typename container>
SearchServer::SearchServer(const container& cont) {
    for (const auto& word : cont) {
        if (!IsNotContainSpecSymbols(word)) {
            throw invalid_argument("Stop words query contains special symbols");
        }
        stop_words_.insert(word);
    }
}

template <typename DocumentPredicate>
vector<Document> SearchServer::FindTopDocuments(const string& raw_query,
    DocumentPredicate document_predicate) const {

    const Query query = ParseQuery(raw_query);
    vector<Document> matched_docs = FindAllDocuments(query, document_predicate);

    sort(matched_docs.begin(), matched_docs.end(),
        [](const Document& lhs, const Document& rhs) {
            if (abs(lhs.relevance - rhs.relevance) < EPSILON) {
                return lhs.rating > rhs.rating;
            }
            return lhs.relevance > rhs.relevance;
        });
    if (matched_docs.size() > MAX_RESULT_DOCUMENT_COUNT) {
        matched_docs.resize(MAX_RESULT_DOCUMENT_COUNT);
    }
    return matched_docs;
}

template <typename predicat>
vector<Document> SearchServer::FindAllDocuments(const Query& query, predicat comp) const {
    map<int, double> document_to_relevance;
    for (const string& word : query.plus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
        for (const auto& [document_id, term_freq] : word_to_document_freqs_.at(word)) {
            document_to_relevance[document_id] += term_freq * inverse_document_freq;
        }
    }

    for (const string& word : query.minus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        for (const auto& [document_id, _] : word_to_document_freqs_.at(word)) {
            document_to_relevance.erase(document_id);
        }
    }

    vector<Document> matched_documents;
    for (const auto& [id, relevance] : document_to_relevance) {
        if (comp(id, docs_status.at(id), docs_rating.at(id))) {
            matched_documents.push_back({ id, relevance, docs_rating.at(id) });
        }
    }
    return matched_documents;
}