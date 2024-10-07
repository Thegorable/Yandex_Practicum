#include "document.h"

using namespace std;

Document::Document(int doc_id, double doc_relevance, int doc_rating) :
    id(doc_id),
    relevance(doc_relevance),
    rating(doc_rating) {}

void PrintDocument(const Document& document) {
    cout << document << endl;
}

ostream& operator<<(ostream& out, const Document& document) {
    out << "{ "s
        << "document_id = "s << document.id << ", "s
        << "relevance = "s << document.relevance << ", "s
        << "rating = "s << document.rating << " }"s;
    return out;
}

ostream& operator<<(ostream& output, const DocumentStatus& status) {
    switch (status) {
    case DocumentStatus::ACTUAL:
        output << "ACTUAL"s;
        break;
    case DocumentStatus::IRRELEVANT:
        output << "IRRELEVANT"s;
        break;
    case DocumentStatus::BANNED:
        output << "BANNED"s;
        break;
    case DocumentStatus::REMOVED:
        output << "REMOVED"s;
        break;
    default:
        output << "<unknown>"s;
        break;
    }
    return output;
}