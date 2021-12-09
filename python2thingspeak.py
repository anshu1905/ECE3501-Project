from os import closerange
import os.path
from os import path
from PIL import Image
import pytesseract as tess
from masterclass import *
tess.pytesseract.tessetact_cmd = r'C:/Users/Anshuman/AppData/Local/Programs/Python/Python37/tessdata.exe'
l=[]
l1=[]

w_key = 'QRGSY40RJD053AED'
r_key = 'LBW5R681NXWE988U'
channel_id = 1578749
ob = Thingspeak(write_api_key=w_key, read_api_key=r_key, channel_id=channel_id)

def check_float(potential_float):
    try:
        float(potential_float)
        return True
    except ValueError:
        return False


for i in range(1,10):
    image = 'output_sstv_'+str(i)+'.png'
    if path.exists(image)==True:
        text = tess.image_to_string(Image.open(image), lang="eng")
        print(text)
        #print(isinstance(text[11:16],float))
        #print(isinstance(text[30:35],float))
        if (check_float(text[11:16])==True) & (check_float(text[30:35])==True):
            humidity=float(text[11:16])
            temperature=float(text[30:35])
            gas=0.00
            rain=0.00
            ob.post_cloud(value1=temperature,value2=humidity,value3=gas,value4=rain)
            print("Sent to thingspeak")
        
    elif path.exists(image)==False:
        print("Image not found")
        print("\n")


