#include "mqtt.h"

#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <utility>
#include <thread>

constexpr int MQTT_SUCCESS = 0;
constexpr int MQTT_FAILED = -1;

void mqtt_connect_callback(struct mosquitto* mosq, void* obj, int rc, int flag, const mosquitto_property* property)
{
  std::cout << "connect callback, " << rc << std::endl;
}

void mqtt_disconnect_callback(struct mosquitto* mosq, void* obj, int rc, const mosquitto_property* property)
{
  char* reason_string = nullptr;
  auto reason_property = mosquitto_property_read_string(property, MQTT_PROP_REASON_STRING, &reason_string, false);

  std::cout << "disconnect callback, " << rc << ", " << reason_string << std::endl;
}

void mqtt_subscribe_callback(struct mosquitto* mosq, void* obj, int mid, int qos_count, const int* granted_qos, const mosquitto_property* properties)
{
  std::cout << "subscribe callback" << std::endl;
}

void mqtt_message_callback(struct mosquitto* mosq, void* obj, const struct mosquitto_message* msg, const mosquitto_property* properties)
{
  if (msg && msg->topic && msg->payload && msg->payloadlen > 0)
  {
    reinterpret_cast<mqtt*>(obj)->recvMessage(msg);
  }
}

mqtt::~mqtt()
{
  if (initDone)
  {
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
  }
}

void mqtt::secure(bool secure, const char* cafile, const char* capath, const char* certfile, const char* keyfile)
{
  if (secure_opt.secure != secure)
  {
    secure_opt = secure ? mqtt_secure_option(secure, cafile, capath, certfile, keyfile) : mqtt_secure_option();
  }
}

int mqtt::init()
{
  if (!initDone)
  {
    if (mosquitto_lib_init() != MOSQ_ERR_SUCCESS)
    {
      std::cout << "Could not init mosquitto lib" << std::endl;
      return MQTT_FAILED;
    }

    mosq = mosquitto_new(NULL, true, this);
    if (!mosq)
    {
      std::cout << "Could not create new mosquitto struct" << std::endl;

      mosquitto_lib_cleanup();
      return MQTT_FAILED;
    }

    mosquitto_connect_v5_callback_set(mosq, mqtt_connect_callback);
    //mosquitto_disconnect_v5_callback_set(mosq, mqtt_disconnect_callback);
    mosquitto_subscribe_v5_callback_set(mosq, mqtt_subscribe_callback);
    mosquitto_message_v5_callback_set(mosq, mqtt_message_callback);

    if (secure_opt.secure)
    {
      mosquitto_tls_set(mosq, secure_opt.cafile.c_str(), secure_opt.capath.c_str(), secure_opt.certfile.c_str(), secure_opt.keyfile.c_str(), nullptr);
    }

    int mqtt_version = MQTT_PROTOCOL_V5;
    mosquitto_opts_set(mosq, MOSQ_OPT_PROTOCOL_VERSION, &mqtt_version);

    initDone = true;
  }

  return MQTT_SUCCESS;
}

int mqtt::connect(const char* ipAddr, int keepAlive)
{
  if (!initDone)
    init();
  
  if (!connected)
  {
    int port = secure_opt.secure ? 8883 : 1883;

    if (mosquitto_connect(mosq, ipAddr, port, keepAlive))
    {
      std::cout << "Unable to connect mosquitto." << std::endl;
      return MQTT_FAILED;
    }

    std::thread t1(&mqtt::mqtt_loop_thread, this);
    t1.detach();

    connected = true;
  }

  return MQTT_SUCCESS;
}

int mqtt::subscribeTopic(const char* topic, int qos)
{
  int ret = mosquitto_subscribe_v5(mosq, NULL, topic, qos, 4, NULL);
  return ret;
}

int mqtt::unsubscribeTopic(const char* topic)
{
  int ret = mosquitto_unsubscribe_v5(mosq, NULL, topic, NULL);
  return ret;
}

void mqtt::publish(const char* topic, const void* msgBuf, int msgLen, int qos)
{
  if (msgBuf)
  {
    int ret = mosquitto_publish_v5(mosq, NULL, topic, msgLen, msgBuf, qos, false, NULL);
    if (ret != MOSQ_ERR_SUCCESS) {
      std::cout << "Error publishing: " << mosquitto_strerror(ret) << std::endl;
    }
  }
}

void mqtt::addMsgHandler(std::string topicName, std::function<void(void*, int)> handler)
{
  msgHandlers.insert_or_assign(topicName, handler);
}

void mqtt::regTopic(const std::string& topicName)
{
  auto it = std::find(mqtt_topics.begin(), mqtt_topics.end(), topicName);
  if (it == mqtt_topics.end())
  {
    mqtt_topics.push_back(topicName);
  }
}

void mqtt::regTopic(const std::string& topicName, std::function<void(void*, int)> handler)
{
  regTopic(topicName);
  addMsgHandler(topicName, handler);
}

void mqtt::recvMessage(const struct mosquitto_message* msg)
{
  mqtt_msgs.push_back(*msg);
}

void mqtt::dispatchMsg()
{
  if (mqtt_msgs.size())
  {
    auto& msg = mqtt_msgs.front();
    try
    {
      std::invoke(msgHandlers[msg.topic], msg.payload, msg.payloadlen);
    }
    catch (std::exception& e) {
    }
    mqtt_msgs.erase(mqtt_msgs.begin());
  }
}

void mqtt::mqtt_loop_thread()
{
  while (1)
  {
    dispatchMsg();

    int loop = mosquitto_loop(mosq, -1, 1);
    if (loop) {
      fprintf(stderr, "mosquitto connection error!\n");
      mosquitto_reconnect(mosq);
    }
  }
}