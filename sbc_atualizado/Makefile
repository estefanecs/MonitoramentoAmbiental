all: sbc

sbc: sbc.c
	gcc sbc.c ads1115_rpi.c analogico.c dht11.c displayLCD.c mqtt.c -o sbc -lmosquitto -l wiringPi -l wiringPiDev -l pthread

clean:
	rm sbc
