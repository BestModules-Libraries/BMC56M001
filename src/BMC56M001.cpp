
/***********************************************************
File:           BMC56M001.cpp
Author:         XIAO, BESTMODULES
Description:    UART communication with the BMC56M001   
History:      
V1.0.0   -- initial version；2023-07-18；Arduino IDE : v1.8.16
***********************************************************/
#include  "BMC56M001.h"

/**********************************************************
Description: Constructor
Parameters:  *theSerial: Wire object if your board has more than one UART interface      
                         parameter range:&Serial、&Serial1、&Serial2、&Serial3、&Serial4
Return:          
Others:     
**********************************************************/
BMC56M001::BMC56M001(HardwareSerial *theSerial)
{
     _softSerial = NULL;
     _hardSerial = theSerial;
}

/**********************************************************
Description: Constructor
Parameters:  rxPin : Receiver pin of the UART
             txPin : Send signal pin of UART         
Return:          
Others:   
**********************************************************/
BMC56M001::BMC56M001(uint8_t rxPin,uint8_t txPin)
{
    _hardSerial = NULL;
    _rxPin = rxPin;
    _txPin = txPin;
    _softSerial = new SoftwareSerial(_rxPin,_txPin);   
}

/**********************************************************
Description: Initialize in uart mode
Parameters:  baud: 
                      BR_9600:9600bps
                      BR_19200:19200bps
                      BR_38400:38400bps     
Return:          
Others:      If the _softSerial pointer is empty, the software serial port is initialized,
             otherwise the hardware serial port is initialized.      
**********************************************************/
void BMC56M001::begin(uint8_t baud)
{
    bool x=0;
    uint32_t BR_Parameter[3] = {9600,19200,38400};
    uint8_t temp=0;
    float delaytime;
    delaytime = 107;

   if(_softSerial != NULL)
    {
      do
      {
        _softSerial->begin(BR_Parameter[temp]);
        if(!setBaudRate(baud))
        {
          
          x = 1;
          _softSerial->begin(BR_Parameter[baud]); 
        }
        else temp++;
      }while(!x);
      _softSerial->setTimeout((uint8_t)delaytime); 
    }
    else
    {
      do
      {
        _hardSerial->begin(BR_Parameter[temp]);
        if(!setBaudRate(baud))
        {
          x = 1;
          _hardSerial->begin(BR_Parameter[baud]); 
        }
        else temp++;
      }while(!x);
      _hardSerial->setTimeout((uint8_t)delaytime);      
    }
}

/**********************************************************
Description: Gets the pair status
Parameters:  
Return:     TRUE: paired 
            FALSE: no paired       
Others:        
**********************************************************/
bool BMC56M001::isPaired()
{
   uint8_t sendBuf[4] = {0x01, 0x99, 0x00}; 
   uint8_t buff[6] = {0};
   uint8_t leng = 0;
   sendBuf[3] = checksum(3,sendBuf);
   writeBytes(sendBuf,4);
   if(readBytes(buff,&leng) == CHECK_OK)
   {
    if(buff[4]==1)
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
   }
   else
     return FALSE;
}

/**********************************************************
Description: Send the data as an Pair Package
Parameters:  
Return:      0: The command is executed successfully. 1: Command execution fails.
             2: The command is not supported. 3: Format error; 4: The data is too long.
Others:                
**********************************************************/
uint8_t BMC56M001::writePairPackage()
{
   uint8_t sendBuf[40] = {0};
   sendBuf[0] = 0x10;
   sendBuf[1] = 0x00;
   sendBuf[2] = 4;
   sendBuf[3] = 0X40;
   sendBuf[4] = 0X00;
   sendBuf[5] = 0x55;
   sendBuf[6] = 0xAA;
   sendBuf[7] = checksum(7,sendBuf);
   uint8_t buff[5] = {0};
   uint8_t leng = 0;
   writeBytes(sendBuf,8);
   if(readBytes(buff,&leng) == CHECK_OK)
   {
    return buff[3];
   }
   else 
   {
      return 1; 
   }
}

