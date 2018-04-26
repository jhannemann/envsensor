import struct

class Station:
    def __init__(self, id):
        self.id = id
        self.timestamps = []
        self.temperatures = []
        self.humidities = []
        self.pressure = []

stations = {}

with open('data', 'rb') as binary_file:
    while True:
        bytes = binary_file.read(10)
        if not bytes: break
        # Feather M0 is little-endian, hence the '<'
        # Packet layout is
        # +---+------+----+---+-----+
        # | ID|  TS  |Temp|Hum|Press|
        # +---+------+----+---+-----+
        # | 1 |   4  | 2  + 1 +  2  |
        # +---+------+----+---+-----+
        # All numbers, except for temperature, are unsigned 
        fields = struct.unpack('<BIhBH', bytes)
        id = fields[0]
        timestamp = fields[1]
        temperature = fields[2]/10.0
        humidity = fields[3]
        pressure = fields[4]/10.0
        print(id, timestamp, temperature, humidity, pressure)
        if id not in stations.keys():
            stations[id] = Station(id)
        stations[id].timestamps.append(timestamp)
        stations[id].temperatures.append(temperature)
        stations[id].humidities.append(humidity)
        stations[id].pressure.append(pressure)


