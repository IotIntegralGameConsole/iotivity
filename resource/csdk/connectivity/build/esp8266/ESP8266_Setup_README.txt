STEPS to setup ESP8266 on linux machine for building CA code:

1) Going to the <PATH_TO_ESP8266_INSTALL_DIR> (any directory you want)

2) Clone the esp8266/Arduino repository into hardware/esp8266com/esp8266 directory
	$ cd hardware
	$ mkdir esp8266com
	$ cd esp8266com
	$ git clone https://github.com/esp8266/Arduino.git esp8266

3) Download binary tools (you need Python 2.7)
	$ cd esp8266/tools
	$ python get.py

4) ONE TIME ONLY - ADDITIONAL LIBRARIES (Time)
	ESP8266 builds are dependent on latest Time library. Download it from here:
		I. http://playground.arduino.cc/code/time
	Place this library in the "<PATH_TO_ESP8266_INSTALL_DIR>/hardware/esp8266com/esp8266/libraries/Time" folder where other libraries are present.
		[P.S - The Time library files path should be as follows "<PATH_TO_ESP8266_INSTALL_DIR>/hardware/esp8266com/esp8266/libraries/Time/{***files***}"]
	Some files in this library has windows style line endings, for patch to run properly, please execute following command:
		cd <PATH_TO_ESP8266_INSTALL_DIR/hardware/esp8266com/esp8266/>
		find ./libraries/Time/ -type f -exec dos2unix {} \;

5) ONE TIME ONLY - PATCH
	Complete patch has been provided at : "connectivity\lib\esp8266\esp8266_ld.patch"
		Copy this patch file to "<PATH_TO_ESP8266_INSTALL_DIR>"
        To apply patch give the command :
		patch -p1 < esp8266.patch
	To undo the patch give the command :
		patch -R -p1 < esp8266.patch

6) Building IoTivity for ESP8266
	    $ scons TARGET_OS=esp8266 ESP8266_HOME=<PATH_TO_ESP8266_INSTALL_DIR>
