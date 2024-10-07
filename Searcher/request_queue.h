#pragma once
#include <vector>
#include <deque>
#include <string>

#include "search_server.h"

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);

    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate);
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status = DocumentStatus::ACTUAL);
    int GetNoResultRequests() const;

private:
    struct QueryResult {
        bool is_empty_ = true;
        std::vector<Document> result;
    };

    std::deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    const SearchServer& server;
    int empty_results_size_ = 0;
};

template<typename DocumentPredicate>
inline std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
    QueryResult result;
    result.result = server.FindTopDocuments(raw_query, document_predicate);
    result.is_empty_ = result.result.empty();

    if (result.is_empty_) { empty_results_size_++; }

    if (requests_.size() == min_in_day_) {
        if (requests_.front().is_empty_) { empty_results_size_--; }
        requests_.pop_front();
    }
    requests_.push_back(std::move(result));

    return requests_.back().result;
}