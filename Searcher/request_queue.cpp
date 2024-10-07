#include "request_queue.h"

using namespace std;

RequestQueue::RequestQueue(const SearchServer& search_server) : server(search_server) {}

vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentStatus status) {
    return AddFindRequest(raw_query, [status](int document_id, DocumentStatus status_lambda, int rating) {
        return status == status_lambda; });
}

int RequestQueue::GetNoResultRequests() const {
    return empty_results_size_;
}