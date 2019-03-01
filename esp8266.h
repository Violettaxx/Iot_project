#include <SoftwareSerial.h>

#ifndef __ESP8266_H
#define __ESP8266_H
#include <Arduino.h>

typedef unsigned long ul_t;
#define TXD 8
#define RXD 9
#define DEBUG
#define SERVER_IP "115.28.93.201"
#define SERVER_PORT 7005
#define ROUTER_SSID "xueshubu_xiaomi"
#define ROUTER_PWD "xueshubu"
#define BUSY_WAIT 100
#define DEV_ID "2001370405"
#define DEV_PWD "23c485fd964250b1"
#define USR_ID "1020407057"
#define SENSER_ID "321"
#define BUSY_WAIT 100
class Wifi : public SoftwareSerial
{
  public:
    enum{FAILED = 0,ERR,TIMEOUT,BUSY};
    Wifi(byte _rxd,byte _txd,byte _pad);
    bool init(bool);
    bool check_busy();
    bool connect_to_server(const char *protocal,const char *ip,int port);
    bool connect_to_router(const char *ssid,const char *pwd);
    bool send_cmd(const char* command);
    bool send_cmd(const char* command,const char *response);
    bool send_cmd(const char* command,char **res,byte n);
    bool send_sensor_dat(const char *dev_id,const char *dev_pwd,int sensor_id,float val);
    bool send_heart_pack(const char *dev_id,const char *dev_pwd);
    bool send_data(int);
    bool send_data(const char *);
    void enable_chpad();
    void test();
    byte error_no;
  private:
    byte chpad;
};
#endif

