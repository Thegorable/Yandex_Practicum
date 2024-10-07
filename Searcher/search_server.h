#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>
#include <tuple>
#include <stdexcept>
#include <algorithm>

#include "document.h"
#include "string_processing.h"

const int MAX_RESULT_DOCUMENT_COUNT = 5;
const double EPSILON = 1e-6;

class SearchServer {
public:
    template<class container>
    SearchServer(const container& cont);

    SearchServer(const std::string& text);

    void SetStopWords(const std::string& text);

    void AddDocument(int doc_id,
        const std::string& document,
        DocumentStatus status,
        const std::vector<int>& ratings);

    template <typename DocumentPredicate>
    std::vector<Document> FindTopDocuments(const std::string& raw_query,
        DocumentPredicate document_predicate) const;

     std::vector<Document> FindTopDocuments(const std::string& raw_query,
         const DocumentStatus status = DocumentStatus::ACTUAL) const;

    std::tuple<std::vector<std::string>, DocumentStatus> MatchDocument(const std::string& raw_query,
        int document_id) const;

    int GetDocumentCount();
    int GetDocumentId(int index) const;
    
    static bool IsCharSpecSymbol(char c);
    static bool IsNotContainSpecSymbols(const std::string& text);
    static bool IsCharsAreDoubleMinus(const char* c);
    static bool IsNotCharsAfterMinus(const char* c);

private:
    struct DocumentData {
        int rating;
        DocumentStatus status;
    };
    
    int document_count_ = 0;
    std::set<std::string> stop_words_;
    std::map<std::string, std::map<int, double>> word_to_document_freqs_;
    std::map<int, DocumentData> documents_;
    std::vector<int> ids;

    struct QueryWord {
        std::string data;
        bool is_minus;
        bool is_stop;
    };

    struct Query {
        std::set<std::string> plus_words;
        std::set<std::string> minus_words;
    };

    template <typename predicat>
    std::vector<Document> FindAllDocuments(const Query& query, predicat comp) const;

    static int ComputeAverageRating(const std::vector<int>& ratings);

    bool IsStopWord(const std::string& word) const;

    std::vector<std::string> SplitIntoWordsNoStop(const std::string& text) const;

    QueryWord ParseQueryWord(std::string text) const;

    Query ParseQuery(const std::string& text) const;

    double ComputeWordInverseDocumentFreq(const std::string& word) const;

    bool IsClearRawQuery(const std::string& raw_query) const;
};

template<typename container>
SearchServer::SearchServer(const container& cont) {
    for (const auto& word : cont) {
        if (!IsNotContainSpecSymbols(word)) {
            throw std::invalid_argument("Stop words query contains special symbols");
        }
        stop_words_.insert(word);
    }
}

template <typename DocumentPredicate>
std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query,
    DocumentPredicate document_predicate) const {

    const Query query = ParseQuery(raw_query);
    std::vector<Document> matched_docs = FindAllDocuments(query, document_predicate);

    std::sort(matched_docs.begin(), matched_docs.end(),
        [](const Document& lhs, const Document& rhs) {
            if (std::abs(lhs.relevance - rhs.relevance) < EPSILON) {
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
std::vector<Document> SearchServer::FindAllDocuments(const Query& query, predicat comp) const {
    std::map<int, double> document_to_relevance;
    for (const std::string& word : query.plus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
        for (const auto& [document_id, term_freq] : word_to_document_freqs_.at(word)) {
            document_to_relevance[document_id] += term_freq * inverse_document_freq;
        }
    }

    for (const std::string& word : query.minus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        for (const auto& [document_id, _] : word_to_document_freqs_.at(word)) {
            document_to_relevance.erase(document_id);
        }
    }

    std::vector<Document> matched_documents;
    for (const auto& [id, relevance] : document_to_relevance) {
        if (comp(id, documents_.at(id).status, documents_.at(id).rating)) {
            matched_documents.push_back({ id, relevance, documents_.at(id).rating });
        }
    }

    return matched_documents;
}