
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <winsock2.h>
#include <windows.h>

#include "mosquitto.h"
#include "mqtt_main.h"

typedef enum {
	MQ_FAIL = -1,
	MQ_SUCCESS = 0,
}E_MP_CONFIG;

#define MQ_LOOP_THREAD_RUN					1
#define MQ_LOOP_THREAD_STOP					0

#define TOPIC_LEN	10

typedef struct
{
	char Topic1[TOPIC_LEN];
	char Topic2[TOPIC_LEN];
	char Topic3[TOPIC_LEN];
	char MQTT_Server[128];
	int MQTT_Server_Port;
	char InstanceName[128];
	struct mosquitto* MqttInstance;

	int ThreadState;
	HANDLE ThreadID;
} S_MQContext;

static S_MQContext s_MQ;
S_MQContext* g_pMQ;


void sendToMQTT(int topic, char* mqttMsg, size_t length)
{
	//	-	GNSS Raw measurement를 전달
	//	-	GNSS에서 함수 호출시 pMsg 내용을 MQTT message로 전송
	int ret = MQ_FAIL;

	
	if (topic == 1)
	{
		ret = MQ_PublishMessage(g_pMQ->MqttInstance, g_pMQ->Topic1, length, mqttMsg, 0);
		printf("MQ_ sendToMQTT, %s, %d", g_pMQ->Topic1, ret);
	}
	else if (topic == 2)
	{
		ret = MQ_PublishMessage(g_pMQ->MqttInstance, g_pMQ->Topic2, length, mqttMsg,0);
		printf("MQ_ sendToMQTT, %s, %d", g_pMQ->Topic2, ret);
	}
	else if (topic == 3)
	{
		ret = MQ_PublishMessage(g_pMQ->MqttInstance, g_pMQ->Topic3, length, mqttMsg,0);
		printf("MQ_ sendToMQTT, %s, %d", g_pMQ->Topic3, ret);
	}
	else printf("MQ_ sendToMQTT, invalid topic");
	
}


void MQ_LoopThreadStop(void)
{
	if (g_pMQ->ThreadState != MQ_LOOP_THREAD_STOP) g_pMQ->ThreadState = MQ_LOOP_THREAD_STOP;
}

/*
 * int rc : return code
 *      0 : success
 *      1 : connection refused(unacceptable protocol version)
 *      2 : connection refused(id reject)
 *      3 : connection refused(broker unavailable)
 */
void MQ_MQTT_Connect(struct mosquitto* mosq, void* obj, int rc, int flag, const mosquitto_property* property)
{
	printf("MQ_MQTT_Connect callback %d", rc);
}
/*
 * int rc :  indicating the reason for the disconnect
 *      0 : client disconnect
 *      other  : disconnect is unexpected
 */
void MQ_MQTT_Disconnect(struct mosquitto* mosq, void* obj, int rc, const mosquitto_property* property)
{
	int rc_disconnect = rc;
	char* reason_string = NULL;
	const mosquitto_property* reason_property;

	reason_property = mosquitto_property_read_string(property, MQTT_PROP_REASON_STRING, &reason_string, false);

	printf("MQ_MQTT_Disconnect callback %d, %s", rc, reason_string);


	MQ_LoopThreadStop();
}

void MQ_MQTT_Subscribe(struct mosquitto* mosq, void* obj, int mid, int qos_count, const int* granted_qos, const mosquitto_property* properties)
{
	printf("MQ_MQTT_Subscribe callback");
}

