
#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "ESPHomeNativApi"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "../../../common/easylogging/easylogging++.h"
#include "../../../common/utils/commonutils.h"
#include "ESPHomeNativApi.hpp"

ESPHomeNativApi::ESPHomeNativApi(/* args */)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
}

ESPHomeNativApi::~ESPHomeNativApi()
{
}

int ESPHomeNativApi::Connect(const std::string ip, unsigned short port)
{
    if ((_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        LOG(ERROR) << "Socket creation error";
        return -1;
    }

    struct sockaddr_in serv_addr;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0) {
        LOG(ERROR) <<"Invalid address/ Address not supported";
        return -1;
    }

    int status, valread;

    if((status = connect(_socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
        LOG(ERROR) << "Connection Failed";
        return -1;
    }

    char buff[1024];
    bzero(buff, sizeof(buff));
    //  * A zero byte.
    //  * VarInt denoting the size of the message object. (type is not part of this)
    //  * VarInt denoting the type of message.
    //  * The message object encoded as a ProtoBuf message
    ESPHomeApi::HelloRequest request;
    request.set_api_version_major(1);
    request.set_api_version_minor(7);
    request.set_client_info("MNE-BACkend");
    std::string request_s;
    request.SerializeToString(&request_s);
    //request.encode();

    int length = request_s.size();
    buff[1] = length;
    buff[2] = 1;

    for (int i=0;i<length;++i) {
        buff[i+3] = request_s[i];
    }

    send(_socket_fd, buff, length + 2, 0);

    valread = read(_socket_fd, buff, 1024);
    buff[valread] = 0;
    printf("From Server : %s", buff);

    valread = read(_socket_fd, buff, 1024);
    buff[valread] = 0;
    printf("From Server : %s", buff);

    close(_socket_fd);

    return 0;
}