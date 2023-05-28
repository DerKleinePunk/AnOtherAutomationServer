#pragma once

#include <string>

class mDnsClient
{
private:
    /* data */
public:
    mDnsClient(/* args */);
    ~mDnsClient();

    void ScanNetwork(const std::string serviceType);
};