/**********************************************************
Description: get Pair Status
Parameters:     
Return:      0: Pairing 
             1: Pair Success
             2: pair fail   
             3: pair timout
Others:          
**********************************************************/
uint8_t BMC56M001::getPairStatus()
{
  uint8_t leng = 0;
  uint8_t dataBuff[50] = {0};
  uint8_t flag = 0;
  
  if(_softSerial != NULL)
  {
    if(_softSerial->available() >= 3)
    {
      if(readBytes(dataBuff,&leng) == CHECK_OK)
      {
        flag = 1;
      }
    }
    else 
    {
      flag = 0;
    }
 }
 else
 {
    if(_hardSerial->available() >= 3)
    {
      if(readBytes(dataBuff,&leng) == CHECK_OK)
      {
        flag = 1;
      }
    }
    else 
    {
      flag = 0;
    }
 }
    
 if(flag == 1) 
 {
  if(dataBuff[0] == 0x40)
  {
   if((dataBuff[3] == (Pair_Op_Code>>8)) || (dataBuff[3] == (Unpair_Op_Code>>8)))
   {
      return 1;
   }

   else if(dataBuff[3] == 0x05)
   {
      return 2;
   }

   else if(dataBuff[3] == 0x06)
   {
      return 3;
   }

   else
      return FALSE;
  }
}
return FALSE;
}

/**********************************************************
Description: Send the data as an RF packet
Parameters:  shortAddr: short address
             len: data length(=<32)
             data[]: The starting store to send data
Return:      0: The command is executed successfully. 1: Command execution fails.
             2: The command is not supported. 3: Format error; 4: The data is too long.
             5: The pairing fails. 6: pairing timeout; 7: failed to send. 8: The message is sent successfully. 
Others:          
**********************************************************/
uint8_t BMC56M001::writeRFData(uint32_t shortAddr,uint8_t len,uint8_t data[])
{
   uint8_t sendBuf[40] = {0};
   sendBuf[0] = 0x10;
   sendBuf[1] = 0x00;
   sendBuf[2] = (len+2);
   sendBuf[3] = (uint8_t)(shortAddr>>8);
   sendBuf[4] = (uint8_t)shortAddr;
   for(uint8_t i=0;i<len;i++)
   {
    sendBuf[i+5] = data[i];
   }
   sendBuf[len+5] = checksum(len+5,sendBuf);
   uint8_t buff[5] = {0};
   uint8_t leng = 0;
   writeBytes(sendBuf,(len+6));
   if(readBytes(buff,&leng) == CHECK_OK)
   {
     return buff[3];
   }
   else 
   {
      return 1; 
   }
}

/**********************************************************
Description: Query whether the packets sent by the module are received
Parameters:    
Return:      FALSE: No data received
             TRUE: Data received  
Others:       
**********************************************************/
bool BMC56M001::isInfoAvailable()
{
  if(_softSerial != NULL)
  {
    if(_softSerial->available() >= 3)
    {
      return  TRUE;
    }
    else 
    {
      return   FALSE;
    }
  }
  else
  {
    if(_hardSerial->available() >= 3)
    {
      return   TRUE;
    }
    else 
    {
      return  FALSE;
    }
  }
}

/**********************************************************
Description: Read the data inside the RF packet
Parameters:  rxData[]: Used to store packets
             &len: Used to store packets
Return:      0: No packets received
             1: received data
             2: send data success
             3: send data fail    
Others:           
**********************************************************/
uint8_t BMC56M001::readRFData(uint8_t rxData[],uint8_t &len)
{
  uint8_t leng = 0;
  uint8_t dataBuff[50] = {0};
  uint8_t flag = 0;
  
  if(readBytes(&dataBuff[0],&leng) == CHECK_OK)
  {
    flag = 1;
  }
  else 
  {
    flag = 0;
    return FALSE;
  }
    
  if(flag == 1)
  {
   if(dataBuff[0] == 0x40)
   {
    len = (leng-4);
    for(int i=0;i<(leng-4);i++)
    {
     rxData[i]= dataBuff[i+4];
    } 

    if(dataBuff[2] > 1)
    {
      if((dataBuff[3] == shrotAddress) || ((dataBuff[3] >= Node1_ShortAddr) && (dataBuff[3] <= Node5_ShortAddr)))
        return 1;
      else
        return FALSE;
    }

    else if(dataBuff[2] == 1)
    {
      if(dataBuff[3] == 0x08)
        return 2;
      else if(dataBuff[3] == 0x07)
        return 3;
      else
        return FALSE;
    }

    else
      return FALSE;
  }
  else
    return FALSE;
  } 
}

