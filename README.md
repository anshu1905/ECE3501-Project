# ECE3501 IoT Fundamentls Project
## L47+L48
## Team Members
* Anshuman Phadke – 19BEC0428
* Anirudh Karnik – 19BEC0353
* Arvind N – 19BEC0564

# Project Description

## Software Used
* Arduino IDE
* Python IDLE
* Thingspeak Data Analytics Tool

## Hardware Used
* Arduino Uno 
* DHT11 Temperature and Humidity Sensor
* Gas Sensor 
* Rain Sensor

## To run the python files install the dependencies using
```
pip install pyserial
pip install time
pip install os-win
pip install pyteserract
pip install pillow
```

## Make the following ciruit connections   
<p align="center">
  <img src="">
</p>

## Upload the following code to the Arduino Uno Board
```
data_fetch_sstv.ino
```

## For transmitting the image(data) through sstv run the following 
```
python sstv_image_send.py
```

## For receiving back the image(data) and putting it up on thingspeak run the following 
```
python python2thingspeak.py
python masterclass.py
```

## Thingspeak Channel

https://thingspeak.com/channels/1578749

## Video Demonstration

https://drive.google.com/drive/u/1/folders/1sVIIqHa9b1y_xpHrPC9CtE0m2hM2E_ia
