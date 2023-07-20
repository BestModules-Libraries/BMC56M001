/*****************************************************************
File:         Star-Node.ino
Description: 
          1. Short press KEY1 (D8), the MRX indicator starts flashing, 
             indicating that pairing mode is entered, and the pairing time is 8 seconds.
          2. After both boards enter the pairing mode, only the node side can short press KEY2 (D9) to send the pairing request packet, 
             the pairing success MRX indicator is off, and the pairing success is displayed on the Serial Monitor, 
             if there is no pairing success within 8 seconds, the MRX indicator will also go off, 
             the user can press and hold KEY1 (D8) again to enter the pairing mode.
          3. After the two boards are successfully paired, enter the receive mode, 
             the user can press KEY2 (D9) to send the data packet, and the MTX indicator will flash once. 
             When the received data is successful, the MRX indicator will flash once, 
             and the received data will be displayed on the Serial Monitor.      
Note:    rxPin:D5 txPin:D4  
******************************************************************/

#include  "BMC56M001.h"

BMC56M001     BMC56(5,4);         //rxPin,txPin,Please comment out this line of code if you don't use SW Serial
//BMC56M001     BMC56(&Serial);     //Please uncomment out this line of code if you use HW Serial on BMduino
//BMC56M001     BMC56(&Serial1);      //Please uncomment out this line of code if you use HW Serial1 on BMduino
//BMC56M001     BMC56(&Serial2);    //Please uncomment out this line of code if you use HW Serial2 on BMduino
//BMC56M001     BMC56(&Serial3);    //Please uncomment out this line of code if you use HW Serial3 on BMduino
//BMC56M001     BMC56(&Serial4);    //Please uncomment out this line of code if you use HW Serial4 on BMduino

#define   KEY1_Pin (8)
#define   KEY2_Pin (9)
uint8_t   Message_ShortAddr;
bool Flag_Pairing,Flag_PairSuccess;
uint8_t TXDATA[16] = {0},RXDATA[32] = {0};
uint8_t DATA,STATUS,len;

uint8_t Sys_KEY(void);
void RFMessage_Process();
void Handle_RFPacket_Process();

void setup()
{
  pinMode(KEY1_Pin, INPUT_PULLUP);  
  pinMode(KEY2_Pin, INPUT_PULLUP); 
  Serial.begin(115200); 
  BMC56.begin(BR_38400);          //set BMduino and BMC56 uart baudrate 
  BMC56.setDeviceRole(Node_of_Star);          //set device role
}

void loop() 
{              
 RFMessage_Process();                        
 Handle_RFPacket_Process();        
}


/******************************************************
Description: Scan KEY         
Input:  none            
Output: none         
Return: KEY Value         
Others: put this funtion to loop,circularly Scan KEY         
*******************************************************/
uint8_t Sys_KEY(void)
{
  if(!digitalRead(KEY1_Pin))
  {
   delay(60);
   if(!digitalRead(KEY1_Pin))
   {
    return 0x01;
   }
  }

  if(!digitalRead(KEY2_Pin))
  {
   delay(60);
   if(!digitalRead(KEY2_Pin))
   {
    return 0x02;
   }
  }
  return 0x00;
}

/******************************************************
Description: Send commands according to keyvalue        
Input:  none            
Output: none         
Return: none         
Others: put this funtion to loop         
*******************************************************/
void RFMessage_Process()
{
  switch(Sys_KEY())
  { 
   case 0x01: 
      BMC56.setMode(Pairing_Mode);                                                 //send pairing mode command
      Flag_Pairing = TRUE; 
      Flag_PairSuccess = FALSE;            
    break;
          
   case 0x02:   
      if(Flag_Pairing)
      {
        BMC56.writePairPackage();                //Send request pairing packets during pair mode                 
      } 
      if(Flag_PairSuccess)
      {
        for(uint8_t temp=0;temp<16;temp++)
         {
          TXDATA[temp] = DATA++;
         }
        BMC56.writeRFData(Message_ShortAddr,16,TXDATA);            //Send  data packet after the pairing is successful  
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
          Message_ShortAddr = BMC56.getShortAddress();
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
       STATUS = BMC56.readRFData(RXDATA,len);  
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