/**********************************************************
Description: Obtain the RF communication short address
Parameters:  
Return:      shrotAddress: short address 
Others:         
**********************************************************/
uint8_t BMC56M001::getShortAddress()
{
   shrotAddress = 0;
   uint8_t sendBuf[4] = {0x01, 0x85, 0x00};      
   uint8_t buff[6] = {0};
   uint8_t leng = 0;
   sendBuf[3] = checksum(3,sendBuf);
   writeBytes(sendBuf,4);
   if(readBytes(buff,&leng) == CHECK_OK)
   {
    shrotAddress = buff[4];
   } 
   return shrotAddress;
}

/**********************************************************
Description: Gets the current signal strength
Parameters:  
Return:   
        rssi: Current signal strength
Others:         
**********************************************************/
uint8_t BMC56M001::getRSSI()
{
   uint8_t sendBuf[4] = {0x01, 0x96, 0x00};     
   uint8_t buff[6] = {0};
   uint8_t leng = 0;
   sendBuf[3] = checksum(3,sendBuf);
   writeBytes(sendBuf,4);
   if(readBytes(buff,&leng) == CHECK_OK)
   {
    return buff[4];
   }     
}

/**********************************************************
Description: Gets the signal strength at the time the packet is received
Parameters:  
Return:   
        pktRSSI: The signal strength when the packet is received
Others:        
**********************************************************/
uint8_t BMC56M001::getPktRSSI()
{
   uint8_t sendBuf[4] = {0x01, 0x97, 0x00};
   uint8_t buff[6] = {0};
   uint8_t leng = 0;
   sendBuf[3] = checksum(3,sendBuf);
   writeBytes(sendBuf,4);
   if(readBytes(buff,&leng) == CHECK_OK)
   {
    return buff[4]; 
   }
   return 0; 
}


/**********************************************************
Description: Written information
Parameters:  len: length of the deviInfo[] 
             deviInfo[]:Information that needs to be written(=<32bytes) 
Return:      0: The command is executed successfully. 1: Command execution fails.
             2: The command is not supported. 3: Format error; 4: The data is too long.
             5: The pairing fails. 6: pairing timeout; 7: failed to send. 8: The message is sent successfully.    
Others:         
**********************************************************/
uint8_t BMC56M001::writeEEPROM(uint8_t len,uint8_t deviInfo[])
{
   uint8_t sendBuf[40] = {0};
   sendBuf[0] = 0x01;
   sendBuf[1] = 0x03;
   sendBuf[2] = len;
   for(uint8_t i=0;i<len;i++)
   {
    sendBuf[i+3] = deviInfo[i];
   }   
   uint8_t buff[5] = {0};
   uint8_t leng = 0;
   sendBuf[len+3] = checksum(len+3,sendBuf);
   writeBytes(sendBuf,(len+4));
   if(readBytes(buff,&leng) == CHECK_OK)
     return buff[3];
   else 
     return 1; 
}

/**********************************************************
Description: Get information about the module
Parameters:  deviInfo[]: Used to store the obtained information (=<32bytes)
             &len: Valid information data length  
Return:      0: The command is executed successfully. 1: Command execution fails.
             2: The command is not supported. 3: Format error; 4: The data is too long.
             5: The pairing fails. 6: pairing timeout; 7: failed to send. 8: The message is sent successfully.    
Others:                
**********************************************************/
uint8_t BMC56M001::readEEPROM(uint8_t deviInfo[],uint8_t &len)
{
  uint8_t sendBuf[4] = {0x01, 0x83, 0x00};  
  uint8_t buff[40] = {0};
  uint8_t leng = 0;
  sendBuf[3] = checksum(3,sendBuf);
  writeBytes(sendBuf,4);
  if(readBytes(buff,&leng) == CHECK_OK)
  {
    len = (buff[2]-1);
    for(uint8_t i=0;i<(buff[2]-1);i++)
    {
      deviInfo[i] = buff[i+4];
    }
      return buff[3];
  }
  else
    return 1;
}

