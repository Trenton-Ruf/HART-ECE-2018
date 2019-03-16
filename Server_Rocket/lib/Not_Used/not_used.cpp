
/*
void recieve_launch(){

  if (manager.available()){
    // Wait for a message addressed to us from the client
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from)){ // recieve from ground station will wait forever instead consider recvFromAckTimout
      Serial.print("got request from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);
      
      char* launch = "launch"; // maybe use strstr((char *)buf, "launch") instead
      if(strstr((char*)buf, launch)){ // if the recieved message is "launch"
        launched = true;          // then set the launched bool true
        strcpy(data,"launched");
      }
      else{
        Serial.print("Launch command not recieved");
        strcpy(data,"failed");
      }

      // Send a reply back to the originator client
      if (!manager.sendtoWait((uint8_t *)data, strlen(data), from))
      Serial.println("sendtoWait failed");
    }
  }

}
*/









 /*
  while(!launched){

    Serial.println("PRESS LAUNCH BUTTON");
    while(!digitalRead(LAUNCH_BUTTON)){};

    //Serial.println("Press any key to launch");
    //while(Serial.available() == 0){};
    
    if (manager.sendtoWait(data, sizeof(data), CLIENT_ADDRESS)){
      // Now wait for a reply from the server
      uint8_t len = sizeof(buf);
      uint8_t from;   
      if (manager.recvfromAckTimeout(buf, &len, 2000, &from)){
        Serial.print("got reply from : 0x");
        Serial.print(from, HEX);
        Serial.print(": ");
        Serial.println((char*)buf);

        char * test = "launched";
        if(strstr((char*)buf,test)){
          launched = true;
          digitalWrite(LED,HIGH);
        }
        else{
          Serial.print("CRITICAL ERROR: sent launch code recieved garbage");
        }
      }
      else{
        Serial.println("No reply, is rf24_reliable_datagram_server running?");
      }
    }
    else{
      Serial.println("sendtoWait failed");
      delay(500);
    }
  }
  Serial.println("Sent launch signal");
  Serial.println("Retrieving Telemetry Data");
  */
 
 //FOR TESTING
 // data_telemetry.gps.latitude = 111112;
  //data_telemetry.tmp = 600001;