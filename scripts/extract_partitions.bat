@echo off

set PIO_PATH="C:\Users\arne-\.platformio"

python "%PIO_PATH%\packages\tool-esptoolpy\esptool.py" read_flash 0x8000 0xc00 partitions.img
python "%PIO_PATH%\packages\framework-arduinoespressif32\tools\gen_esp32part.py" partitions.img > partitions.csv
rm partitions.img