/**********************************************************
Description: Gets the version number
Parameters:  number[]: Used to store the obtained version number 
Return:      0: The command is executed successfully. 1: Command execution fails.
             2: The command is not supported. 3: Format error; 4: The data is too long.
             5: The pairing fails. 6: pairing timeout; 7: failed to send. 8: The message is sent successfully.    
Others:         
**********************************************************/
uint8_t BMC56M001::getFWVer(uint8_t number[])
{
   uint8_t sendBuf[4] = {0x01, 0x80, 0x00};
   uint8_t buff[36] = {0};
   uint8_t leng = 0;
   sendBuf[3] = checksum(3,sendBuf);
   writeBytes(sendBuf,4);
   if(readBytes(buff,&leng) == CHECK_OK)
   {
      for(uint8_t i=0;i<(buff[2]-1);i++)
      {
        number[i] = buff[i+4];
      }
      return buff[3];
   }
   else
    return 1;
}

/**********************************************************
Description: Get the ID number
Parameters:  id[]: Used to store the obtained ID number (4bytes)
Return:      0: The command is executed successfully. 1: Command execution fails.
             2: The command is not supported. 3: Format error; 4: The data is too long.
             5: The pairing fails. 6: pairing timeout; 7: failed to send. 8: The message is sent successfully.    
Others:         
**********************************************************/
uint8_t BMC56M001::getSN(uint8_t id[])
{
   uint8_t sendBuf[4] = {0x01, 0x82, 0x00};      
   uint8_t buff[9] = {0};
   uint8_t leng;
   sendBuf[3] = checksum(3,sendBuf);
   writeBytes(sendBuf,4);
   if(readBytes(buff,&leng) == CHECK_OK)
   {
    id[0] = buff[7];
    id[1] = buff[6];
    id[2] = buff[5];
    id[3] = buff[4];
    return buff[3];
   }
   else 
      return 1;  
}

/**********************************************************
Description: Gets the device role
Parameters:  
Return:      0:Peer Mode
             1:Star Mode(Node)
             2:Star Mode(Concentrator)
Others:         
**********************************************************/
uint8_t BMC56M001::getDeviceRole()
{
   uint8_t sendBuf[4] = {0x01, 0x90, 0x00};  
   uint8_t buff[6] = {0};
   uint8_t leng;
   sendBuf[3] = checksum(3,sendBuf);
   writeBytes(sendBuf,4);
   if(readBytes(buff,&leng) == CHECK_OK)
   {
    return buff[4];
   }
   return 0;   
}

/**********************************************************
Description: Gets the module operating mode
Parameters:  
Return:      0:deep sleep mode
             1:light sleep mode
             2:receive mode
             3:pairing mode
Others:      
**********************************************************/
uint8_t BMC56M001::getMode()
{
   uint8_t sendBuf[4] = {0x01, 0x91, 0x00};   
   uint8_t buff[6] = {0};
   uint8_t leng;
   sendBuf[3] = checksum(3,sendBuf);
   writeBytes(sendBuf,4);
   if(readBytes(buff,&leng) == CHECK_OK)
   {
    return buff[4];
   } 
   return 0;   
}

