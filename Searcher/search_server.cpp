#include <numeric>
#include <cmath>

#include "search_server.h"
#include "read_input_functions.h"

bool SearchServer::IsCharSpecSymbol(char c) {
    return c >= '\0' && c < ' ';
}

bool SearchServer::IsNotContainSpecSymbols(const std::string& text) {
    for (const char& c : text) {
        if (IsCharSpecSymbol(c)) {
            return false;
        }
    }
    return true;
}

bool SearchServer::IsCharsAreDoubleMinus(const char* c) {
    return *c == '-' && (*(c + 1) == '-');
}

bool SearchServer::IsNotCharsAfterMinus(const char* c) {
    return *c == '-' && (*(c + 1) == '\0' || *(c + 1) == ' ');
}

SearchServer::SearchServer(const std::string& text) :
    SearchServer(SplitIntoWords(text)) {}

void SearchServer::SetStopWords(const std::string& text) {
    if (!IsNotContainSpecSymbols(text)) {
        throw std::invalid_argument("Stop words query contains special symbols");
    }

    for (const std::string& word : SplitIntoWords(text)) {
        stop_words_.insert(word);
    }
}

void SearchServer::AddDocument(int doc_id,
    const std::string& document,
    DocumentStatus status,
    const std::vector<int>& ratings) {

    if (documents_.count(doc_id) ||
        doc_id < 0 ||
        !IsNotContainSpecSymbols(document)) {
        throw std::invalid_argument("The dirty document is added");
    };

    ++document_count_;
    ids.push_back(doc_id);
    const std::vector<std::string> words = SplitIntoWordsNoStop(document);
    const double inv_word_count = 1.0 / words.size();
    documents_[doc_id].rating = ComputeAverageRating(ratings);
    documents_[doc_id].status = status;

    for (const std::string& word : words) {
        word_to_document_freqs_[word][doc_id] += inv_word_count;
    }
}

std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query,
    const DocumentStatus status) const {

    return FindTopDocuments(raw_query, [status](int document_id, DocumentStatus status_lambda, int rating) {
        return status == status_lambda; }
    );
}

std::tuple<std::vector<std::string>, DocumentStatus> SearchServer::MatchDocument(const std::string& raw_query,
    int document_id) const {

    Query query = ParseQuery(raw_query);
    std::tuple<std::vector<std::string>, DocumentStatus> matched_docs(std::tuple<std::vector<std::string>, DocumentStatus>{});
    get<DocumentStatus>(matched_docs) = documents_.at(document_id).status;

    for (const std::string& word : query.minus_words) {
        if (word_to_document_freqs_.count(word) &&
            word_to_document_freqs_.at(word).count(document_id)) {
            return matched_docs;
        }
    }

    std::vector <std::string>& matched_words = get<0>(matched_docs);
    std::set<std::string> plus_words(query.plus_words);

    for (const std::string& word : plus_words) {
        if (word_to_document_freqs_.count(word)
            && word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.push_back(word);
        }
    }

    return matched_docs;
}

int SearchServer::GetDocumentCount() {
    return document_count_;
}

int SearchServer::GetDocumentId(int index) const {
    return ids.at(index);
}

int SearchServer::ComputeAverageRating(const std::vector<int>& ratings) {
    static int static_rating;
    static_rating = 0;

    if (ratings.empty()) {
        return static_rating;
    }

    static_rating = std::accumulate(ratings.begin(), ratings.end(), 0);
    static_rating /= static_cast<int>(ratings.size());

    return static_rating;
}

bool SearchServer::IsStopWord(const std::string& word) const {
    return stop_words_.count(word) > 0;
}

std::vector<std::string> SearchServer::SplitIntoWordsNoStop(const std::string& text) const {
    std::vector<std::string> words;
    for (const std::string& word : SplitIntoWords(text)) {
        if (!IsStopWord(word)) {
            words.push_back(word);
        }
    }
    return words;
}

SearchServer::QueryWord SearchServer::ParseQueryWord(std::string text) const {
    bool is_minus = false;
    if (text[0] == '-') {
        is_minus = true;
        text = text.substr(1);
    }
    return { text, is_minus, IsStopWord(text) };
}

SearchServer::Query SearchServer::ParseQuery(const std::string& text) const {
    if (!IsClearRawQuery(text)) { throw std::invalid_argument("Query is dirty"); }

    Query query;
    for (const std::string& word : SplitIntoWords(text)) {
        const QueryWord query_word = ParseQueryWord(word);
        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                query.minus_words.insert(query_word.data);
            }
            else {
                query.plus_words.insert(query_word.data);
            }
        }
    }
    return query;
}

double SearchServer::ComputeWordInverseDocumentFreq(const std::string& word) const {
    return log(document_count_ * 1.0 / word_to_document_freqs_.at(word).size());
}

bool SearchServer::IsClearRawQuery(const std::string& raw_query) const {
    for (const char& c : raw_query) {
        bool is_dirty_query = IsCharSpecSymbol(c) || IsCharsAreDoubleMinus(&c) || IsNotCharsAfterMinus(&c);
        if (is_dirty_query) {
            return false;
        }
    }
    return true;
}