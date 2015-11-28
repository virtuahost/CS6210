#ifndef URL_PROCESSING_H
#define URL_PROCESSING_H
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/select.h>
#include <curl/curl.h>


// size_t writeCallback(char* buf, size_t size, size_t nmemb, void* up);
// std::string fetch_url(std::string input_url) ;
// size_t onGetDocument(void *contents, size_t size, size_t nmemb, void *userp);
std::string fetch_url(std::string url);


#endif