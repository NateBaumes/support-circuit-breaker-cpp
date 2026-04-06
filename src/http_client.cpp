#include "http_client.h"
#include <iostream>

HttpClient::HttpClient() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl_ = curl_easy_init();
}

HttpClient::~HttpClient() {
    if (curl_) {
        curl_easy_cleanup(curl_);
    }
    curl_global_cleanup();
}

std::optional<HttpResponse> HttpClient::post_json(const std::string& url,
                                                   const std::string& json_body,
                                                   const std::string& api_key) {
    if (!curl_) {
        return std::nullopt;
    }
    
    HttpResponse response;
    response.status_code = 0;
    response.elapsed_seconds = 0;
    
    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, json_body.c_str());
    curl_easy_setopt(curl_, CURLOPT_TIMEOUT, 30L);
    
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    
    if (!api_key.empty()) {
        std::string auth_header = "Authorization: Bearer " + api_key;
        headers = curl_slist_append(headers, auth_header.c_str());
    }
    
    curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers);
    
    std::string response_body;
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response_body);
    
    CURLcode res = curl_easy_perform(curl_);
    
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &response.status_code);
        response.body = response_body;
        return response;
    }
    
    curl_slist_free_all(headers);
    return std::nullopt;
}

std::optional<HttpResponse> HttpClient::get(const std::string& url) {
    if (!curl_) {
        return std::nullopt;
    }
    
    HttpResponse response;
    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl_, CURLOPT_TIMEOUT, 10L);
    
    std::string response_body;
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response_body);
    
    CURLcode res = curl_easy_perform(curl_);
    
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &response.status_code);
        response.body = response_body;
        return response;
    }
    
    return std::nullopt;
}

size_t HttpClient::write_callback(void* contents, size_t size, 
                                   size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append(static_cast<char*>(contents), total_size);
    return total_size;
}