/**********************************************************
Description: Gets the module hopping channel
Parameters:  
Return:      0:Frequency hopping group 1     1:Frequency hopping group 2
             2:Frequency hopping group 3     3:Frequency hopping group 4
             4:Frequency hopping group 5     5:Frequency hopping group 6
             6:Frequency hopping group 7     7:Frequency hopping group 8
             8:Frequency hopping group 9     9:Frequency hopping group 10
             10:Frequency hopping group 11   11:Frequency hopping group 12
             12:Frequency hopping group 13   13:Frequency hopping group 14
             14:Frequency hopping group 15   15:Frequency hopping group 16
Others:         
**********************************************************/
uint8_t BMC56M001::getChannelPtn()
{
   uint8_t sendBuf[4] = {0x01, 0x92, 0x00}; 
   uint8_t buff[6] = {0};
   uint8_t leng;
   sendBuf[3] = checksum(3,sendBuf);
   writeBytes(sendBuf,4);
   if(readBytes(buff,&leng) == CHECK_OK)
   {
    return buff[4]; 
   }
   return 0;  
}

/**********************************************************
Description: Gets  RF transmit power
Parameters:  
Return:   
        0:-3dBm
        1:0dBm
        2:5dBm
        3:7dBm
Others:        
**********************************************************/
uint8_t BMC56M001::getRFPower()
{
   uint8_t sendBuf[4] = {0x01, 0x93, 0x00};
   uint8_t buff[6] = {0};
   uint8_t leng;
   sendBuf[3] = checksum(3,sendBuf);
   writeBytes(sendBuf,4);
   if(readBytes(buff,&leng) == CHECK_OK)
   {
    return buff[4]; 
   }
   return 0;  
}

/**********************************************************
Description: Gets the RF communication rate
Parameters:  
Return:   
        0:500Kbps
        1:250Kbps
        2:125Kbps
Others:        
**********************************************************/
uint8_t BMC56M001::getDataRate()
{
   uint8_t sendBuf[4] = {0x01, 0x94, 0x00};    
   uint8_t buff[6] = {0};
   uint8_t leng;
   sendBuf[3] = checksum(3,sendBuf);
   writeBytes(sendBuf,4);
   if(readBytes(buff,&leng) == CHECK_OK)
   {
    return buff[4]; 
   }
   return 0; 
}

/**********************************************************
Description: Gets the module frequency hopping period
Parameters:  period[]: Used to store the hopping period acquired (2bytes)
Return:      0: The command is executed successfully. 1: Command execution fails.
             2: The command is not supported. 3: Format error; 4: The data is too long.
             5: The pairing fails. 6: pairing timeout; 7: failed to send. 8: The message is sent successfully.    
Others:        
**********************************************************/
uint8_t BMC56M001::getHoppPeriod(uint8_t period[])
{
   uint8_t sendBuf[4] = {0x01, 0x95, 0x00};   
   uint8_t buff[7] = {0};
   uint8_t leng;
   sendBuf[3] = checksum(3,sendBuf);
   writeBytes(sendBuf,4);
   if(readBytes(buff,&leng) == CHECK_OK)
   {
      period[0] = buff[4];
      period[1] = buff[5];
      return buff[3];
   }
   else 
      return 1;
}

/**********************************************************
Description: Gets the UART communication baud rate
Parameters: 
Return:      0: 9600bps
             1: 19200bps
             2: 38400bps  
Others:       
**********************************************************/
uint8_t BMC56M001::getBaud()
{
   uint8_t sendBuf[4] = {0x01, 0x81, 0x00};
   uint8_t buff[6] = {0};
   uint8_t leng;
   sendBuf[3] = checksum(3,sendBuf);
   writeBytes(sendBuf,4);
   if(readBytes(buff,&leng) == CHECK_OK)
   {
    return buff[4]; 
   }
   return 0; 
}

/**********************************************************
Description: Set device roles
Parameters:  role:
                  Peer: Peer Mode  
                  Node_of_Star: Star Mode(Node)
                  Concentrator_of_Star: Star Mode(Concentrator)
Return:      0: The command is executed successfully. 1: Command execution fails.
             2: The command is not supported. 3: Format error; 4: The data is too long.
             5: The pairing fails. 6: pairing timeout; 7: failed to send. 8: The message is sent successfully.     
Others:        
**********************************************************/
uint8_t BMC56M001::setDeviceRole(uint8_t role)
{
   uint8_t sendBuf[5] = {0x01, 0x10, 0x01, 0x00};
   sendBuf[3] = role;     
   uint8_t buff[5] = {0};
   uint8_t leng;
   sendBuf[4] = checksum(4,sendBuf);
   writeBytes(sendBuf,5);
   if(readBytes(buff,&leng) == CHECK_OK)
     return buff[3];
   else 
     return 1; 
}

