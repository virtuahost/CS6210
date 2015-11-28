#include "url_processing.h"

using namespace std;
string data;

static size_t onGetDocument(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    ((string*)userp)->append((char*)contents, realsize);
    return realsize;
  }

string fetch_url(string url)
{
  CURL *curl;
    CURLcode res;
    string resBody;

    curl = curl_easy_init();

    if (curl) {
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, onGetDocument);
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resBody);

      res = curl_easy_perform(curl);

      if (res != CURLE_OK) {
        cerr << "response is unsuccessful" << endl;
        throw 1;
      }

      curl_easy_cleanup(curl);
    } else {
      cerr << "failed to initialize curl instance" << endl;
      throw 1;
    }

    return resBody;
}