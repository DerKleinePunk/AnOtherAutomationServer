#pragma once

#include <termios.h>
#include <fstream>
#include <string>
#include <thread>

#define MAXBUFFERSIZE 2048

class SerialBase
{
  private:
    int _handle;
    std::string _portname;
    termios _oldtio;
    termios _newtio;
    struct epoll_event* _epoll_events;
    int _efd;
    std::thread loop_thread_;
    bool _run;
    std::ofstream _rawLogFile;
    std::string _rawLogFileName;
    void SetSpeed(speed_t speed);
    void SetParity();
    void Loop();
  protected:
    int pipefdwrite_;
    int currentReadPos_;
    char readBuffer_[MAXBUFFERSIZE];
    virtual void AnalyseBuffer(int count) = 0;

  public:
    SerialBase(const std::string& portname);
    SerialBase(const SerialBase& orig) = delete;
	SerialBase(SerialBase&& other) = delete;
	SerialBase& operator=(const SerialBase& other) = delete;
	SerialBase& operator=(SerialBase&& other) = delete;
    virtual ~SerialBase();

    bool Open(const std::string& speed);
    bool Open(speed_t speed);
    int GetHandle() const;
    int Read();
    int Write(const std::string& message);
    int Start();
    void Close();
    void SetRawLogFile(const std::string& name);
};


