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
    return *c == '-' && (*(c+1) == '\0' || *(c + 1) == ' ');
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