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

    int GetDocumentId(int index) const; // ???????? ????

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

bool IsCharSpecSymbol(char c) {
    return c >= 0 && c <= 31;
}

bool IsNotContainSpecSymbols(const string& text) {
    for (const char& c : text) {
        if (IsCharSpecSymbol(c)) {
            return false;
        }
    }
    return true;
}

bool IsCharsAreDoubleMinus(const char* c) {
    return *c == '-' && (*(c + 1) == '-');
}

bool IsNotCharsAfterMinus(const char* c) {
    return *c == '-' && (*(c + 1) == '\0' || *(c + 1) == ' ');
}

void PrintDocument(const Document& document) {
    cout << "{ "s
        << "document_id = "s << document.id << ", "s
        << "relevance = "s << document.relevance << ", "s
        << "rating = "s << document.rating << " }"s << endl;
}

SearchServer::SearchServer(const string& text) :
    SearchServer(SplitIntoWords(text)) {}

void SearchServer::SetStopWords(const string& text) {
    if (!IsNotContainSpecSymbols(text)) {
        throw invalid_argument("Stop words query contains special symbols");
    }

    for (const string& word : SplitIntoWords(text)) {
        stop_words_.insert(word);
    }
}

void SearchServer::AddDocument(int doc_id,
    const string& document,
    DocumentStatus status,
    const vector<int>& ratings) {

    if (docs_rating.count(doc_id) ||
        doc_id < 0 ||
        !IsNotContainSpecSymbols(document)) {
        throw std::invalid_argument("The dirty document is added");
    };

    ++document_count_;
    ids.push_back(doc_id);
    const vector<string> words = SplitIntoWordsNoStop(document);
    const double inv_word_count = 1.0 / words.size();
    docs_rating[doc_id] = ComputeAverageRating(ratings);
    docs_status[doc_id] = status;
    for (const string& word : words) {
        word_to_document_freqs_[word][doc_id] += inv_word_count;
    }
}

vector<Document> SearchServer::FindTopDocuments(const string& raw_query,
    const DocumentStatus status) const {

    return FindTopDocuments(raw_query, [status](int document_id, DocumentStatus status_lambda, int rating) {
        return status == status_lambda; }
    );
}

tuple<vector<string>, DocumentStatus> SearchServer::MatchDocument(const string& raw_query,
    int document_id) const {

    Query query = ParseQuery(raw_query);
    tuple<vector<string>, DocumentStatus> matched_docs(tuple<vector<string>, DocumentStatus>{});
    get<DocumentStatus>(matched_docs) = docs_status.at(document_id);

    for (const string& word : query.minus_words) {
        if (word_to_document_freqs_.count(word) &&
            word_to_document_freqs_.at(word).count(document_id)) {
            return matched_docs;
        }
    }

    vector <string>& matched_words = get<0>(matched_docs);
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

int SearchServer::GetDocumentId(int index) const {
    return ids.at(index);
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

vector<string> SearchServer::SplitIntoWordsNoStop(const string& text) const {
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
    if (!IsClearRawQuery(text)) { throw invalid_argument("Query is dirty"s); }

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

bool SearchServer::IsClearRawQuery(const string& raw_query) const {
    for (const char& c : raw_query) {
        bool is_dirty_query = IsCharSpecSymbol(c) || IsCharsAreDoubleMinus(&c) || IsNotCharsAfterMinus(&c);
        if (is_dirty_query) {
            return false;
        }
    }
    return true;
}