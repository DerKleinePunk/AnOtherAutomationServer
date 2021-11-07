#pragma once

#include "Config.hpp"

class GlobalFunctions
{
private:
    Config* _config;
public:
    GlobalFunctions(Config* config);
    ~GlobalFunctions();

    bool IsApiKeyOk(const std::string& value);
    const std::string GetContentTypeFromFileName(const std::string& fileName) const;
    uint16_t GetServerPort() const;
};

