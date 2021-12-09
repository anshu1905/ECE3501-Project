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
    image.save(filename)

l=[]

while True:
    data=ser.readline()
    print(data)
    time.sleep(1)
    l.append(data)
    data_sstv(l)
    print("done")


