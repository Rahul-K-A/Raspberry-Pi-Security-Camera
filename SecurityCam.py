#includes 
from cv2 import data
import numpy as np
import cv2
import datetime
import paho.mqtt.client as mqtt #import the client1
import time
import glob
import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.base import MIMEBase
from email.mime.text import MIMEText
from email.utils import formatdate
import datetime
from email import encoders
import os
import sys

#Name of video and timestap
FileName=""
DateAndTime=""

#Email params
COMMASPACE = ', '
FromEmail = 'Add your sender here '
LoginName = 'Email ID of the camera'
LoginPassword = 'Password of LoginName'
ToEmail='<Add your reciever here>'
SMTPserver = 'smtp.gmail.com'
SMTPort = 587
Subject = "SecurityAlert"
log = 'logfile.txt'

#Callback
def on_connect(client, userdata, flags, rc):
    if rc==0:
        print("connected OK Returned code=",rc)
    else:
        print("Bad connection Returned code=",rc)

def on_subscribe(client, userdata, mid, granted_qos):
    print("Subscribed", client, userdata, mid, granted_qos)

def on_message(Client, Userdata, Message):
    SensorMessage=str(Message.payload.decode("utf-8"))
    if SensorMessage=="MOTION DETECTED":
        print("Motion was detected")
        print("Video starting")
        CaptureVideo()
        SendMail(LoginName, LoginPassword,ToEmail,FromEmail,Subject,GetText(), FileName, SMTPserver, SMTPort)

#Record video
def CaptureVideo():
        global FileName
        global DateAndTime
        #Get date and time of video for file name
        DateAndTime=datetime.datetime.now().strftime("%I_%M_%S_%p_%B_%d_%Y")
        StartTime = int(time.time())
        #Start video stream
        VideoStream = cv2.VideoCapture(0)
        FileName="Videos/"+DateAndTime+".avi"
        # Define the codec and create VideoWriter object
        fourcc = cv2.VideoWriter_fourcc(*'XVID')
        Output = cv2.VideoWriter(FileName,fourcc,30.0, (640,480))

        while(VideoStream.isOpened()):
            CurrentTime=int(time.time())
            TimeElapsed =CurrentTime-StartTime 
            if TimeElapsed>=6:
                break
            ret, Frame = VideoStream.read()
            if ret==True:
                Output.write(Frame)
                if cv2.waitKey(1) & 0xFF == ord('q'):
                    break
            else:
                break
        VideoStream.release()
        Output.release()
        cv2.destroyAllWindows()
        print('video done')    

#Send email
def SendMail(login, password, send_to, send_from, Subject, text, send_file, server, port):
    #print("Sending Mail")
    Email = MIMEMultipart()
    Email['From'] = send_from
    Email['To'] = COMMASPACE.join(send_to)
    Email['Date'] = formatdate(localtime=True)
    Email['Subject'] = Subject

    Email.attach(MIMEText(text))

    VideoFile = MIMEBase('application', "octet-stream")
    VideoFile.set_payload(open(FileName, "rb").read())
    encoders.encode_base64(VideoFile)
    VideoFile.add_header('Content-Disposition', 'attachment; FileName="%s"' % os.path.basename(send_file))
    Email.attach(VideoFile)

    smtp = smtplib.SMTP(SMTPserver, SMTPort)
    smtp.set_debuglevel(1)
    smtp.ehlo()
    smtp.starttls()
    smtp.login(login, password)
    smtp.sendmail(send_from, send_to, Email.as_string())
    smtp.close()



#Returns the body of email
def GetText():
    global DateAndTime
    Text = "A new file has been added to the security footage folder. \nTime Stamp: " + DateAndTime
    return (Text)



def main():
    Broker_Address="YOUR MQTT SERVER IP"
    LocalUser="YOUR MQTT SERVER USER"
    LocalPass="YOUR MQTT SERVER PASSWORD"
    print("creating new instance")
    Client = mqtt.Client("P1",False,None,mqtt.MQTTv311) #create new instance
   #Attach functions to callbacks
    Client.on_message=on_message 
    Client.on_connect=on_connect
    Client.on_subscribe=on_subscribe
    Client.username_pw_set(LocalUser,LocalPass)
    #Connect to broker
    print("connecting to broker")
    Client.connect(Broker_Address,keepalive=1000) 
    #Create new thread to handle messages
    Client.loop_start()
    #Subscribe to topic
    Client.subscribe("HomeSecurity/Motion")
    #Loop forever
    Client.loop_forever()
    time.sleep(4) # wait


if __name__=="__main__":
    main()
