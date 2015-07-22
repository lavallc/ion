This is a sample, experimental IronPython utility provided to peform device firmware 
upgrade on an nrf51822 device over Bluetooth Low Energy. Following setup is required for 
this utility to behave as expected:

a. IronPython v2.7.3 (or later)                   http://ironpython.net/
b. .Net Framework 4.5 (or later)                  http://www.microsoft.com/en-us/download/details.aspx?id=30653
c. Master Control Panel and Master Emulator API   http://www.nordicsemi.com - log into mypage select kit and downloads
d. PCA10000 with Master Emulator firmware
e. IntelHex v1.5 (or later)                       https://pypi.python.org/pypi/IntelHex


The python script can take following command line arguments:

  --help, -h                     Show help message and exit.
  --file FILE, -f FILE           Filename of Hex file. This is mandatory to provide.
  --name NAME, -n NAME           Advertising name of nrf51822 device, this is mandatory to provide.
  --address ADDRESS, -a ADDRESS  Advertising address of nrf51822 device, this is mandatory to provide.



If it is required to upload a firmware image 'ble_app_hrs.hex' located at 
C:\NewFirmware, the nRF Device is advertising Device Name 'DfuTarg' and the nRD Device 
address is CD:B5:3D:6D:16:B6, run the following in windows command window:

    ipy main.py -f C:\NewFirmware\ble_app_hrs.hex -n DfuTarg -a CDB53D6D16B6

Note 1: Please ensure that the master dongle is plugged in when running the script.
Note 2: If ipy is not a recognized command, add IronPython to the System PATH environment variable.
Note 3: The target device address can be resolved using Master Control Panel.

The application provides comprehensive messages to provide the progress of the firmware 
upgrade or the reason for failure in case the procedure did not succeed. However, the 
application is experimental and is has only been tested on Windows 7 systems.