/**********************************************************
Description: Sets the current operating mode
Parameters:  mode:
                   DeepSleep_Mode: Set to deep sleep mode
                   LightSleep_Mode: Set to light sleep mode
                   Rx_Mode: Set to receive mode
                   Pairing_Mode: Set to pairing mode
Return:      0: The command is executed successfully. 1: Command execution fails.
             2: The command is not supported. 3: Format error; 4: The data is too long.
             5: The pairing fails. 6: pairing timeout; 7: failed to send. 8: The message is sent successfully.     
Others:         
**********************************************************/
uint8_t BMC56M001::setMode(uint8_t mode)
{
   uint8_t sendBuf[5] = {0x01, 0x11, 0x01, 0x00};
   sendBuf[3] = mode; 
   uint8_t buff[5] = {0};
   uint8_t leng;
   sendBuf[4] = checksum(4,sendBuf);
   writeBytes(sendBuf,5);
   if(readBytes(buff,&leng) == CHECK_OK)
     return buff[3];
   else 
     return 1;
}

/**********************************************************
Description: Set the module frequency hopping channel
Parameters:  channel:
                     ChannelG_X(Set to Skip Group X,X=1、2、3、4、5、6、7、8、9、10、11、12、13、14、15、16)              
Return:      0: The command is executed successfully. 1: Command execution fails.
             2: The command is not supported. 3: Format error; 4: The data is too long.
             5: The pairing fails. 6: pairing timeout; 7: failed to send. 8: The message is sent successfully.     
Others:        
**********************************************************/
uint8_t BMC56M001::setChannelPtn(uint8_t channel)
{
   uint8_t sendBuf[5] = {0x01, 0x12, 0x01, 0x00};
   sendBuf[3] = channel;   
   uint8_t buff[5] = {0};
   uint8_t leng;
   sendBuf[4] = checksum(4,sendBuf);
   writeBytes(sendBuf,5);
   if(readBytes(buff,&leng) == CHECK_OK)
     return buff[3];
   else 
     return 1;
}

/**********************************************************
Description: Set the RF transmit power
Parameters:  power:
                    N3dBm:-3dBm 
                    P0dBm:0dBm 
                    P5dBm:5dBm
                    P7dBm:7dBm         
Return:      0: The command is executed successfully. 1: Command execution fails.
             2: The command is not supported. 3: Format error; 4: The data is too long.
             5: The pairing fails. 6: pairing timeout; 7: failed to send. 8: The message is sent successfully.     
Others:         
**********************************************************/
uint8_t BMC56M001::setRFPower(uint8_t power)
{
   uint8_t sendBuf[5] = {0x01, 0x13, 0x01, 0x00};
   sendBuf[3] = power;
   uint8_t buff[5] = {0};
   uint8_t leng;
   sendBuf[4] = checksum(4,sendBuf);
   writeBytes(sendBuf,5);
   if(readBytes(buff,&leng) == CHECK_OK)
     return buff[3];
   else 
     return 1;
}

/**********************************************************
Description: Set the RF communication rate
Parameters:  rate:
                  DR125Kbps: 125Kbps 
                  DR250Kbps: 250Kbps
                  DR500Kbps: 500Kbps      
Return:      0: The command is executed successfully. 1: Command execution fails.
             2: The command is not supported. 3: Format error; 4: The data is too long.
             5: The pairing fails. 6: pairing timeout; 7: failed to send. 8: The message is sent successfully.    
Others:         
**********************************************************/
uint8_t BMC56M001::setDataRate(uint8_t rate)
{
   uint8_t sendBuf[5] = {0x01, 0x14, 0x01, 0x00};
   sendBuf[3] = rate;
   uint8_t buff[5] = {0};
   uint8_t leng;
   sendBuf[4] = checksum(4,sendBuf);
   writeBytes(sendBuf,5);
   if(readBytes(buff,&leng) == CHECK_OK)
     return buff[3];
   else 
     return 1;
}

