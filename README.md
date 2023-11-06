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
