from flask import Flask, request, send_file
import RPi.GPIO as GPIO
import time
import threading
import io
import pygame
from picamera import PiCamera
from flask import Response
import pyaudio
import wave
from werkzeug.utils import secure_filename
import os


app = Flask(__name__)

# Set up GPIO pins for servo
servo_pin = 27
GPIO.setmode(GPIO.BCM)
GPIO.setup(servo_pin, GPIO.OUT)
servo = GPIO.PWM(servo_pin, 50)
servo.start(0)

upload_folder = '/home/fyp2'
# Endpoint for playing a sound
@app.route('/upload_sound', methods=['POST'])
def upload():
    # Check if the POST request has the file part
    if 'file' not in request.files:
        return 'No file part'

    file = request.files['file']

    # If the user does not select a file, the browser submits an empty file
    if file.filename == '':
        return 'No selected file'

    # Secure filename and save file to upload folder
    filename = secure_filename(file.filename)
    file.save(os.path.join(upload_folder, filename))

    # Play the uploaded MP3 file
    pygame.mixer.init()
    pygame.mixer.music.load(os.path.join(upload_folder, filename))
    pygame.mixer.music.play()
    while pygame.mixer.music.get_busy():
        continue
    return 'File uploaded and playing'


# Endpoint for moving servo clockwise 15 degrees
@app.route('/move-servo-right', methods=['GET'])
def move_servo_right():
    servo.ChangeDutyCycle(2)
    time.sleep(1)
    servo.ChangeDutyCycle(0)
    return 'Servo moved right'

# Endpoint for moving servo to center
@app.route('/move-servo-center', methods=['GET'])
def move_servo_center():
    servo.ChangeDutyCycle(7)
    time.sleep(1)
    servo.ChangeDutyCycle(0)
    return 'Servo moved center'


# Endpoint for moving servo counterclockwise 15 degrees
@app.route('/move-servo-left', methods=['GET'])
def move_servo_left():
    servo.ChangeDutyCycle(10)
    time.sleep(1)
    servo.ChangeDutyCycle(0)
    return 'Servo moved left'

# set the audio format
FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 44100
CHUNK = 1024
RECORD_SECONDS = 20  # changed to 20 seconds
WAVE_OUTPUT_FILENAME = "output.wav"

# initialize the PyAudio object
audio = pyaudio.PyAudio()

# define the Flask endpoint for recoding audio
@app.route('/record', methods=['GET'])
def record():
    # open the audio stream
    stream = audio.open(format=FORMAT,
                        channels=CHANNELS,
                        rate=RATE,
                        input=True,
                        frames_per_buffer=CHUNK)
    print("Recording...")

    frames = []
    for i in range(0, int(RATE / CHUNK * RECORD_SECONDS)):
        data = stream.read(CHUNK)
        frames.append(data)

    # stop the audio stream
    stream.stop_stream()
    stream.close()

    # terminate the PyAudio object
    audio.terminate()

    # save the audio file
    wf = wave.open(WAVE_OUTPUT_FILENAME, 'wb')
    wf.setnchannels(CHANNELS)
    wf.setsampwidth(audio.get_sample_size(FORMAT))
    wf.setframerate(RATE)
    wf.writeframes(b''.join(frames))
    wf.close()

    # return the audio file
    return send_file(WAVE_OUTPUT_FILENAME, as_attachment=True)

#initializing temperature and humidity sensor
GPIO.setmode(GPIO.BOARD)
sensor = Adafruit_DHT.DHT11
pin = 23
GPIO.setup(pin, GPIO.IN)

# define the endpoint for getting the temperature and humidity
@app.route('/temp', methods=['GET'])
def temp():
    humidty, temperature = Adafruit_DHT.read_retry(sensor, pin,2)
    if humidty is None and temperature is None:
        print("Failed to get reading. Try Again!")
    else:
        print("Temp={0:0.1f}*C Humidity={1:0.2f}%".format(temperature,humidity))

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=6600, debug=True)

