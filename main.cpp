#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <cmath>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
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
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Document {
    int id;
    double relevance;
};

class SearchServer {
public:
    int document_count_ = 0;
    
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document) {
        for (const string& word : SplitIntoWordsNoStop(document)) {
            documents_[word].insert({document_id, 0});
        }
    const vector<string> j = SplitIntoWordsNoStop(document);
    const double TF = 1.0 / j.size();
        for(const string& k : j){
        documents_[k][document_id] += TF;
        }
    ++document_count_;
    }

    double ret_IDF(const string& word) const{
       return log((document_count_  * 1.0) / documents_.at(word).size());
    }
    
    vector<Document> FindTopDocuments(const string& raw_query) const {
        const Query query_words = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query_words);

        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                 return lhs.relevance > rhs.relevance;
             });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

private:

    map<string, map<int, double>> documents_;

    set<string> stop_words_;

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }
    

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)){
                words.push_back(word);
                }        
            }
        return words;
    }
    struct Query{
        set<string> minus_words;
        set<string> plus_words;
    };

    Query ParseQuery(const string& text) const {
        Query query_words;
        for (const string& word : SplitIntoWordsNoStop(text)){
            if(word[0] == '-'){
                string otrez = word.substr(1);
            if(!IsStopWord(otrez)){
                query_words.minus_words.insert(otrez);
            }
            }else{
            query_words.plus_words.insert(word);
            }
            }
        return query_words;
    }

    vector<Document> FindAllDocuments(const Query& query_words) const {
        map<int, double> document_to_relevance;
        for(const string& word : query_words.plus_words){
            if(documents_.count(word) != 0){ 
                double x = ret_IDF(word);
                for(const auto& [document_id, freq] : documents_.at(word)){
                  document_to_relevance[document_id] += freq * x;  
        }
            }  
   }                    
            
    for(const string& word : query_words.minus_words){
        if(documents_.count(word) != 0){
            for(const auto& [document_id, freq] : documents_.at(word)){
                document_to_relevance.erase(document_id);
            }
        }   
        }
        vector<Document> matched_documents;
        for (const auto& [key,value]:document_to_relevance){
            matched_documents.push_back({key, value});
        }
        return matched_documents;
    }    
};    


SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
             << "relevance = "s << relevance << " }"s << endl;
    }
}
