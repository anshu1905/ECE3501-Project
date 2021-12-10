#Getting sensor data from Arduino Uno board and converting this text in the form of an image and transmitting it through SSTV

#importing libraries
import serial
import time
from PIL import Image, ImageDraw, ImageFont
import os

ser=serial.Serial('COM10',9600)

def data_sstv(l):
    fnt = ImageFont.truetype('arial.ttf', 15)
    image = Image.new(mode ="RGB", size = (1000,1000), color = "white")
    draw = ImageDraw.Draw(image) 
    filename = "output_sstv_"+str(len(l))+".png"
    text=""
    for j in range(len(l)-1,len(l)):
        text=str(l[j])+": "+str(j)+"  "
        draw.text((250,250+j*20),text, font=fnt, fill=(0,0,0))
        #drawing the text onto the image
    image.save(filename)
    #saving the image to transmit it over SSTV

l=[]

while True:
    data=ser.readline()
    #get the data from arduino serially 
    print(data)
    time.sleep(1)
    l.append(data)
    data_sstv(l)
    #function that converts the text to image
    print("done")


