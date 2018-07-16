/*******************************************************************************
 * Copyright (c) 2014, 2015 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *    Ian Craggs - make sure QoS2 processing works, and add device headers
 *******************************************************************************/
 
 /**
  This is a sample program to illustrate the use of the MQTT Client library
  on the mbed platform.  The Client class requires two classes which mediate
  access to system interfaces for networking and timing.  As long as these two
  classes provide the required public programming interfaces, it does not matter
  what facilities they use underneath. In this program, they use the mbed
  system libraries.
 */
 
#define MQTTCLIENT_QOS2 1

#include "ESP8266Interface.h"
#include "MQTTNetwork.h"
#include "MQTTmbed.h"
#include "MQTTClient.h"
#define IAP_LOCATION 0x1FFF1FF1
Serial pc(USBTX, USBRX);

DigitalOut pace_led(LED4);
DigitalOut sense_led(LED1);
InterruptIn sense(p5);
InterruptIn pace(p6);

volatile int s_count = 0;
volatile int p_count = 0;
Timer t;
 
 
void sense_function(){
    sense_led = true;
    s_count++;
    sense_led = false;
    }
 
void pace_function(){
    pace_led = true;
    p_count++;
    pace_led = false;
    }

ESP8266Interface wifi(p28, p27);
//TextLCD lcd(p15,p16,p17,p18,p19,p20, TextLCD::LCD16x2);
TCPSocket socket;
int arrivedcount = 0;
unsigned int command[5];
unsigned int output[5];
//EthernetInterface net;

typedef void (*IAP)(unsigned int [],unsigned int[]);
IAP iap_entry;
int oi;

void messageArrived(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;
    //printf("message = %s\n",&message);
    //printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\r\n", message.qos, message.retained, message.dup, message.id);
    //printf("Payload %.*s\r\n", message.payloadlen, (char*)message.payload);
    //printf("Payload %.*s\r\n", message.payloadlen, (char*)message.payload);
    //printf("sub hi\n");
    ++arrivedcount;
}

int main(int argc, char* argv[]) {
    int a[12];
    int p = 0;
    int w = 0;
    char p_c;
    char w_c;
    printf("\r\n heart monitor enter P\r \n");
    p_c = pc.getc();
    printf("\r\n heart monitor enter W\r \n");
    w_c = pc.getc();
    p = atoi(&p_c);
    w = atoi(&w_c);
    w = w *10;
    printf("p %d \r\n",p);
    printf("w %d \r\n",w);

    //int p = 5;
    //int w = 20;
    int navg = w/p;
    int i = 0;
    int sum = 0;
    int heart_rate = 0;
    float version = 2.0;
    int lowalarm = 0;
    int highalarm = 0;
    int rcount;
    int random;
    int rc;
    char* topic = "Hello_Publish";
    char outputUUID[128];
    char other_string[128];
    char topicbuff[128];
    char topicbuff1[1024];
    int n;
    int pay;
    memset(other_string,0,128);
    memset(outputUUID,0,128);
    memset(topicbuff,0,128);
    memset(topicbuff1,0,1024);
    //pc.printf("heart monitor\r \n");


    memset(a,0,12*sizeof(int));

    /*Socket*/
    wifi.connect("AirPennNet-Device", "penn1740wifi");
    //wifi.connect("Information_Superhighway-2.4", "Apple@0range");
    const char *ip = wifi.get_ip_address();
    const char *mac = wifi.get_mac_address();

    
    /*UUID*/
    iap_entry=(IAP) IAP_LOCATION;
    command[0] = 58;
    //output[0] = 58;
    iap_entry(command, output);
    /*for(oi = 0;oi<5;oi++){
        //printf("UUID output = %02x\n",output[oi]);
        }*/
    n = sprintf(outputUUID,"26013f37-200800bae2a8ca552b0bca2f5001e87",output[1],output[2],output[3],output[4]);
    strcat(other_string, outputUUID);
    //printf("UUID = %s\n",other_string);
    //printf("%s\n",other_string);
    //command[0] = 54;
    
    
    /*MQTT*/
    //ESP8266Interface* network;
    MQTTNetwork mqttNetwork(&wifi);
    MQTT::Client<MQTTNetwork, Countdown> client = MQTT::Client<MQTTNetwork, Countdown>(mqttNetwork) ;
    const char* hostname = "35.188.242.1";
    //const char* hostname = "128.91.162.127";
    int port = 1883;
    rc = mqttNetwork.connect(hostname, port);
    printf("RC = %d\n",rc);
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = other_string;
    data.username.cstring = "mbed";
    data.password.cstring = "homework";
    if ((rc = client.connect(data)) != 0)
        printf("rc from MQTT connect is %d\r\n", rc);
        
    /*Subscribe*/
    sprintf(topicbuff1,"cis541/hw-mqtt/26013f37-200800bae2a8ca552b0bca2f5001e87/echo",other_string);
    const char* topic1 = topicbuff1;
    //printf("subTopic = %s\n",topic1);
    rc = client.subscribe(topic1, MQTT::QOS0,messageArrived);
    printf("rc subscribe = %d\r\n", rc);
    sense.rise(&sense_function);
    pace.rise(&pace_function);
    sense.enable_irq();
    pace.enable_irq();
    printf("rc IRQ Enabled\r\n");
    t.reset();
    t.start();
    printf("rc Timer Started\r\n");
    //set_time(0);
    while(1){
        //wait(5);
        //time_t t = time(NULL);
        /*Publish*/
    
    //printf("Connecting to %s:%d\r\n", hostname, port);
    
    if(t.read() > p){
            int temp = a[i];
            a[i] = s_count + p_count;
            sum = sum + a[i] - temp;
            printf("sum %d \r\n",sum);
            heart_rate = sum *(60/w);            
            i++;
            s_count = 0;
            p_count = 0;
            if(i == navg){
                i = 0;
                }
            printf("heart rate is %d \r \n",heart_rate);
            if(heart_rate>100){
                highalarm = 1;
                }
            else if(heart_rate<60){
                lowalarm = 1;
                }
            else{
                highalarm = 0;
                lowalarm = 0;
                }
        t.reset();
        MQTT::Message message;
        char buf[100];
        //buf = "Hello World";
        //pay = rand()%10;
        sprintf(buf,"%d:%d:%d",heart_rate,highalarm,lowalarm);
        message.qos = MQTT::QOS0;
        message.retained = false;
        message.dup = false;
        message.payload = (void*)buf;
        message.payloadlen = strlen(buf)+1;
        
        //topicbuff
        sprintf(topicbuff,"cis541/hw-mqtt/26013f37-200800bae2a8ca552b0bca2f5001e87/data",other_string);
        const char* topic = topicbuff;
        printf("pubTopic = %s\n",topic);
        rc = client.publish(topic, message);
        printf("rc publish = %d\r\n", rc);
        printf("Publised Data %s\r\n",buf);
        //t.reset();
        printf("tReset");
        /*while (arrivedcount < 1){
            client.yield(15);
        }*/
        printf("yieldOut");
    }
}
    
    socket.close();
    wifi.disconnect();
    memset(other_string,0,128);
    memset(outputUUID,0,128);
    memset(topicbuff,0,128);
    memset(topicbuff1,0,1024);
    //printf("Arrivecount = %d\n",arrivedcount);
    return 0;
}