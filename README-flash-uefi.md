Setup Console
----------------

   * Install ser2net. Use telnet as the console since UEFI will output window
     that fails to display in minicom.
     <br>`$sudo apt-get install ser2net`</br>

   * Configure ser2net.
     <br>`$sudo vi /etc/ser2net.conf`</br>
     <br>Append one line for serial-over-USB in below.</br>
     <br>_#ser2net.conf_</br>
     <br>`2004:telnet:0:/dev/ttyUSB0:115200 8DATABITS NONE 1STOPBIT banner`</br>

   * Open the console.
     <br>`$telnet localhost 2004`</br>
     <br>And you could open the console remotely, too.</br>

     OR USE picocom

   * Install picocom. Use picocom as the console since UEFI will output window
     that fails to display in minicom.
     <br>`$sudo apt-get install picocom`</br>
     
   * Open the console.
     <br>`$picocom /dev/ttyUSB0 -b 115200 -f x`</br>

1. Connect the HiKey960 and prepare for flash assuming the host machine already has adb and fastboot installed
a. 
On the Hikey960 

  ...Set switch 1 and 2 to on and 3 to off 

  ...Connect the uart to the host system and make sure it shows up as ttyUSB0 using dmesg command 

  ...Power on the HiKey960 

  ...Connect the usb-c otg to host system and make sure that shows up as ttyUSB1

2. ``` sudo ./hikey960-efi-prebuilt.sh -h ```

3. Set switch 1 to on and 2 & 3 to off

4. Reboot and proceed to install OS.
