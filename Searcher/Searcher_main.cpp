#include "Searcher.h"

using namespace std;

void PrintDocument(const Document& document) {
    cout << "{ "s
        << "document_id = "s << document.id << ", "s
        << "relevance = "s << document.relevance << ", "s
        << "rating = "s << document.rating << " }"s << endl;
}

int main() {
    SearchServer search_server;
    search_server.SetStopWords("and in on"s);
    search_server.AddDocument(0, "white cat and cool collar"s, DocumentStatus::ACTUAL, { 8, -3 });
    search_server.AddDocument(1, "fluffy cat fluffy tail"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
    search_server.AddDocument(2, "groomed dog beatuful eyes"s, DocumentStatus::ACTUAL,{ 5, -12, 2, 1 });
    search_server.AddDocument(3, "groomed starling evgeny"s, DocumentStatus::BANNED, { 9 });

    cout << "ACTUAL by default:"s << endl;
    for (const Document& document : search_server.FindTopDocuments("fluffy groomed cat"s)) {
        PrintDocument(document);
    }
    cout << "BANNED:"s << endl;
    for (const Document& document : search_server.FindTopDocuments("fluffy groomed cat"s, DocumentStatus::BANNED)) {
        PrintDocument(document);
    }
    cout << "Even ids:"s << endl;
    for (const Document& document : search_server.FindTopDocuments("fluffy groomed cat"s, [](int document_id, DocumentStatus status, int rating) { return document_id % 2 == 0; })) {
        PrintDocument(document);
    }
    return 0;
}