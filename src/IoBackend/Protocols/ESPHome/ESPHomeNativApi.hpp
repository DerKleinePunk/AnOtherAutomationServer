#pragma once

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <iomanip>
#include <fcntl.h>

#include <arpa/inet.h>
#include <iostream>
#include <sstream>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <api.pb.h>
#pragma GCC diagnostic warning "-Wdeprecated-declarations"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

class ESPHomeNativApi
{
private:
    int _socket_fd;
public:
    ESPHomeNativApi(/* args */);
    ~ESPHomeNativApi();

    int Connect(const std::string ip, unsigned short port = 6053);

};


