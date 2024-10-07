#pragma once
#include<iostream>

struct Document {
    Document() = default;
    Document(int doc_id, double doc_relevance = 0, int doc_rating = 0);

    int id = 0;
    double relevance = 0.0;
    int rating = 0;
};

enum class DocumentStatus {
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED
};

void PrintDocument(const Document& document);

std::ostream& operator << (std::ostream& output, const Document& doc);
std::ostream& operator << (std::ostream& output, const DocumentStatus& status);