import sys, time
from datetime import datetime
import serial

print(f"Connecting to serial port {sys.argv[1]}...")
ser = serial.Serial(sys.argv[1])
found = False
chars = "/-\\|"
char_i = 0
while not found:
    sys.stdout.write(f"\rPinging device...{chars[char_i]}")
    char_i = (char_i+1)%4
    ser.write(b"P")
    time.sleep(1)
    if (ser.in_waiting == 3 and ser.read(1).decode() == 'P'):
        print("\rPinging device...found.")
        found = True

print("Writing control code...")
ser.write(b"T")
ts : int = round(datetime.now().timestamp())
print(f"Sending timestamp({ts})...")
ser.write(f"{ts}\n".encode('utf-8'))
time.sleep(1)
print(f"Response: {repr(ser.read(ser.in_waiting).decode())}")
ser.close()