# NDN Arduino Firmware

## Mirror
If you are viewing this from a mirror then please visit `https://github.com/NDNAppBachelorThesis/ndn-arduino` to
access the build artifacts



# Get started developing
1. Install the ESP32 board driver from `https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads`
2. Clone and open this project in CLion
3. Install the platformio plugin for clion
4. Install PlatformIO core [from here](https://docs.platformio.org/en/latest/core/installation/methods/installer-script.html#local-download-macos-linux-windows)
5. In CLions PlatformIO settings configure the path to the core installation. Usually `C:\Users\<username>\.platformio`
6. If CLion doesn't detect your platformio project, you have to create a new `Espressif Esp32 Dev` project and copy the code into it
7. Go to ``C:\Users\<username>\.platformio\packages\framework-arduinoespressif32\libraries`` and clone 
   `https://github.com/yoursunny/esp8266ndn.git` and `https://github.com/yoursunny/NDNph.git`
8. Configure WIFI
9. From the CLion PlatformIO toolbar (top right) run `Upload and Monitor`


# Installing libraries
1. Copy the library folder to ``C:\Users\<username>\.platformio\packages\framework-arduinoespressif32\libraries``
2. Rightlick the `platformio.ini` file and select `PlatformIO > Reload Project`


# Configure WIFI SSID / password
To prevent confidential WIFI credentials from getting pushed to github, you have to create a file called ``wifiClient.ini`` 
in the root directory, which contains the following text
```ini
[wifiClient]
ssid = your-wifiClient-ssid
password = your-super-secret-wifiClient-password
```


# Register device in router
1. Create face ``nfdc face create udp4://<ip>:6363``
2. Add route ``nfdc route add <prefix> <id from step 1>``

To show all routes run ``nfdc route list``


# Pin Layout
See [this link](https://raw.githubusercontent.com/AchimPieters/esp32-homekit-camera/master/Images/ESP32-38%20PIN-DEVBOARD.png)
The [corresponding blog](https://www.studiopieters.nl/esp32-pinout/) includes some tips on which pins to use
for the ESP-WROOM-32
