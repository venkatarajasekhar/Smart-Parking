#include <RS485.h>
#include <SoftwareSerial.h>

#include "globals.h"
#include "WiredCommunication.h"

SoftwareSerial xbee(5, 6); // RX, TX
WiredCommunication wiredbus;

#define MAX_SENSORS 32
String nodeId = "test1";

int  numSensors = 99; // Starts off at 99 so that the base recognizes first message as startup message. SET TO 3 FOR TESTING
int retryCount = 0;//retry count for adding messages to queue
int check = 0;//checks for success of adding to queue
String messageFromBase = "";


char sensorStatus[MAX_SENSORS]; // keeps track of status of each sensor
bool dataChanged = false; // Set to true if any sensor's status has changed. XBee only sends if this is true.

/*
    Setup block
    Initializes communication protocols and initiates other setup related tasks.
*/
void setup()
{
    DEBUG_INIT(9600);
    DEBUG_PRINTLN("New System Startup - Sender");

    RS485_Begin(4800);
    xbee.begin(9600);
    
    // Wait for sensors to power up and calibrate
    delay(2000);
	
    String powerOnMessage = nodeId + ',' + String(numSensors) + ',' + String(0);//numSensors should be 99 at startup so the base knows its asking for number of sensors
    DEBUG_PRINTLN(numSensors); delay(2000);
    /*
    while(numSensors == 99 || numSensors == 0){
        numSensors = sendMessageBase(powerOnMessage);
    }
    */
    numSensors = 3;
}

void loop()
{
    /*
        Loops through all the sensors asking for a status.
        Records the responses or time outs in sensorStatus[]
    */
    for (int sensorNum = 1; sensorNum <= numSensors; sensorNum++) {
        
        DEBUG_PRINT("Querying Sensor ");DEBUG_PRINTLN(sensorNum);
        
        //The message is composed of a sensor ID and command.
        char sendBuff[maxMsgLen+1];
        sendBuff[0] = getSensorIdFromIndex(sensorNum);//get sensorIdFromIndex
        sendBuff[1] = 'C';
        sendBuff[2] = '\0';
        
        if(wiredbus.sendMessage(sendBuff))
        {
            DEBUG_PRINTLN(sendBuff);
            
            // Listen for response
            char recBuff[maxMsgLen+3+1];
            timeout_init(2000);
            bool gotResponse = false;
            while (!timeout_timedout()) {
                if (wiredbus.getMessage(recBuff)){
                    DEBUG_PRINT("Got:");
                    DEBUG_PRINTLN(recBuff);
                    
                    /*
                        If the response is from the same sensor we responded data from, parse it.
                    */
                    if (recBuff[0] == getSensorIdFromIndex(sensorNum)) {
                        switch (recBuff[1]) {
                            case 'T': // Car present
                                DEBUG_PRINTLN("CAR PRESENT");
                                if (sensorStatus[sensorNum-1] != 'T'){
                                    sensorStatus[sensorNum-1] = 'T';
                                    dataChanged = true;
                                }
                                break;
                            case 'A': // Car NOT present
                                DEBUG_PRINTLN("CAR NOT PRESENT");
                                if (sensorStatus[sensorNum-1] != 'A'){
                                    sensorStatus[sensorNum-1] = 'A';
                                    dataChanged = true;
                                }
                                break;
                            default:
                                DEBUG_PRINTLN("Invalid Response");
                                sensorStatus[sensorNum-1] = 'I';
                        }
                    } else {
                        // Wrong sensor ID
                        DEBUG_PRINTLN("Wrong sensor responded to request...");
                        sensorStatus[sensorNum-1] = 'W';
                    }
                    
                    gotResponse = true;
                    break;
                }
                delay(100);
            }
            
            if (!gotResponse) {
                DEBUG_PRINTLN("Timed out!");
                sensorStatus[sensorNum-1] = 'L';
            }
        }
		else {
            DEBUG_PRINTLN("Send Failed");
        }
        DEBUG_PRINTLN();
        delay(100);
    }
    
    
    /*
        Print stuff out to serial
    */
    DEBUG_PRINTLN("==============================");
    DEBUG_PRINT("Sensor: ");
    for (int i = 1; i <= numSensors; i++) {
        DEBUG_PRINT(i);
        DEBUG_PRINT(" ");
    }
    DEBUG_PRINT("\nStatus: ");
    for (int i = 1; i <= numSensors; i++) {
        DEBUG_PRINT(sensorStatus[i-1]);
        DEBUG_PRINT((i < 10) ? " " : "  ");
    }
    DEBUG_PRINTLN();
    
    DEBUG_PRINTLN(dataChanged ? "Data changed, sending..." : "Not sending. no change");
    
    if (dataChanged) {
        /*
            Send data to base station using XBee
        */
        // Get count of available spots
        uint8_t countAvailable = 0;
        for (int i = 1; i <= numSensors; i++) {
            if (sensorStatus[i-1] == 'A') {
                countAvailable++;
            }
        }
        
        /*
            Prepare XBee message packet:
            [Node ID],[Total Spots],[Available Spots]
        */
        String updateMessage = nodeId + ',' + String(numSensors) + ',' + String(countAvailable);
        /*
		xbee.listen();
        DEBUG_PRINTLN(updateMessage);
        xbee.println(updateMessage);
		*/
		
		check = sendMessageBase(updateMessage);
		if(check < 0){
			DEBUG_PRINTLN("sendMessageBase() failed");
		}
		
    }
    
    DEBUG_PRINTLN("==============================\n");
    
    dataChanged = false;
}