/**********************************************************
Description: Set the RF frequency hopping period
Parameters:  period[]: The hopping cycle parameter needs to be written (2bytes) 
                       Parameter range:({0x00,0x02}~{0xff,0xfe}->2~65534(Converted parameters))
                       Actual hopping time = 8us*Converted parameters
Return:      0: The command is executed successfully. 1: Command execution fails.
             2: The command is not supported. 3: Format error; 4: The data is too long.
             5: The pairing fails. 6: pairing timeout; 7: failed to send. 8: The message is sent successfully.     
Others:         
**********************************************************/
uint8_t BMC56M001::setHoppPeriod(uint8_t period[])
{
   uint8_t sendBuf[6] = {0x01, 0x15, 0x02, 0x00, 0x00};
   sendBuf[3] = period[0];
   sendBuf[4] = period[1];
   uint8_t buff[5] = {0};
   uint8_t leng;
   sendBuf[5] = checksum(5,sendBuf);
   writeBytes(sendBuf,6);
   if(readBytes(buff,&leng) == CHECK_OK)
     return buff[3];
   else 
     return 1;
}

/**********************************************************
Description: Set the RF communication address
Parameters:  address[]: The address that needs to be set(4bytes)  
Return:      0: The command is executed successfully. 1: Command execution fails.
             2: The command is not supported. 3: Format error; 4: The data is too long.
             5: The pairing fails. 6: pairing timeout; 7: failed to send. 8: The message is sent successfully.    
Others:           
**********************************************************/
uint8_t BMC56M001::setRFAddress(uint8_t address[])
{
   uint8_t sendBuf[8] = {0x01, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00};
   sendBuf[3] = address[0];
   sendBuf[4] = address[1];
   sendBuf[5] = address[2];
   sendBuf[6] = address[3]; 
   uint8_t buff[5] = {0};
   uint8_t leng;
   sendBuf[7] = checksum(7,sendBuf);
   writeBytes(sendBuf,8);
   if(readBytes(buff,&leng) == CHECK_OK)
     return buff[3];
   else 
     return 1; 
}

/**********************************************************
Description: Set the UART communication baud rate
Parameters:  baudRate:
                      BR_9600: 9600bps
                      BR_19200: 19200bps
                      BR_38400: 38400bps     
Return:      0: The command is executed successfully. 1: Command execution fails.
             2: The command is not supported. 3: Format error; 4: The data is too long.
             5: The pairing fails. 6: pairing timeout; 7: failed to send. 8: The message is sent successfully.    
Others:       
**********************************************************/
uint8_t BMC56M001::setBaudRate(uint8_t baudRate)
{
   uint8_t sendBuf[5] = {0x01, 0x01, 0x01, 0x00};
   sendBuf[3] = baudRate; 
   uint8_t buff[5] = {0};
   uint8_t leng; 
   sendBuf[4] = checksum(4,sendBuf);
   writeBytes(sendBuf,5);
   if(readBytes(buff,&leng) == CHECK_OK)
     return buff[3];
   else 
     return 1; 
}

/**********************************************************
Description: Obtain the RF communication address
Parameters:  address[]: Used to store the obtained Mailing address (4bytes)
Return:      0: The command is executed successfully. 1: Command execution fails.
             2: The command is not supported. 3: Format error; 4: The data is too long.
             5: The pairing fails. 6: pairing timeout; 7: failed to send. 8: The message is sent successfully.    
Others:               
**********************************************************/
uint8_t BMC56M001::getRFAddress(uint8_t address[])
{  
   uint8_t sendBuf[4] = {0x01, 0x84, 0x00};
   uint8_t buff[9] = {0};
   uint8_t leng;
   sendBuf[3] = checksum(3,sendBuf);
   writeBytes(sendBuf,4);
   if(readBytes(buff,&leng) == CHECK_OK)
   {
      address[0] = buff[7];
      address[1] = buff[6];
      address[2] = buff[5];
      address[3] = buff[4];
      return buff[3];
   }
   else 
      return 1; 
}

