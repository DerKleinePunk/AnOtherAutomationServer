#include "CurrentUserData.hpp"

#include <uuid/uuid.h>

CurrentUserData::CurrentUserData(/* args */)
{
}

CurrentUserData::~CurrentUserData()
{
}

const std::string CurrentUserData::GetSession() const
{
    return _sessionId;
}

const std::string CurrentUserData::CreateSession(const std::string& user)
{
    uuid_t id;
    uuid_generate_time_safe(id);
    char uuid_str[37]; // ex. "1b4e28ba-2fa1-11d2-883f-0016d3cca427" + "\0"
    uuid_unparse_upper(id, uuid_str);
    _sessionId = std::string(uuid_str);
    _user = user;

    return _sessionId;
}