void MQ_MQTT_Message(struct mosquitto* mosq, void* obj, const struct mosquitto_message* message, const mosquitto_property* properties)
{
	char* RecvMsg;
	int RecvMsgLen = 0;
	char* RecvTopic;
	unsigned int RecvTopicLen;

	if ((message == NULL) || (message->payload == NULL))
	{
		printf("MQ_MQTT_Message message is null");
		return;
	}

	printf("MQ_MQTT_Message callback payloadlen = %d, payload = %s", message->payloadlen, message->payload);

	RecvMsg = (char*)malloc(message->payloadlen + 1);
	if (RecvMsg == NULL) {
		printf("MQ_MQTT_Message RecvMsg malloc fail");
		return;
	}

	memset(RecvMsg, 0, message->payloadlen + 1);
	memcpy(RecvMsg, message->payload, message->payloadlen);
	RecvMsg[message->payloadlen] = '\0';
	RecvMsgLen = message->payloadlen;

	RecvTopicLen = strlen(message->topic);
	RecvTopic = (char*)malloc(RecvTopicLen + 1);

	if (RecvTopic == NULL) {
		printf("MQ_MQTT_Message RecvTopic malloc fail");
		free(RecvMsg);
		return;
	}

	//memset(RecvTopic, 0, RecvTopicLen);
	memcpy(RecvTopic, message->topic, RecvTopicLen);
	RecvTopic[RecvTopicLen] = '\0';

	if (!memcmp(RecvTopic, g_pMQ->Topic1, RecvTopicLen))
	{
		printf("MQ_MQTT_Message Topic %s", RecvTopic);
		MQ_receiveFromMQTT(RecvTopic, RecvMsg, RecvMsgLen);
	}
	else if (!memcmp(RecvTopic, g_pMQ->Topic2, RecvTopicLen))
	{
		printf("MQ_MQTT_Message Topic %s", RecvTopic);
		MQ_receiveFromMQTT(RecvTopic, RecvMsg, RecvMsgLen);
	}
	else if (!memcmp(RecvTopic, g_pMQ->Topic3, RecvTopicLen))
	{
		printf("MQ_MQTT_Message Topic %s", RecvTopic);
		MQ_receiveFromMQTT(RecvTopic, RecvMsg, RecvMsgLen);
	}
	else
	{
		printf("MQ_MQTT_Message Topic not matched");
	}

	free(RecvMsg);
	free(RecvTopic);
}

int MQ_Config(void)
{
	memcpy(g_pMQ->Topic1, "GNSSMEAS", strlen("GNSSMEAS"));
	memcpy(g_pMQ->Topic2, "LOC", strlen("LOC"));
	memcpy(g_pMQ->Topic3, "PDR", strlen("PDR"));
	memset(g_pMQ->MQTT_Server, 0, sizeof(g_pMQ->MQTT_Server));
	g_pMQ->MQTT_Server_Port = 1883;
	memcpy(g_pMQ->MQTT_Server, "13.209.41.151", strlen("13.209.41.151"));

	printf("MQ_Config : Server Address %s:%d", g_pMQ->MQTT_Server, g_pMQ->MQTT_Server_Port);

	return MQ_SUCCESS;
}


void* MQ_LoopMainThread(void* loop_mosq)
{
	g_pMQ->ThreadState = MQ_LOOP_THREAD_RUN;
	printf("MQ_LoopMainThread Start");

	while (g_pMQ->ThreadState == MQ_LOOP_THREAD_RUN) {
		mosquitto_loop((struct mosquitto*)loop_mosq, -1, 1);
	}

	printf("MQ_LoopMainThread End");

	//pthread_exit(NULL);
}

