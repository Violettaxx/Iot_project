#include "esp8266.h"
#include <SoftwareSerial.h>
Wifi::Wifi(byte _rxd,byte _txd,byte _pad) : SoftwareSerial(_rxd,_txd)
{
  chpad = _pad;
  error_no = -1;
}
void Wifi::enable_chpad()
{
  pinMode(chpad,OUTPUT);
  digitalWrite(chpad,HIGH);
}
bool Wifi::check_busy()
{
  String msg;
  int loop_cnt = 500;
  flush();
  print("AT\r\n");
  do{
      if(available())
      {
        msg = readString();
        #if defined(DEBUG)
        Serial.println(msg);
        #endif
        if(msg.indexOf("OK") != -1)
        {
          return true;
        }
        else
        {
          return false;
        }
      }
      delay(100);
    }while(loop_cnt--);
    Serial.println("esp8266 not found");
    error_no = TIMEOUT;
    return false;
}
bool Wifi::send_cmd(const char* command,const char *response)
{
  String msg;
  int loop_cnt = 100;
  while(!check_busy())
  {
    #if defined(DEBUG)
    Serial.println("esp8266 is busy wait 0.5s");
    #endif
    delay(500);
  }
  flush();//清空缓冲区
  print(command);//发送AT指令
  while(loop_cnt--)
  {
    if(available())
    {
      msg = readString();
      #if defined(DEBUG)
      Serial.print("response raw info:");
      Serial.println(msg);
      #endif
      if(msg.indexOf(response) != -1)
      {
        return true;
      }
      else if(msg.indexOf("ERROR") != -1)
      {
        return false;  
      }
      else
      {
      }
    }
    delay(BUSY_WAIT);//命令执行需要时间
    #if defined(DEBUG)
    Serial.println("wait for response,as the cmd need enough time to excute");
    #endif
  }
  error_no = TIMEOUT;
  return false;
}
bool Wifi::send_cmd(const char* command)
{
  while(!check_busy())
  {
    #if defined(DEBUG)
    Serial.println("esp8266 is busy wait 0.5s");
    #endif
    delay(500);
  }
  flush();
  print(command);
  return true;
}
 bool Wifi::send_cmd(const char* command,char **res,byte n)
{
  String msg;
  int loop_cnt = 100;
  while(!check_busy())
  {
    #if defined(DEBUG)
    Serial.println("esp8266 is busy wait 0.5s");
    #endif
    delay(500);
  }
  flush();//清空缓冲区
  print(command);//发送AT指令
  while(loop_cnt--)
  {
    if(available())
    {
      msg = readString();
      #if defined(DEBUG)
      Serial.print("response raw info:");
      Serial.println(msg);
      #endif
      for(byte i = 0;i<n;i++)
      {
         if(msg.indexOf(res[i]) != -1)
         {
            return true;
         }
      }
      if(msg.indexOf("FAILED") != -1)
      {
        error_no = FAILED;
        return false;
      }
      else if(msg.indexOf("ERROR") != -1)
      {
        error_no = ERR;
        return false;  
      }
      else
      {
      }
    }
    delay(BUSY_WAIT);//因为busy，所以延时等待回应
    #if defined(DEBUG)
    Serial.println("wait for response,as the cmd need enough time to excute");
    #endif
  }
  error_no = TIMEOUT;
  return false;
}
bool Wifi::init(bool echo)
{
   begin(9600);
   setTimeout(100);
   enable_chpad();//使能ESP8266
   delay(1000);
   if(echo)
   {
    send_cmd("ATE1\r\n");//使能回显
   }
   else
   {
    send_cmd("ATE0\r\n");//禁止回显
   }
   delay(1000);
   return true;
}
bool Wifi::connect_to_router(const char *ssid,const char *pwd)
{
   char package[60];
   memset(package,0,sizeof(package));
   sprintf(package,"AT+CWJAP=\"%s\",\"%s\"\r\n",ssid,pwd);
   send_cmd("ATE0\r\n");//禁止回显
   delay(1000);
   while(true)
  {
      //config mode
      if(!send_cmd("AT+CWMODE=3\r\n","OK"))
      {
        Serial.println("config mode failed");
        Serial.println("start reset esp8266");
        delay(2000);
        send_cmd("AT+RST\r\n");
        delay(3000);
        continue;
      }
      send_cmd("AT+RST\r\n");
      delay(3000);
      Serial.print("ready to connect router:");
      Serial.println(ssid);
      send_cmd("ATE0\r\n");//禁止回显
      char *res_array[2]={"OK","ALREAD CONNECTED"};
      if(!send_cmd(package,res_array,2))
      {
        Serial.println("connect to router failed");
        continue;
      }
      break;
  }
  return true;
}
bool Wifi::connect_to_server(const char *protocal,const char *ip,int port)
{
  while(true)
  {
      if(!send_cmd("AT+CIPMUX=1\r\n","OK"))
      {
          Serial.println("multi connect config failed");
          continue;
      }
      char *res_array[2]={"OK","ALREAD CONNECTED"};
      char package[60];
      memset(package,0,sizeof(package));
      sprintf(package,"AT+CIPSTART=4,\"%s\",\"%s\",%d\r\n",protocal,ip,port);
      if(!send_cmd(package,res_array,2))
      {
        Serial.println("connect to server failed");
        continue;
      }
      break;
  }
  return true;
}
bool Wifi::send_data(int num)
{
  char buf[10];
  memset(buf,0,sizeof(buf));
  sprintf(buf,"%d",num);
  //request send data
  char package[30]={'\0'};
  sprintf(package,"AT+CIPSEND=4,%d\r\n",strlen(buf));
  send_cmd(package);
  send_cmd(buf);
  String msg;
  while(1)
  {
    if(available())
    {
      msg = readString();
      if(msg.indexOf("SEND OK") != -1)
      {
        return true;
      }
    }
  }
  return false;
}
void Wifi::test()
{
  String msg;
  if(Serial.available())
  {
    msg = Serial.readString();
    this->print(msg);
  }
  if(this->available())
  {
    msg = this->readString();
    Serial.println(msg);
  }
}
//发送字符串
 bool Wifi::send_data(const char *pack)
 {
    char at_pack[30]={'\0'};
    sprintf(at_pack,"AT+CIPSEND=4,%d\r\n",strlen(pack));
    print(at_pack);
    delay(100);
    print(pack);
    return true;
 }
 //发送传感器数据包
 bool Wifi::send_sensor_dat(const char *dev_id,const char *dev_pwd,int sensor_id,float val)
 {
    char sensor_buf[10];
    memset(sensor_buf,0,sizeof(sensor_buf));
    int val_int = val;
    int val_float = (val - val_int) * 100;
    sprintf(sensor_buf,"%d.%d",val_int,val_float);
    char buf[100];
    memset(buf,0,sizeof(buf));
    sprintf(buf+4,"32A01%s%s10040B%s1%3d%d%s05",dev_id,dev_pwd,USR_ID,sensor_id,strlen(sensor_buf),sensor_buf);

    int total_len = strlen(buf+4) + 4;
    buf[0] = total_len / 1000 % 10 + '0';
    buf[1] = total_len / 100 % 10 + '0';
    buf[2] = total_len / 10 % 10 + '0';
    buf[3] = total_len % 10 + '0';
    send_data(buf);
    #if defined(DEBUG)
    Serial.println(buf);   
    #endif
    return true;
 }
 //发送心跳包
 bool Wifi::send_heart_pack(const char *dev_id,const char *dev_pwd)
 {
    char buf[46];
    memset(buf,0,sizeof(buf));
    sprintf(buf,"004532A01%s%s100101hi05",dev_id,dev_pwd);
    send_data(buf);
    #if defined(DEBUG)
    Serial.println(buf);   
    #endif
    return true;
 }
 
