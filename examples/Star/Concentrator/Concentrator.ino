/*****************************************************************
File:         Star-Concentrator.ino
Description: 
        1. Short press KEY1 (D8), the MRX indicator starts flashing, 
           indicating that pairing mode is entered, and the pairing time is 8 seconds.
        2. After both boards enter the pairing mode, wait for the pairing request packet sent by the receiving node, 
           the pairing success MRX indicator is off, and the pairing success is displayed on the Serial Monitor,
           if there is no pairing success within 8 seconds, the MRX indicator will also go off, 
           the user can press and hold KEY1 (D8) again to enter the pairing mode.
        3. By pairing Concentrator to pass its own ID and assign a different short address to each node,
           it forms a 1-to-many (max:5) star topology network. The first one to come in and pair is node1, 
           and the second one to come in and pair is node2,... The sixth pairing will start from node1 again.
        4. After the two boards are successfully paired, enter the receive mode, 
          the user can short press the KEY2~KEY6 of any board to send a packet to Node1~Node5, 
          and the MTX indicator flashes once. When the received data is successful,
          the MRX indicator will flash once and the received data will be displayed on the serial monitor.       
Note:    rxPin:D5 txPin:D4 
******************************************************************/
 #include  "BMC56M001.h"

BMC56M001     BMC56(5,4);         //rxPin,txPin,Please comment out this line of code if you don't use SW Serial
//BMC56M001     BMC56(&Serial);     //Please uncomment out this line of code if you use HW Serial on BMduino
//BMC56M001     BMC56(&Serial1);      //Please uncomment out this line of code if you use HW Serial1 on BMduino
//BMC56M001     BMC56(&Serial2);    //Please uncomment out this line of code if you use HW Serial2 on BMduino
//BMC56M001     BMC56(&Serial3);    //Please uncomment out this line of code if you use HW Serial3 on BMduino
//BMC56M001     BMC56(&Serial4);    //Please uncomment out this line of code if you use HW Serial4 on BMduino


/*define KEY interface*/
#define   KEY1_Pin (8)
#define   KEY2_Pin (9)
#define   KEY3_Pin (10)
#define   KEY4_Pin (11)
#define   KEY5_Pin (12)
#define   KEY6_Pin (13)

bool Flag_Pairing,Flag_PairSuccess;
uint8_t TXDATA[16] = {0},RXDATA[32] = {0};
uint8_t DATA,STATUS,len;

byte Sys_KEY(void);
void RFMessage_Process();
void Handle_RFPacket_Process();

void setup()
{
  pinMode(KEY1_Pin, INPUT_PULLUP);  
  pinMode(KEY2_Pin, INPUT_PULLUP); 
  pinMode(KEY3_Pin, INPUT_PULLUP);
  pinMode(KEY4_Pin, INPUT_PULLUP);
  pinMode(KEY5_Pin, INPUT_PULLUP);
  pinMode(KEY6_Pin, INPUT_PULLUP);
  Serial.begin(115200);
  BMC56.begin(BR_38400);                   //set BMduino and BMC56 uart baudrate 
  BMC56.setDeviceRole(Concentrator_of_Star);    //set device role
}

void loop() 
{
 RFMessage_Process();                         //Send commands according to keyvalue   
 Handle_RFPacket_Process();                  //returns data after Rf receive data
}

/******************************************************
Description: Scan KEY         
Input:  none            
Output: none         
Return: KEY Value         
Others: put this funtion to loop,circularly Scan KEY         
*******************************************************/
byte Sys_KEY(void)
{
   if(!digitalRead(KEY1_Pin))
  {
   delay(50);
   if(!digitalRead(KEY1_Pin))
   {
    return 0x01;
   }
  }

  if(!digitalRead(KEY2_Pin))
  {
   delay(50);
   if(!digitalRead(KEY2_Pin))
   {
    return 0x02;
   }
  }

  if(!digitalRead(KEY3_Pin))
  {
   delay(50);
   if(!digitalRead(KEY3_Pin))
   {
    return 0x03;
   }
  }

  if(!digitalRead(KEY4_Pin))
  {
   delay(50);
   if(!digitalRead(KEY4_Pin))
   {
    return 0x04;
   }
  }

  if(!digitalRead(KEY5_Pin))
  {
   delay(50);
   if(!digitalRead(KEY5_Pin))
   {
    return 0x05;
   }
  }

  if(!digitalRead(KEY6_Pin))
  {
   delay(50);
   if(!digitalRead(KEY6_Pin))
   {
    return 0x06;
   }
  }
  return 0x00;
}

