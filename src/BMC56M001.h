/***********************************************************
File:               BMC56M001.h
Author:             BEST MODULES CORP.
Description:        Define classes and required variables
Version:            V1.0.3   --2025-05-17
***********************************************************/
#ifndef _BMC56M001_H_
#define _BMC56M001_H_

#include "Arduino.h"
#include <SoftwareSerial.h>

#define TRUE 1
#define FALSE 0
#define   CHECK_OK        0
#define   CHECK_ERROR     1
#define   TIMEOUT_ERROR   2

#define   Node1_ShortAddr    (0x0001)
#define   Node2_ShortAddr    (0x0002)
#define   Node3_ShortAddr    (0x0003)
#define   Node4_ShortAddr    (0x0004)
#define   Node5_ShortAddr    (0x0005)

#define   BR_9600 0
#define   BR_19200 1
#define   BR_38400 2

#define   SUCCESS           0
#define   FAIL              1

class BMC56M001
{
   public:   
      BMC56M001(HardwareSerial *theSerial = &Serial);
      BMC56M001(uint8_t rxPin,uint8_t txPin);
      void begin(uint8_t baud = BR_9600);                                                  
      bool isPaired();
      uint8_t writePairPackage();
      uint8_t getPairStatus();
      uint8_t writeRFData(uint32_t shortAddr,uint8_t len,uint8_t data[]);
      bool isInfoAvailable();
      uint8_t readRFData(uint8_t rxData[],uint8_t &len);    
      uint8_t getShortAddress(); 
      uint8_t getRSSI();                          
      uint8_t getPktRSSI();
      uint8_t writeEEPROM(uint8_t len,uint8_t deviInfo[]);   
      uint8_t readEEPROM(uint8_t deviInfo[],uint8_t &len);
      uint8_t getFWVer(uint8_t number[]); 
      uint8_t getSN(uint8_t id[]);   
                                  
      uint8_t getDeviceRole();                       
      uint8_t getMode();                             
      uint8_t getChannelPtn();                       
      uint8_t getRFPower();                          
      uint8_t getDataRate();                         
      uint8_t getHoppPeriod(uint8_t period[]);                                             
      uint8_t getBaud(); 
      uint8_t setDeviceRole(uint8_t role);               
      uint8_t setMode(uint8_t mode);                     
      uint8_t setChannelPtn(uint8_t channel);              
      uint8_t setRFPower(uint8_t power);                     
      uint8_t setDataRate(uint8_t rate);                  
      uint8_t setHoppPeriod(uint8_t period[]); 
                                                           
   private:                                          
      uint8_t setBaudRate(uint8_t baudRate); 
      uint8_t setRFAddress(uint8_t address[]);
      uint8_t getRFAddress(uint8_t address[]); 
      uint8_t checksum(uint8_t len,uint8_t data[]);    
      void writeBytes(uint8_t wbuf[],uint8_t wlen);
      uint8_t readBytes(uint8_t rbuf[],uint8_t *rlen,uint16_t timeOut = 250); 
      uint16_t _rxPin; 
      uint16_t _txPin;
      uint8_t  shrotAddress;
      HardwareSerial *_hardSerial = NULL;
      SoftwareSerial *_softSerial = NULL ;          
};



/*define Pair or Unpair Op code*/
enum
{
  Pair_Op_Code = 0x4000,
  Unpair_Op_Code = 0x8000,
};

/*device role Optional parametersl*/
enum Device_Role_TYPE
{
  Peer  = 0x00,            
  Node_of_Star,                       
  Concentrator_of_Star ,                       
};

/*device channel*/
enum Device_Channel_TYPE
{
  ChannelG_1 = 0,
  ChannelG_2,
  ChannelG_3,
  ChannelG_4,
  ChannelG_5,
  ChannelG_6,
  ChannelG_7,
  ChannelG_8, 
  ChannelG_9,
  ChannelG_10,
  ChannelG_11,
  ChannelG_12,
  ChannelG_13,
  ChannelG_14,
  ChannelG_15,
  ChannelG_16,                    
};

/*device mode Optional parameters*/
enum Device_Mode_TYPE
{
  DeepSleep_Mode    = 0x00,            
  Sleep_Mode ,                  
  Rx_Mode ,                         
  Pairing_Mode ,                                         
};

/*device TX Power Optional parameters*/
enum Device_TXPower_TYPE
{
  N3dBm = 0,            
  P0dBm ,                  
  P5dBm ,                         
  P7dBm ,                                         
}; 

/*device RF data rate Optional parameters*/
enum Device_RFDataRate_TYPE
{
  DR500Kbps = 0,            
  DR250Kbps ,                  
  DR125Kbps ,                                                                 
}; 
#endif 