/**********************************************************
Description: checksum
Parameters:  len:data length
             data[]:Communication frame content
Return:      checksum
Others:           
**********************************************************/
uint8_t BMC56M001::checksum(uint8_t len,uint8_t data[])
{
  uint8_t a=0;
  uint8_t result=0;
  
  for(a=0;a<len;a++)
    result  += data[a];
  result = ~result+1;
  return result;
}

/**********************************************************
Description: writeBytes
Parameters:  wbuf[]:Variables for storing Data to be sent
             wlen:Length of data sent  
Return:   
Others:
**********************************************************/
void BMC56M001::writeBytes(uint8_t wbuf[], uint8_t wlen)
{
  /* Select SoftwareSerial Interface */
  if (_softSerial != NULL)
  {
    while (_softSerial->available() > 0)
    {
      _softSerial->read();
    }
    _softSerial->write(wbuf, wlen);
  }
  /* Select HardwareSerial Interface */
  else
  {
    while (_hardSerial->available() > 0)
    {
      _hardSerial->read();
    }
    _hardSerial->write(wbuf, wlen);
  }
}

/**********************************************************
Description: readBytes
Parameters:  rbuf[]:Variables for storing Data to be obtained
             *rlen:Length of data to be obtained
Return:   
Others:
Note:(修改，改變收數據的方式，由原來的用delay等數據發完，改為邊發邊讀)
**********************************************************/
uint8_t BMC56M001::readBytes(uint8_t rbuf[], uint8_t *rlen, uint16_t timeOut)
{
  uint8_t i = 0, delayCnt = 0, checkSum = 0, datalength = 0;
  /* Select SoftwareSerial Interface */
  if(_softSerial != NULL)
  {
    delayCnt = 0;
    while(_softSerial->available() < 3)
    {
      if (delayCnt > timeOut)
      {
        return TIMEOUT_ERROR;                    // Timeout error
      }
      delay(1);
      delayCnt++;
    }
    for(i = 0; i < 3; i++)
    {
      rbuf[i] = _softSerial->read();
    }
    datalength = (rbuf[2] + 1);
  }
  /* Select HardwareSerial Interface */
  else
  {
     delayCnt = 0;
     while(_hardSerial->available() < 3)
     {
      if (delayCnt > timeOut)
      {
        return TIMEOUT_ERROR;                    // Timeout error
      }
      delay(1);
      delayCnt++;
    }
    for(i = 0; i < 3; i++)
    {
      rbuf[i] = _hardSerial->read();
    }
    datalength = (rbuf[2] + 1);
 }
  
/* Select SoftwareSerial Interface */
  if (_softSerial != NULL)
  {
    for (i = 0; i < datalength; i++)
    {
      while (_softSerial->available() == 0)
      {
        if (delayCnt > timeOut)
        {
          return TIMEOUT_ERROR;                    // Timeout error
        }
        delay(1);
        delayCnt++;
      }
      rbuf[i+3] = _softSerial->read();
    }
    datalength = (rbuf[2] + 3);
  }
/* Select HardwareSerial Interface */
  else
  { 
    for (i = 0; i < datalength; i++)
    {
      while (_hardSerial->available() == 0)
      {
        if (delayCnt > timeOut)
        {
          return TIMEOUT_ERROR;                   // Timeout error
        }
        delay(1);
        delayCnt++;
      }
      rbuf[i+3] = _hardSerial->read();
    }
    datalength = (rbuf[2] + 3);
  }
  /* check Sum */
  for (i = 0; i < datalength; i++)
  {
    checkSum += rbuf[i];
  }
  checkSum = ((~checkSum)+1);
  if (checkSum == rbuf[datalength])
  {
    *rlen = datalength;
    return CHECK_OK; // Check correct
  }
  else
  {
    *rlen = datalength;
    return CHECK_ERROR; // Check error
  }
}
