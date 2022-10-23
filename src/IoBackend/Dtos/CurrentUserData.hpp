#pragma once

#include <string>

class CurrentUserData
{
private:
    std::string _sessionId;
    std::string _user;
public:
    CurrentUserData(/* args */);
    ~CurrentUserData();

    const std::string GetSession() const;
    const std::string CreateSession(const std::string& user);
};

