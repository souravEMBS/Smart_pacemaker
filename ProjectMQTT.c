void mqttinit(){
    MQTTNetwork mqttNetwork(&wifi);
    MQTT::Client<MQTTNetwork, Countdown> client = MQTT::Client<MQTTNetwork, Countdown>(mqttNetwork) ;
    const char* hostname = "35.188.242.1";
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
    }
	
	void generateUUID(){
        /*UUID*/
    iap_entry=(IAP) IAP_LOCATION;
    command[0] = 58;
    //output[0] = 58;
    iap_entry(command, output);
    for(oi = 0;oi<5;oi++){
        printf("UUID output = %02x\n",output[oi]);
        }
    n = sprintf(outputUUID,"26013f37-%s%s%s%s",output[1],output[2],output[3],output[4]);
    strcat(other_string, outputUUID);
    printf("UUID = %s\n",other_string);
    printf("%s\n",other_string);
    }
	
	while(1){
        if(t.read() > p)
        {
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
        sprintf(buf,"%d:%d:%d",heart_rate,highalarm,lowalarm);
        message.qos = MQTT::QOS0;
        message.retained = false;
        message.dup = false;
        message.payload = (void*)buf;
        message.payloadlen = strlen(buf)+1;
        sprintf(topicbuff,"cis541/hw-mqtt/26013f37-200800bae2a8ca552b0bca2f5001e87/data",other_string);
        const char* topic = topicbuff;
        printf("pubTopic = %s\n",topic);
        rc = client.publish(topic, message);
        printf("rc publish = %d\r\n", rc);
        }
    }