# Arduino IoT: Hydroponics 

Hydroponics system thru IoT using Arduino, PH-4502C pH Sensor, and DFR0300 EC Sensor.

## Getting started

1. Install `arduino` Arduino IDE using this [link](https://www.arduino.cc/en/software/).

2. Clone this repository using `git` or download it as a zip using this [link](https://github.com/gigaByteSized/arduino-hydroponics/archive/refs/heads/main.zip)

```
git clone https://github.com/gigaByteSized/arduino-hydroponics.git
``` 

3. Extract the zip file 

4. In `arduino`, install the included `DFRobot_EC-master.zip` in 
`Sketch > Include library > Add .ZIP library...`

5. You are now good to go!

## Additional Setup

### Logging with PuTTY

1. Install `putty` using
```
sudo apt-get install putty putty-tools
```

2. Check serial line using Arduino IDE.

```
/dev/tty/USB0.
``` 

3. In PuTTY client, set connection type to 'Serial'. Make sure 'Serial line' is set to correct 'Serial line' and 'speed' is set to '9600'.

4. Go to Serial > Logging (In left "Categories"). Set session logging to 'Printable output'

5. You are now good to go!