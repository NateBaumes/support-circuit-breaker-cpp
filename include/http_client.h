#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <string>
#include <optional>
#include <curl/curl.h>

struct HttpResponse {
    int status_code;
    std::string body;
    double elapsed_seconds;
};

class HttpClient {
public:
    HttpClient();
    ~HttpClient();
    
    // POST JSON to endpoint
    std::optional<HttpResponse> post_json(const std::string& url, 
                                           const std::string& json_body,
                                           const std::string& api_key = "");
    
    // GET request
    std::optional<HttpResponse> get(const std::string& url);

private:
    CURL* curl_;
    static size_t write_callback(void* contents, size_t size, 
                                  size_t nmemb, std::string* output);
};

#endif
