
## READ Temperature over BLE

This project shows how to write arduino code for the nRF52832, such as the Sparkfun breakout board, using a DS18B20 temperature sensor using an interrupt routine. It also shows how to use BLE to make a Raspberry PI read this data from the nRF52 board.

### Sub Folders

* **nrf52Blink** - *Simple program to test/show the interrupt routine*
* **DallasTempOnSparfun_nrf52832** - *Main Arduino program*
* **readTemp** - *[Go](https://golang.org/) program to read the temperature data on a rPI over BLE*
  
*See details below.*

### Prerequisites
You need [Sparkfun nRF52832 Breakout board](https://learn.sparkfun.com/tutorials/nrf52832-breakout-board-hookup-guide) or similar, with the Arduino environment up and running. You need some kind of 3.3v FTDI to program the board over the serial port. You need a [DS18B20](https://www.sparkfun.com/products/11050) temperature sensor. For outdoor use I suggest the waterproof model. 
To read the data over BLE I suggest you use a  raspberry Pi3, with the [Go](https://golang.org/) compiler environment installed, unless you choose to cross compile on some other machine.


### nrf52Blink
Using interrupt routines is a convenient way of letting the main loop do its normal stuff without being blocked by handling routines that don't need to be run as part of the main loop.  This small program shows the basics how that can be accomplished. It can be used to evaluate the prescaler and possibly counter resolution for values that suits your need.

### DallasTempOnSparfun_nrf52832
This program is loosely based in the *temp_sensor* sample in the package [arduino-BLEPeripheral](https://github.com/sandeepmistry/arduino-BLEPeripheral) by Sandeep Mistry. Biggest change is the replacement of the TimerOne library in favor of the interrupt routine from the sample above, and the use of the DS18B20 sensor instead of the DHT. I preffer the water proof Dallas sensor due to weather requrements - think direct rain.

I use pin #31 for the 1wire bus as it is close to the power pins on the sparkfun board. I want to solder wires to just parts of the board and it's convenient to not have them spread out.

### readTemp
To show how to read the data for any post processing I have a small demo program written in Go. Start by cloning the [paypal/gatt](https://github.com/paypal/gatt) library. Follow the [instructions](https://godoc.org/github.com/paypal/gatt) on how to use. Then try to find your device by doing 

	go build examples/discoverer.go
	sudo ./discoverer

You should see someting like this as part of the list.

	
	Peripheral ID:D0:54:5A:92:8A:8B, NAME:(Temperature)
	  Local Name        = Temperature
	  TX Power Level    = 0
	  Manufacturer Data = []
	  Service Data      = []

In the readTemp folder do

	go get github.com/paypal/gatt
	go build ./readTemp.go 
	sudo ./readTemp D0:54:5A:92:8A:8B	

Using the Peripheral ID from the discover run. You should expect something like

	Connected
	Service: ccc0
	  Characteristic  ccc1
	    properties    read notify 
	    value     2.500000 |  00002040 | "\x00\x00 @"
	  Descriptor      2902 (Client Characteristic Configuration)
	    value         0000 | "\x00\x00"
	  Descriptor      2901 (Characteristic User Description)
	    value         54656d702043656c73697573 | "Temp Celsius"

Where 2.5C is the temperature reading.

### Background

A while back I wanted to do a small project where I would read the temperature at an outdoor location where I did not have access to power, nor wanted to run any cable nor wires.

The solution that came to mind was using a small Arduino board with a DS18B20 outdoor temperature sensor. Then use BLE to make the data availabe for a Raspberry pi that would then relay the data upstream.

### License
This code is [licensed](https://github.com/sandeepmistry/arduino-BLEPeripheral/blob/master/LICENSE) under the [MIT Licence](https://en.wikipedia.org/wiki/MIT_License).



### Acknowledgments
* [Sandeep Mistry](https://github.com/sandeepmistry) for the BLE Peripheral library
* [The Paypal/gatt team](https://github.com/paypal/gatt/graphs/contributors) for the Go gatt library
* The ppl over at [Nordic Semiconductor](https://devzone.nordicsemi.com) for writing samples that I copied and pasted into my code to create the interrupt routine.