/******************************************************
Description: control BMC56 RF funtion        
Input:  none            
Output: none         
Return: none         
Others: put this funtion to loop         
*******************************************************/
void RFMessage_Process()
{
  byte x;
  switch(Sys_KEY())
  { 
    case 0x01:  
     if(!BMC56.setMode(Pairing_Mode))                                       //send pairing mode command
     {
        Flag_Pairing = TRUE; 
        Flag_PairSuccess = FALSE;        
     }
     break;          
    case 0x02:   
      if(Flag_PairSuccess)
      {
        for(uint8_t temp=0;temp<16;temp++)
         {
          TXDATA[temp] = DATA++;
         }
       BMC56.writeRFData(Node1_ShortAddr,16,TXDATA);       //use Node1 address to Send data packet after the pairing is successful 
      }
      break;
    case 0x03:
      if(Flag_PairSuccess)
      {
       for(uint8_t temp=0;temp<16;temp++)
         {
          TXDATA[temp] = DATA++;
         }
       BMC56.writeRFData(Node2_ShortAddr,16,TXDATA);        //use Node2 address to Send data packet after the pairing is successful   
      }
      break;
    case 0x04:
      if(Flag_PairSuccess)
      {
        for(uint8_t temp=0;temp<16;temp++)
         {
          TXDATA[temp] = DATA++;
         }
        BMC56.writeRFData(Node3_ShortAddr,16,TXDATA);       //use Node3 address to Send data packet after the pairing is successful    
      }
      break;
    case 0x05:
      if(Flag_PairSuccess)
      {
        for(uint8_t temp=0;temp<16;temp++)
         {
          TXDATA[temp] = DATA++;
         }
        BMC56.writeRFData(Node4_ShortAddr,16,TXDATA);       //use Node4 address to Send data packet after the pairing is successful 
      }
      break;
    case 0x06:
      if(Flag_PairSuccess)
      {
        for(uint8_t temp=0;temp<16;temp++)
         {
          TXDATA[temp] = DATA++;
         }
        BMC56.writeRFData(Node5_ShortAddr,16,TXDATA);      //use Node5 address to Send data packet after the pairing is successful
      }
      break;
  }     
}
/******************************************************
Description: dispose RF Packet         
Input:  none            
Output: none         
Return: none         
Others: put this funtion to loop         
*******************************************************/
void Handle_RFPacket_Process()
{
   if(Flag_Pairing)
      {
       STATUS = BMC56.getPairStatus(); 
       if(STATUS == 1) //pairing success
        {
          Flag_Pairing = FALSE;
          Flag_PairSuccess = TRUE;
          BMC56.setMode(Rx_Mode);
          Serial.println("Pair Success");                                                             
        }  
       if(STATUS == 2)  //pairing fail          
        {
          Flag_Pairing = FALSE;
          Flag_PairSuccess = FALSE;
        }   
        if(STATUS == 3) //pairing timout
        {
          Flag_Pairing = FALSE;
          Flag_PairSuccess = FALSE;                                                           
        }                                              
      }
   
      if(Flag_PairSuccess)
      {
      if(BMC56.isInfoAvailable())  
      {
       STATUS = BMC56.readRFData(RXDATA,&len);  
       if(STATUS == 1)                    //After the match is successful, data is received
        { 
         Serial.print("RXDATA[]:");
         for(uint8_t temp=0;temp<len;temp++)
         {
          Serial.print(RXDATA[temp],HEX);
          Serial.print(" ");
         }
         Serial.println(" ");
        }
      }
     }
}
