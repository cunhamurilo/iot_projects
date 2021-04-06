import sys
import time
from machine import Pin

import array
import micropython
import utime
from micropython import const

class InvalidChecksum(Exception):
    pass

class InvalidPulseCount(Exception):
    pass

MAX_UNCHANGED = const(100)
MIN_INTERVAL_US = const(200000)
HIGH_LEVEL = const(50)
EXPECTED_PULSES = const(84)

class DHT11:
    _temperature: float
    _humidity: float

    def __init__(self, pin):
        self._pin = pin
        self._last_measure = utime.ticks_us()
        self._temperature = -1
        self._humidity = -1

    def measure(self):
        current_ticks = utime.ticks_us()
        if utime.ticks_diff(current_ticks, self._last_measure) < MIN_INTERVAL_US and (
            self._temperature > -1 or self._humidity > -1
        ):
            # Less than a second since last read, which is too soon according
            # to the datasheet
            return

        self._send_init_signal()
        pulses = self._capture_pulses()
        buffer = self._convert_pulses_to_buffer(pulses)
        self._verify_checksum(buffer)

        self._humidity = buffer[0] + buffer[1] / 10
        self._temperature = buffer[2] + buffer[3] / 10
        self._last_measure = utime.ticks_us()

    @property
    def humidity(self):
        # self.measure()
        return self._humidity

    @property
    def temperature(self):
        # self.measure()
        return self._temperature

    def _send_init_signal(self):
        self._pin.init(Pin.OUT, Pin.PULL_DOWN)
        self._pin.value(50)
        utime.sleep_ms(8)
        self._pin.value(0)
        utime.sleep_ms(18)

    @micropython.native
    def _capture_pulses(self):
        pin = self._pin
        pin.init(Pin.IN, Pin.PULL_UP)

        val = 1
        idx = 0
        transitions = bytearray(EXPECTED_PULSES)
        unchanged = 0
        timestamp = utime.ticks_us()

        while unchanged < MAX_UNCHANGED:
            if val != pin.value():
                # if idx >= EXPECTED_PULSES:
                #     raise InvalidPulseCount(
                #         "Got more than {} pulses".format(EXPECTED_PULSES)
                #     )
                now = utime.ticks_us()
                transitions[idx] = now - timestamp
                timestamp = now
                idx += 1

                val = 1 - val
                unchanged = 0
            else:
                unchanged += 1
        pin.init(Pin.OUT, Pin.PULL_DOWN)
        # if idx != EXPECTED_PULSES:
        #     raise InvalidPulseCount(
        #         "Expected {} but got {} pulses".format(EXPECTED_PULSES, idx)
        #     )
        return transitions[4:]

    def _convert_pulses_to_buffer(self, pulses):
        """Convert a list of 80 pulses into a 5 byte buffer
        The resulting 5 bytes in the buffer will be:
            0: Integral relative humidity data
            1: Decimal relative humidity data
            2: Integral temperature data
            3: Decimal temperature data
            4: Checksum
        """
        # Convert the pulses to 40 bits
        binary = 0
        for idx in range(0, len(pulses), 2):
            binary = binary << 1 | int(pulses[idx] > HIGH_LEVEL)

        # Split into 5 bytes
        buffer = array.array("B")
        for shift in range(4, -1, -1):
            buffer.append(binary >> shift * 8 & 0xFF)
        return buffer

    def _verify_checksum(self, buffer):
        # Calculate checksum
        checksum = 0
        for buf in buffer[0:4]:
            checksum += buf
        if checksum & 0xFF != buffer[4]:
            raise InvalidChecksum()


#########################  DISPLAY #########################
### functions for print in display 7 segments with 4 digits    

delay = 0.001 # delay between digits refresh
selDigit_list = [14,15,16,13]
selDigit = []
# Digits:   1, 2, 3, 4

display_list = [27, 26, 22, 21, 20, 19, 18] # define GPIO ports to use
#disp.List ref:  A,  B,  C,  D,  E,  F,  G
display = []

digitDP = 17
#DOT = GPIO 17

# Set all pins as output
for pin in selDigit_list:
  selDigit.append(Pin(pin, Pin.OUT))

for pin in display_list:
  display.append(Pin(pin, Pin.OUT))
  
dp = Pin(digitDP, Pin.OUT)

# DIGIT map as array of array ,
#so that arrSeg[0] shows 0, arrSeg[1] shows 1, etc
arrSeg = [[1,1,1,1,1,1,0],\
          [0,1,1,0,0,0,0],\
          [1,1,0,1,1,0,1],\
          [1,1,1,1,0,0,1],\
          [0,1,1,0,0,1,1],\
          [1,0,1,1,0,1,1],\
          [1,0,1,1,1,1,1],\
          [1,1,1,0,0,0,0],\
          [1,1,1,1,1,1,1],\
          [1,1,1,1,0,1,1]]

celsius = [[1,1,0,0,0,1,1],\
           [1,0,0,1,1,1,0]]

dp.low() # DOT pin

def showDisplay(digit, option):
  for i in range(0, 4):
    sel = [1,1,1,1]
    sel[i] = 0

    values = []
    if option == 'temp' and i > 1:
      values = celsius[i-2]

    else:  
      numDisplay = int(digit[i].replace(".", ""))
      values = arrSeg[numDisplay]
    for j in range(0, len(display)):
      if values[j] == 1:
        display[j].low()
      else:
        display[j].high()

    if option != 'temp':
      if digit[i].count(".") == 1:
        dp.low()
      else:
        dp.high()
    else:
      dp.high()
    
    selDigit[i].high()
    time.sleep(0.001)
    selDigit[i].low()

def splitToDisplay (toDisplay): # splits string to digits to display
  arrToDisplay=list(toDisplay)
  for i in range(len(arrToDisplay)):
    if arrToDisplay[i] == ".": arrToDisplay[(i-1)] = arrToDisplay[(i-1)] + arrToDisplay[i] # dots are concatenated to previous array element
  while "." in arrToDisplay: arrToDisplay.remove(".") # array items containing dot char alone are removed

  return arrToDisplay

led = Pin(25, Pin.OUT)

aux = True
change = False

temp = '00.0'

pin = Pin(28, Pin.OUT, Pin.PULL_DOWN)
sensor = DHT11(pin)

try:
  while True:
    # print('reading..')
    ltime = time.localtime()
    
    # fuso horario
    htime = ltime[3] - 3
    htime = htime if htime >= 0 else 24 + htime

    h = '0'+ str(htime) if htime < 10 else str(htime) 
    m = '0'+ str(ltime[4]) if ltime[4] < 10 else str(ltime[4])
    hour = str(h+'.'+m)
    
    if aux == True:
      showDisplay(splitToDisplay(hour), 'hour')
    else:
      showDisplay(splitToDisplay(temp), 'temp')
    
    if ltime[5] % 10 == 0:
      if change == False:
        if aux == False:
            aux = True
        else:
            aux = False
            sensor.measure()
            t = (sensor.temperature)
            h = (sensor.humidity)
            if(t > 0):
                # print('Temperature: ', t)
                # print('Humidity: ', h)
                temp = str(t)
        change = True

        led.toggle()
    else:
      change = False

except KeyboardInterrupt:
  print('interrupted!')
  sys.exit()
