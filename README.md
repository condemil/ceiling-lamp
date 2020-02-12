# ceiling-light

Drop-in replacement of LED ceiling lamp module ADY-788T with ADY-013 remote to control lamp though wifi.
ESP8266 wifi module is used to send messages though MQTT that can be parsed by home-assistant.

Project includes:

* Firmware sources, requires PlatformIO to build
* Scheme / PCB of the board


Example of Home Assisstant configuration:

```yaml
light:
  - platform: mqtt
    name: Ceiling Light
    schema: json
    state_topic: esp8266/xxxxxx
    command_topic: esp8266/xxxxxx/set
    brightness: true
    color_temp: true
    qos: 1
```
