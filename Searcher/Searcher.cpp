#include"Searcher.h"

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

template<>
bool IsEqualFloat<double>(const double& a, const double& b) {
    return abs(a - b) <= EPSILON;
}

template<>
bool IsEqualFloat<std::pair<int, double>>(const std::pair<int, double>& a, const std::pair<int, double>& b) {
    bool int_comp = a.first == b.first;
    bool double_comp = abs(a.second - b.second) <= EPSILON;
    return (int_comp && double_comp);
}

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
        else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

vector<int> ReadNumbers(int count) {
    vector<int> numbers;
    int num = 0;
    for (int i = 0; i < count; i++) {
        cin >> num;
        numbers.push_back(num);
    }

    cin >> ws;
    return numbers;
}

void SearchServer::SetStopWords(const string& text) {
    for (const string& word : SplitIntoWords(text)) {
        stop_words_.insert(word);
    }
}

void SearchServer::AddDocument(int doc_id,
    const string& document,
    DocumentStatus status,
    const vector<int>& ratings) {

    ++document_count_;
    const vector<string> words = SplitIntoWordsNoStop(document);
    const double inv_word_count = 1.0 / words.size();
    docs_rating[doc_id] = ComputeAverageRating(ratings);
    docs_status[doc_id] = status;
    for (const string& word : words) {
        word_to_document_freqs_[word][doc_id] += inv_word_count;
    }
}

vector<Document> SearchServer::FindTopDocuments(const string& raw_query, const DocumentStatus status) const {
    return this->FindTopDocuments(raw_query, [status](int document_id, DocumentStatus status_lambda, int rating) {
        return status == status_lambda; });
}

tuple<vector<string>, DocumentStatus> SearchServer::MatchDocument(const string& raw_query, int document_id) const {
    tuple<vector<string>, DocumentStatus> matched_docs;
    get<DocumentStatus>(matched_docs) = docs_status.at(document_id);
    Query query = ParseQuery(raw_query);

    vector <string>& matched_words = get<0>(matched_docs);

    for (const string& word : query.minus_words) {
        if (word_to_document_freqs_.count(word) &&
            word_to_document_freqs_.at(word).count(document_id)) {
            return matched_docs;
        }
    }

    set<string> plus_words(query.plus_words);

    for (const string& word : plus_words) {
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

int SearchServer::ComputeAverageRating(const vector<int>& ratings) {
    static int static_rating;
    static_rating = 0;

    if (ratings.empty()) {
        return static_rating;
    }

    static_rating = accumulate(ratings.begin(), ratings.end(), 0);
    static_rating /= static_cast<int>(ratings.size());

    return static_rating;
}

bool SearchServer::IsStopWord(const string& word) const {
    return stop_words_.count(word) > 0;
}

vector<string> SearchServer::SplitIntoWordsNoStop(const string& text) const { // Тестить
    vector<string> words;
    for (const string& word : SplitIntoWords(text)) {
        if (!IsStopWord(word)) {
            words.push_back(word);
        }
    }
    return words;
}

SearchServer::QueryWord SearchServer::ParseQueryWord(string text) const {
    bool is_minus = false;
    if (text[0] == '-') {
        is_minus = true;
        text = text.substr(1);
    }
    return { text, is_minus, IsStopWord(text) };
}

SearchServer::Query SearchServer::ParseQuery(const string& text) const {
    Query query;
    for (const string& word : SplitIntoWords(text)) {
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

double SearchServer::ComputeWordInverseDocumentFreq(const string& word) const {
    return log(document_count_ * 1.0 / word_to_document_freqs_.at(word).size());
}

extern ostream& operator << (ostream& os, const std::pair<const int, const double> id_freq) {
    os << id_freq.first << ", "s << id_freq.second;
    return os;
}

SearchServer CreateSearchServer()
{
    const string stop_words_joined = ReadLine();
    const int document_count = ReadLineWithNumber();

    SearchServer searcher;
    searcher.SetStopWords(stop_words_joined);
    string doc_text;
    string raiting_text;

    for (int document_id = 0; document_id < document_count; ++document_id) {
        doc_text = ReadLine();
        int num_count;
        cin >> num_count;
        vector<int> ratings = ReadNumbers(num_count);
        searcher.AddDocument(document_id, doc_text, DocumentStatus::ACTUAL, ratings);
    }

    return searcher;
}