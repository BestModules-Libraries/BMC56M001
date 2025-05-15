<div align=center>
<img src="https://github.com/BestModules-Libraries/img/blob/main/BMC56M001_V1.0.png" width="320" height="240"> 
</div> 

BMC56M001
===========================================================

The Best Modules BMC56M001 is a 2.4G GFSK transceiver transparent transmission module, which uses the UART communication method. This document describes the Arduino Lib functions of the BMC56M001 and how to install the Arduino Lib. The provided examples demonstrate how to pair the modules to form a Peer network topology or a Star network topology to implement pairing, wireless communication and other functions.

#### Applicable types:
<div align=center>

|Part No.   |Description                           |
|:---------:|:------------------------------------:|
|BMC56M001|2.4G GFSK Transceiver Transparent Transmission Module|
|BMC56M001A|2.4G GFSK Transceiver Transparent Transmission Module|
</div> 

This library can be installed via the Arduino Library manager. Search for **BMC56M001**. 

Repository Contents
-------------------

* **/examples** - Example sketches for the library (.ino). Run these from the Arduino IDE. 
* **/src** - Source files for the library (.cpp, .h).
* **keywords.txt** - Keywords from this library that will be highlighted in the Arduino IDE. 
* **library.properties** - General library properties for the Arduino package manager. 

Documentation 
-------------------

* **[BMC56M001 Arduino Library Description]( https://www.bestmodulescorp.com/bmc56m001.html#tab-product2 )** - Arduino Library Description.

Version History  
-------------------

* **V1.0.1**  
&emsp;&emsp;- Initial public release.
* **V1.0.2**  
&emsp;&emsp;- Improve wireless communication speed;  
&emsp;&emsp;- Add function: bool isPaired();  
&emsp;&emsp;- Change the usage of the "uint16_t getShortAddress()" function: Can obtain the last paired short address, after re powering on;  
* **V1.0.3**  
&emsp;&emsp;- Add applicable models: BMC56M001A;  
&emsp;&emsp;- Fix the "writeRFData、writePairPackage" function in the .cpp document to ensure its proper use;  
License Information
-------------------

This product is _**open source**_! 

Please review the LICENSE.md file for license information. 

If you have any questions or concerns on licensing, please contact technical support on our BEST MODULES CORP. Email:service@bestmodulescorp.com

Distributed as-is; no warranty is given.

BEST MODULES CORP.