int MQ_Start(void)
{
	int ret = 0, keepalive = 3600;
	printf("MQ_Start : connect : %s, port : %d", g_pMQ->MQTT_Server, g_pMQ->MQTT_Server_Port);

	if (g_pMQ->MQTT_Server_Port != 1883)
	{
		printf("MQ_Start : port error");
		mosquitto_lib_cleanup();
		return MQ_FAIL;
	}
	// MQTT client Connect start
	ret = mosquitto_connect(g_pMQ->MqttInstance, g_pMQ->MQTT_Server, g_pMQ->MQTT_Server_Port, keepalive);
	if (ret != 0) {
		printf("MQ_Start : mosquitto_connect error %d", ret);
		mosquitto_lib_cleanup();
		return MQ_FAIL;
	}

	//Create MQ Loop Thread - Real MQTT Thread
	if (!(g_pMQ->ThreadID = CreateThread(NULL, 0, MQ_LoopMainThread, (void*)g_pMQ->MqttInstance, 0, NULL))) {
	{
		printf("MQ_Start : MQ_Loop_Thread create error");
		mosquitto_lib_cleanup();
		return MQ_FAIL;
	}

	printf("MQ_Start Complete");

	return MQ_SUCCESS;
}

int MQ_PublishMessage(struct mosquitto* mosq, const char* topic, unsigned int payloadlen, const void* payload, int qos)
{
	int ret;

	if (payload == NULL) {
		//Input Parameter Error
		printf("MQ_Publish_Message Input Parameter Error Payload length %d", payloadlen);
		return MQ_FAIL;
	}

	ret = mosquitto_publish_v5(mosq, NULL, topic, payloadlen, payload, qos, false, NULL);
	printf("MQ_PublishMessage %d, %s", ret, payload);
	return ret;
}

int MQ_Subscribe(struct mosquitto* mosq, const char* topic, int qos)
{
	int ret;
	ret = mosquitto_subscribe_v5(mosq, NULL, (const char*)topic, qos, 4, NULL);
	printf("MQ_Subscribe %d", ret);
	return ret;
}

/*
 * MP_Multiple_UnSubscribe
 * return value : int
 *     -1 : Fail
 *      0 : Success
 * input value :
 *		S_MESSGING_MPINSTANCE *mosq	: MQTT instance
 *		char *const *const sub		: topic
 */
int MQ_UnSubscribe(struct mosquitto* mosq, const char* topic)
{
	int ret;

	ret = mosquitto_unsubscribe_v5(mosq, NULL, (const char*)topic, NULL);
	printf("MQ_UnSubscribe");
	return ret;
}

void MQ_receiveFromMQTT(void* pTopic, void* pMsg, int len)
{
	int topicLen = strlen(pTopic);

	printf("MQ_receiveFromMQTT : %s,  %d, %s\n", pTopic, len, pMsg);

}


int MQ_Init() {
	int mqtt_version;
	int ret;

	g_pMQ = &(s_MQ);

	memset(g_pMQ->Topic1, 0, TOPIC_LEN);
	memset(g_pMQ->Topic2, 0, TOPIC_LEN);
	memset(g_pMQ->Topic3, 0, TOPIC_LEN);
	memset(g_pMQ->MQTT_Server, 0, 128);
	g_pMQ->MQTT_Server_Port = 0;
	memset(g_pMQ->InstanceName, 0, 128);
	g_pMQ->MqttInstance = NULL;
	g_pMQ->ThreadState = MQ_LOOP_THREAD_STOP;
	g_pMQ->ThreadID = 0;
	g_pMQ->Mode = -1;

	//MP Configuration and MQTT Lib init
	if ((MQ_Config() != MQ_SUCCESS) || (mosquitto_lib_init() != 0)) {
		printf("MQ_Config Error or lib init failed");
		return MQ_FAIL;
	}
	//Create MQTT instance
	//if (strlen(g_pMQ->InstanceName) <= 0) {
	//	MQ_CreateInstanceName(g_pMQ->InstanceName);
	//}
	g_pMQ->MqttInstance = mosquitto_new(g_pMQ->InstanceName, true, NULL);

	if (g_pMQ->MqttInstance == NULL) {
		printf("MQ_Start : MQTT instance create fail");
		return MQ_FAIL;
	}

	//Register Callback Function
	mosquitto_connect_v5_callback_set(g_pMQ->MqttInstance, MQ_MQTT_Connect);
	mosquitto_disconnect_v5_callback_set(g_pMQ->MqttInstance, MQ_MQTT_Disconnect);
	mosquitto_subscribe_v5_callback_set(g_pMQ->MqttInstance, MQ_MQTT_Subscribe);
	mosquitto_message_v5_callback_set(g_pMQ->MqttInstance, MQ_MQTT_Message);

	// MQTT Version Setting
	mqtt_version = MQTT_PROTOCOL_V5;
	ret = mosquitto_opts_set(g_pMQ->MqttInstance, MOSQ_OPT_PROTOCOL_VERSION, &mqtt_version);

	if (g_pMQ->ThreadState == MQ_LOOP_THREAD_STOP)
		ret = MQ_Start();
	ret = MQ_Subscribe(g_pMQ->MqttInstance, g_pMQ->Topic1, 0);
	ret = MQ_Subscribe(g_pMQ->MqttInstance, g_pMQ->Topic2, 0);
	ret = MQ_Subscribe(g_pMQ->MqttInstance, g_pMQ->Topic3, 0);

	return MQ_SUCCESS;
}