int sendMessageBase(String message){
	//sends message to base
	//wait for response from base!!!!!!!!!!!!!!!!
	xbee.listen();
	
	//To check
	int responseCount = 0;
	String baseResponse;
	while(responseCount < 5){
		xbee.println(message);
		baseResponse = getBaseMessage();
		if(baseResponse != NULL){
			break;
		}
		delay(10);
		responseCount++;
	}
	
	uint8_t total = -1;
	
	if(responseCount >= 5){
		DEBUG_PRINTLN(F("getBaseMessage() timed out!"));
		//total = -1;
	}
	else{
		if(baseResponse != NULL){
			uint8_t start = 0;
			uint8_t end = baseResponse.indexOf(',');
			String target = baseResponse.substring(start, end);
			
			start = end + 1;
			end = baseResponse.indexOf(',', start);
			total = baseResponse.substring(start, end).toInt();
			
			DEBUG_PRINTLN("Target Node:  " + String(target));
			DEBUG_PRINTLN("Number: " + String(total));
		}
		else{
			DEBUG_PRINTLN("Base Response = NULL");
		}
	}
    //DEBUG_PRINT("This is what sendMessageBase returns: ");
    //DEBUG_PRINTLN(total);
    //delay(2000);
    return total;
}


String getBaseMessage() {

  xbee.listen();
  String xResponse = xbeeResponse();
  uint8_t start = 0;
  uint8_t end = xResponse.indexOf(',');
  String targetNode = xResponse.substring(start, end);
  if(targetNode.equals(nodeId)){
	return xResponse;
  }
  else{
	return NULL;
  }
}

char getSensorIdFromIndex(uint8_t index) {
    // Returns a byte from 0x40 of the ascii table. Starting at @ A B C ...
    // Have to do this because if we start from 0 1 2... the control characters at the beginning mess up transmissions
    // Since sensor 0 is master, it will be @, the slaves will start from A
    return (char)(index + 64);
}

String xbeeResponse() {
    String content = "";
    int counter = 0;
    while(!xbee.available() && counter < 250) {
        counter++;
        delay(10);
    }
    
    if (counter >= 2500) {
        DEBUG_PRINTLN(F("XBEE Timed Out"));
        return NULL;
    }
    
    while (xbee.available()) {
        char input = xbee.read();
        //DEBUG_PRINTLN("Xbee response: " + input);
        content.concat(input);
        if (input == '\n') {
            break;
        }
        delay(1);
    }
    
    xbee.flush();
    return content;
}

// Timeout Functions
// ========================================
unsigned long _timeout_end = 0;

void timeout_init(int timeoutms) {
    _timeout_end = millis() + timeoutms;
}
unsigned long timeout_remaining() {
    return _timeout_end-millis();
}
bool timeout_timedout() {
    return millis() > _timeout_end;
}