# Import standard python modules.
import sys
import time
import serial

# This example uses the MQTTClient instead of the REST client
from Adafruit_IO import MQTTClient
from Adafruit_IO import Client, Feed

# holds the count for the feed
run_count = 0

# Set to your Adafruit IO username and key.
ADAFRUIT_IO_USERNAME = "user"
ADAFRUIT_IO_KEY = "key"

# Set to the ID of the feed to subscribe to for updates.
FEED_ID_motor1 = 'Motor1'
FEED_ID_motor2 = 'Motor2'
FEED_ID_motor3 = 'Motor3'
FEED_ID_motor4 = 'Motor4'
FEED_ID_estado = 'ESTADOS'
FEED_ID_posicion = 'Posiciones_EEPROM'

#Funciones extra
def send_message(dato):
    if dato == b'M\n':
        client.publish('ESTADOS', 'M')
    elif dato == b'E\n':
        client.publish('ESTADOS', 'E')
    elif dato == b'S\n':
        client.publish('ESTADOS', 'S')

def to_bytes(c1, c2):
    return bytes([ord(c1), ord(c2) if isinstance(c2, str) else c2])

def velocidad_pwm(velocidad):
    velocidad = max(-100, min(100, velocidad))
    pwm = int(127 + (velocidad*1.28))
    return max(0, min(255, pwm))

def angulo_pwm(angulo):
    angulo = max(0, min(180, angulo))
    pwm = int(angulo*(255/180))
    return max(0, min(255, pwm))

# Define "callback" functions which will be called when certain events 
# happen (connected, disconnected, message arrived).
def connected(client):
    # Subscribe to changes on a feed named Counter.
    print('Subscribing to Feed {0}'.format(FEED_ID_motor1))
    client.subscribe(FEED_ID_motor1)
    client.subscribe(FEED_ID_motor2)
    client.subscribe(FEED_ID_motor3)
    client.subscribe(FEED_ID_motor4)
    client.subscribe(FEED_ID_estado)
    client.subscribe(FEED_ID_posicion)
    print('Waiting for feed data...')

def disconnected(client):
    sys.exit(1)

def message(client, feed_id, payload):
    print('Feed {0} received new value: {1}'.format(feed_id, payload))
    
    # Publish or "send" message to corresponding feed
    if feed_id == 'ESTADOS':
        dato = to_bytes('E', payload)
    if feed_id == 'Posiciones_EEPROM':
        dato = to_bytes('P', payload)

    if feed_id == 'Motor1':
        payload = velocidad_pwm(float(payload))
        dato = to_bytes('W', payload)
    if feed_id == 'Motor2':
        payload = velocidad_pwm(float(payload))
        dato = to_bytes('X', payload)
    if feed_id == 'Motor3':
        payload = angulo_pwm(int(payload))
        dato = to_bytes('Y', payload)
    if feed_id == 'Motor4':
        payload = angulo_pwm(int(payload))
        dato = to_bytes('Z', payload)
    print('Sending Arduino', dato)
    miarduino.write(dato)
    # client.publish(FEED_ID_Send, payload)

try:
    # Create an MQTT client instance.
    client = MQTTClient(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)
    miarduino = serial.Serial(port='COM7', baudrate=9600, timeout=0.1)

    # Setup the callback functions defined above.
    client.on_connect = connected
    client.on_disconnect = disconnected
    client.on_message = message

    # Connect to the Adafruit IO server.
    client.connect()

    # The first option is to run a thread in the background so you can continue
    # doing things in your program.
    client.loop_background()

    while True:
        """ 
        # Uncomment the next 3 lines if you want to constantly send data
        # Adafruit IO is rate-limited for publishing
        # so we'll need a delay for calls to aio.send_data()
        run_count += 1
        print('sending count: ', run_count)
        client.publish(FEED_ID_Send, run_count)
        """
        dato = miarduino.readline()
        send_message(dato)
        print('Running "main loop" ')
        time.sleep(3)
except KeyboardInterrupt:
    print("Adios")
    if miarduino.is_open:
        miarduino.close()
    sys.exit(1)