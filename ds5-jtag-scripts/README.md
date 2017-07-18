DS-5 can connect the Hikey960 board if you have welded the JTAG connecter on the board to match DSTREAM connector.

1 Please unzip the "Hikey960.zip" and add the Hikey960 CDB to DS-5 CDB, you can refer the link http://infocenter.arm.com/help/topic/com.arm.doc.100953_0527_00_en/deb1359981692428.html

2 Before connect the board with DS-5 please disable the CPU idle if it is enabled, or else the connection might fail.
  2.1 "adb devices" to check whethter the board is bring up sucessful and list the device
  2.2 adb root
  2.3 adb remout
  2.4 adb push close_idle.sh /system/
  2.5 adb shell chmod +x /system/close_idle.sh
  2.6 adb shell ./system/close_idle.sh

3 Connect the board successfully now

 
