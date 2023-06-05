# ESP32 Green House Sensor Node

![3D View](https://user-images.githubusercontent.com/95383090/230792590-40fd404d-087f-4cdb-9712-f6d4d5ffccc1.png)


 ## Overview
  ESP32-C3-MINI-1-N4 powered Sensor Node for Greenhouse data logging. Equipped with Temperature/Humidity sensor (I2C), Gas sensor (I2C), Ambient Light sensor (Analog) and UART to RS232 converter.
 
 • ESP32-C3-MINI-1-N4
 
 • SHTC3-1000 Temperature/Humidity Sensor (I2C)
 
 • AGS10 Gas Sensor (I2C)
 
 • BH1621FVC-TR Ambient Light Sensor (Analog)
 
 • RS232 transceiver
 
 
 ## Revisions
 
 ### <b> 2.0 </b>
 
 • Added decoupling capacitors to U4, U5, U6.
 
 • Rearranged bootstrapping pins.
 
 
 ### <b> 1.0 </b>
 
 • Initial Release.

 ## References
 
 ### <b> Hardware </b>
 
 [1] "How to Make Custom ESP32 Board in 3 Hours | Full Tutorial", Youtube, Robert Feranec,
  https://www.youtube.com/watch?v=S_p0YV-JlfU&t=9782s&ab_channel=RobertFeranec (Accessed 2023)
  
  
 [2] "ESP32 + PCB Antenna Hardware Design Tutorial - Phil's Lab #90", Youtube, Phil's Lab,
  https://www.youtube.com/watch?v=yxU_Kw2de08  (Accessed 2023)
  
  
 
  ### <b> Firmware </b>
  
  
  [3] "AGS02MA", Github, RobTilaart, 
   https://github.com/RobTillaart/AGS02MA (Accessed 2023)
   
   
  [4] "SHTC3", Github, SparkFun, 
   https://github.com/RobTillaart/AGS02MA (Accessed 2023)
   
   
  [5] "ESP32: Webserver, Websockets and JSON (WiFi Part 2) - Arduino", Youtube, MoThunderz,
   https://www.youtube.com/watch?v=15X0WvGaVg8&t=844s&ab_channel=MoThunderz (Accessed 2023)
   
   
  [6] "FIR filter", Github, Phil's Lab, 
   https://github.com/pms67/LittleBrain-STM32F4-Sensorboard/blob/master/Firmware/Core/Src/FIRFilter.c
