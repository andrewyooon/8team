#pragma once

#ifndef __lgdemo_mqtt_h__
#define __lgdemo_mqtt_h__

#include "mosquitto.h"
#include "mqtt_protocol.h"

#include <functional>
#include <vector>
#include <string>
#include <map>

struct mqtt_secure_option
{
  bool secure{};
  std::string cafile{};
  std::string capath{};
  std::string certfile{};
  std::string keyfile{};

  mqtt_secure_option() = default;
  mqtt_secure_option(bool secure, const char* cafile, const char* capath, const char* certfile, const char* keyfile)
    : secure{secure}, cafile{cafile}, capath{capath}, certfile{certfile}, keyfile{keyfile} {}
};

class mqtt
{
public:
  mqtt() = default;
  ~mqtt();

  void secure(bool secure, const char* cafile = {}, const char* capath = {}, const char* certfile = {}, const char* keyfile = {});
  int connect(const char* ipAddr, int keepAlive);
  
  void regTopic(const std::string& topicName);
  void regTopic(const std::string& topicName, std::function<void(void*, int)> handler);
  
  int subscribeTopic(const char* topic, int qos);
  int unsubscribeTopic(const char* topic);

  void publish(const char* topic, const void* msgBuf, int msgLen, int qos);
  
  void recvMessage(const struct mosquitto_message* msg);
  void dispatchMsg();

  inline bool isConnected() const { return connected; }
  void mqtt_loop_thread();

private:
  int init();
  void addMsgHandler(std::string topicName, std::function<void(void*, int)> handler = {});

  bool initDone{};
  bool connected{};

  struct mosquitto* mosq{};

  std::vector<std::string> mqtt_topics;

  std::map<std::string, std::function<void(void*, int)>> msgHandlers;
  std::vector<struct mosquitto_message> mqtt_msgs;

  mqtt_secure_option secure_opt{};
};

#endif // __lgdemo_mqtt_h__