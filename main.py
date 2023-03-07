import serial
from cvlib.object_detection import YOLO
import cv2

arduino = serial.Serial(
    port = "/dev/ttyUSB0",                                         #ls /dev/tty* | grep -E "USB|ACM" растет на 1 каждое переподключение arduino
    baudrate = 115200,
    bytesize = serial.EIGHTBITS,
    parity = serial.PARITY_NONE,
    stopbits = serial.STOPBITS_ONE
)


def forward(dist):                                                  #движение прямо
    arduino.write(f"mov|{dist}|".encode())
    print(arduino.readline().decode())


def rotate(angle):                                                  #поворот на месте
    arduino.write(f"setAngle|{angle * 2}|".encode())
    print(arduino.readline().decode())


def rad_move(angle, ty):                                            #поворот одним колесом
    if ty != "right":                                               #2 аргумент right - едет правым колесом
        arduino.write(f"radius_rotation|{angle}|1|".encode())
    else:
        arduino.write(f"radius_rotation|{angle}|0|".encode())
    print(arduino.readline().decode())


def curve_move(radius, angle, ty):                                  #поворот по дуге
    if ty != "right":                                               #3 аргумент right - едет правым больше
        arduino.write(f"arc_mov|{radius}|{angle}|1|".encode())
    else:
        arduino.write(f"arc_mov|{radius}|{angle}|0|".encode())
    print(arduino.readline().decode())


def change_speed(speed):                                            #смена скорости
    arduino.write(f"changes|{speed}|".encode())
    print(arduino.readline().decode())


def change_accel(speed):                                            #смена разгона
    arduino.write(f"changea|{speed}|".encode())
    print(arduino.readline().decode())


cap = cv2.VideoCapture(0)
weights = "yolov4-tiny-custom_last.weights"
config = "yolov4-tiny-custom.cfg"
labels = "obj.names"
count = 0
label = []

yolo = YOLO(weights, config, labels)

while not label:                    #идет пока не увидит цифру
    ret, img = cap.read()

    count += 1
    if count % 10 != 0:             #обрабатывает каждый 10-ый кадр
        continue

    img = cv2.resize(img, (680, 460))
    bbox, label, conf = yolo.detect_objects(img)

print(label)