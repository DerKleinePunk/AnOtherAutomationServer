
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

    //char buff[1024];
    //bzero(buff, sizeof(buff));
    //  * A zero byte.
    //  * VarInt denoting the size of the message object. (type is not part of this)
    //  * VarInt denoting the type of message.
    //  * The message object encoded as a ProtoBuf message
    ESPHomeApi::HelloRequest request;
    request.set_api_version_major(1);
    request.set_api_version_minor(7);
    request.set_client_info("MNE-BACkend");

    auto size = request.ByteSizeLong();
    auto reqestBuffer = new char[size];

    request.SerializeToArray(reqestBuffer, size);

    //request.encode();

    char buff[1024] = {0x00,0x1b,0x01,0x0A,0x15,0x45,0x53,0x50,0x48,0x6f,0x6d,0x65,0x20,0x4c,0x6f,0x67,0x73,0x20,0x32,0x30,0x32,0x33,0x2e,0x35,0x2e,0x34,0x10,0x01,0x18,0x07};

    /*uint16_t messageType = 1;
    int length = request_s.size();
    buff[0] = 0x00;
    buff[1] = length >> 8; //Message Len
    buff[2] = length;
    buff[3] = messageType >> 8; //Message Type
    buff[4] = messageType;

    for (int i=0;i<length;++i) {
        buff[i+5] = request_s[i];
    }*/

    send(_socket_fd, buff, 30, 0);

    valread = read(_socket_fd, buff, 1024);
    std::string buffer(buff, valread);
    printf("From Server : %s\n", utils::GetPrintAbleString(buffer).c_str());

    valread = read(_socket_fd, buff, 1024);
    std::string buffer2(buff, valread);
    printf("From Server : %s\n", utils::GetPrintAbleString(buffer2).c_str());

    close(_socket_fd);

    return 0;
}