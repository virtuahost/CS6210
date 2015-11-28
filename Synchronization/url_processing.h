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


std::string fetch_url(std::string url);


#endif