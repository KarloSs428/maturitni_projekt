# maturitni_projekt

Cíle projektu:

1) Vytvořit alarmový systém, který by byl schopný včas varovat uživatele před narušitelem(např.: zlodějem a jinými nezvanými hosty)
2) Vytvořit jednoduchou webovou aplikaci, kde by se dalo spravovat nastavení alarmu(vypnutí, zapnutí, heslo k vypnutí a zapnutí, časy, kdy se alarm automaticky zapne a vypne)
3) Pokaždé, když se alarm aktivuje, objeví se oznámení v aplikaci a chtěl bych se pokusit o to, že by mi přisla SMS vždy, když se tohle stane
4) Alarm by se dal ovádat přes klávesnici a LCD displej(zadám heslo, které si nastavím přes aplikaci a pomocí něj vypnu nebo zapnu alarm)



Hardware co plánuji použít:

1) HY-SRF05 měřič vzdálenosti
2) Bzučák 5V
3) Membránová klávesnice 4x4
4) LCD 1602
5) ESP32 Development Board 2.4GHz Dual-Mode WiFi+Bluetooth Antenna Module



Zdroje a návody, kterými se inspiruji:
(budu postupně doplňovat)
</br>
ESP32 web server - https://www.youtube.com/watch?v=ApGwxX6VVzk</br>
lcd displej - https://navody.arduino-shop.cz/zaciname-s-arduinem/lcd-displej.html</br>
měřič vzdálenosti - https://navody.arduino-shop.cz/navody-k-produktum/ultrazvukovy-meric-vzdalenosti-hy-srf05.html</br>
web server - https://randomnerdtutorials.com/esp32-web-server-spiffs-spi-flash-file-system/</br>



AsyncWebServerRequest <br>
platformio run --uploadfs <br>
sudo chmod -R 777 /dev/ttyUSB0 <br>
esptool.py --chip esp32-s2 --port /dev/ttyUSB0 --baud 961200  write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x10000 .pio/build/esp-wrover-kit/firmware.bin  0x8000 .pio/build/esp-wrover-kit/partitions.bin <br>



