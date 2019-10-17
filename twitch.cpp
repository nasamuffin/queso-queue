#include "twitch.h"

#include <curl/curl.h>
#include <iostream>
#include <string>

static size_t CurlWriteCallback(void *contents, size_t size, size_t nmemb,
                                void *userp) {
    reinterpret_cast<std::string*>(userp)->append(
        reinterpret_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

bool Twitch::isOnline(std::string username, std::string channel) {
    CURLcode res;
    std::string channelUrl = "https://tmi.twitch.tv/group/user/";
    channelUrl += channel;
    channelUrl += "/chatters";

    std::string response;

    CURL *curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, channelUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "Oh no! curl_easy_perform() failed: "
                      << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup;
    }

    // This is a nasty hack.
    return response.find(username) != response.npos;
}

void placeStreamMarker() {
    // stub
}
