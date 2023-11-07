# Installing libraries
1. Copy the library folder to ``C:\Users\<username>\.platformio\packages\framework-arduinoespressif32\libraries``
2. Start PlatformIO home
3. Install / uninstall a random library to get CLion to refresh the libraries


# Configure WIFI SSID / password
To prevent confidential WIFI credentials from getting pushed to github, you have to create a file called ``wifi.ini`` 
in the root directory, which contains the following text
```ini
[wifi]
ssid = your-wifi-ssid
password = your-super-secret-wifi-password
```


# Register device in router
1. Create face ``nfdc face create udp4://<ip>:6363``
2. Add route ``nfdc route add <prefix> <id from step 1>``

To show all routes run ``nfdc route list``


# Pin Layout
See [this link](https://raw.githubusercontent.com/AchimPieters/esp32-homekit-camera/master/Images/ESP32-38%20PIN-DEVBOARD.png)
The [corresponding blog](https://www.studiopieters.nl/esp32-pinout/) includes some tips on which pins to use
for the ESP-WROOM-32