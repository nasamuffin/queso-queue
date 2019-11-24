#include "twitch.h"

#include <chrono>
#include <ctime>
#include <curl/curl.h>
#include <iostream>
#include <iomanip>
#include <map>
#include <string>
#include "SimpleJSON/JSON.h"

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
        curl_easy_cleanup(curl);
    }

    // This is a nasty hack.
    return response.find(username) != response.npos;
}

std::set<std::string> Twitch::getOnlineUsers(const std::string& channel) {
    CURLcode res;
    std::string channelUrl = "https://tmi.twitch.tv/group/user/";
    channelUrl += channel;
    channelUrl += "/chatters";

    std::set<std::string> online_users;
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
        curl_easy_cleanup(curl);
    }

    JSONValue *chatters = JSON::Parse(response.c_str())->Child(L"chatters");
    for (auto& sub_chatter_group : chatters->ObjectKeys()) {
        for (auto& user : chatters->Child(sub_chatter_group.c_str())->AsArray()) {
            auto online_user = user->AsString();
            online_users.emplace(std::string(online_user.begin(), online_user.end()));
        }
    }

    auto current_time = std::chrono::system_clock::now();
    auto user_snapshot = _recent_chatters;
    _recent_chatters = {};
    for(auto const& [user, last_heard_from] : user_snapshot) {
        //TODO: This time should be runtime configurable.
        if (current_time - last_heard_from < std::chrono::minutes(5)) {
            online_users.emplace(user);
            _recent_chatters.emplace(user, last_heard_from);
        }
    }

    return online_users;
}

void Twitch::markAsOnline(std::string username) {
    auto current = std::chrono::system_clock::now();
    _recent_chatters.emplace(username, current);
}

void placeStreamMarker() {
    // stub
}
