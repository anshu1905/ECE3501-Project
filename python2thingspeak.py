#Receiving sensor data back from the image and uploading it on Thingspeak Cloud Platform

#importing libraries
from os import closerange
import os.path
from os import path
from PIL import Image
import pytesseract as tess
from masterclass import *
tess.pytesseract.tessetact_cmd = r'C:/Users/Anshuman/AppData/Local/Programs/Python/Python37/tessdata.exe'

#Write and Read API Key of thingspeak channel 
w_key = 'QRGSY40RJD053AED'
r_key = 'LBW5R681NXWE988U'
#thingspeak channel id
channel_id = 1578749
ob = Thingspeak(write_api_key=w_key, read_api_key=r_key, channel_id=channel_id)

#check if the data is float type (temperature and humidity)
def check_float(potential_float):
    try:
        float(potential_float)
        return True
    except ValueError:
        return False

#check if the data is integer type (smoke and rain)
def check_int(potential_integer):
    try:
        int(potential_integer)
        return True
    except ValueError:
        return False

for i in range(1,25):
    image = 'output_sstv_'+str(i)+'.png'
    #load the image for OCR
    if path.exists(image)==True:
        text = tess.image_to_string(Image.open(image), lang="eng")
        #extract the text/sensor data from the image
        print(text)
        if (check_float(text[11:16])==True) & (check_float(text[30:35])==True) & (check_int(text[41:42])==True) & (check_int(text[48:49])==True):
            humidity=float(text[11:16]) #humidity data
            temperature=float(text[30:35]) #temperature data 
            gas=int(text[41:42]) #smoke data 
            rain=int(text[48:49]) #rain data
            ob.post_cloud(value1=temperature,value2=humidity,value3=gas,value4=rain)#upload on thingspeak to the respective field values
            print("Sent to thingspeak")
        
    elif path.exists(image)==False:
        print("Image not found")
        print("\n")


s
