#define MQTTCLIENT_QOS2 1

#include "MQTTEthernet.h"
#include "MQTTClient.h"

// Initialize a pins to perform analog input and digital output fucntions
AnalogIn   ain(A0);
DigitalOut dout(LED1);
int status = 0;

int arrivedcount = 0;
DigitalOut myRelay(D0);

void messageArrived(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;
    printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\n", message.qos, message.retained, message.dup, message.id);
    char * Data = (char*)message.payload;
    printf("Payload %.*s\n", message.payloadlen, (char*)message.payload);
    
    //Switching Relay
    if(strcmp(Data, "Someone is there !!!") == 0)
       myRelay = 1;
       
       
    //if(strcmp(Data, "0") == 0)
    else
       myRelay = 0;
       
    ++arrivedcount;
}


int main(int argc, char* argv[])
{   
    MQTTEthernet ipstack = MQTTEthernet();
    float version = 0.5;
    char* topicP = "presence";
    char* topicS = "sub_sample";
    
    printf("HelloMQTT: version is %f\n", version);
              
    MQTT::Client<MQTTEthernet, Countdown> client = MQTT::Client<MQTTEthernet, Countdown>(ipstack);
    
    char* hostname = "10.208.37.147";
    int port = 1883;
    printf("Connecting to %s:%d\n", hostname, port);
    int rc = ipstack.connect(hostname, port);
    if (rc != 0)
        printf("rc from TCP connect is %d\n", rc);
 
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;       
    data.MQTTVersion = 3;
    data.clientID.cstring = "mbed-sample";
    data.username.cstring = "testuser";
    data.password.cstring = "testpassword";
    if ((rc = client.connect(data)) != 0)
        printf("rc from MQTT connect is %d\n", rc);
    
    if ((rc = client.subscribe(topicS, MQTT::QOS2, messageArrived)) != 0)
        printf("rc from MQTT subscribe is %d\n", rc);

    MQTT::Message message;

    // QoS 0
    char buf[100];
    sprintf(buf, "1");
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void*)buf;
    message.payloadlen = strlen(buf)+1;
    while(1)
    {
        if(ain > 0.3f)
        {
          printf("Someone Arrived Publishing to MQTT\n");
          rc = client.publish(topicP, message);
          
          dout = 1;
          while (ain > 0.3f)
             client.yield(100);
          dout = 0;
        }
        else
        { 
          printf("No one is there \n");
          myRelay = 0;
          while(ain < 0.3f)
             client.yield(100);
        }
    }
        
   
}
