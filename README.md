### 88x2bu

### Linux Driver for the RealTek RTL8812BU and RTL8822BU Chipsets.

- Driver Version: 5.8.7.2.36899.20200819

### Supported Features:

- IEEE 802.11 b/g/n/ac WiFi compliant
- 802.1x, WEP, WPA TKIP and WPA2 AES/Mixed mode for PSK and TLS (Radius)
- WPA3-SAE Personal
- WPS - PIN and PBC Methods
- IEEE 802.11b/g/n/ac Client mode
- Wireless security for WEP, WPA TKIP and WPA2 AES PSK
- Site survey scan and manual connect
- WPA/WPA2 TLS client
- Power saving mode
- Monitor mode
- WiFi-Direct

### Supported Kernels:

- Kernels: 2.6.24 ~ 5.6

### Tested Linux Distributions:

- Ubuntu 20.04
- Ubuntu 18.04
- Linux Mint 20
- Linux Mint 19.3

### Tested Hardware:

- EDUP WiFi Adapter 1300Mbps USB 3.0 High Gain Wireless Adapter:
  https://www.amazon.com/gp/product/B07Q56K68T

### DKMS
This driver can be installed using DKMS. DKMS is a system utility which will automatically recompile and install a kernel module when a new kernel is installed. To make use of DKMS, install the `dkms` package, which on Debian (based) systems such as Ubuntu and Linux Mint is done like this:
```
$ sudo apt-get install dkms
```

### Installation of the Driver
Open a terminal in the directory with the source code and execute the following command:
```
$ sudo ./dkms-install.sh
```
```
$ sudo reboot
```
### Removal of the Driver
Open a terminal in the directory with the source code and execute the following command:
```
$ sudo ./dkms-remove.sh
```
```
$ sudo reboot
```
### Secure Mode:

- If your system is in Secure Mode and all is well, DKMS will handle signing the driver. 

### Entering Monitor Mode with 'iw' and 'ip':
Start by making sure the system recognizes the Wi-Fi interface:
```
$ sudo iw dev
```

The output shows the Wi-Fi interface name and the current mode among other things. The interface name will be something like `wlx00c0cafre8ba` and is required for the below commands. I will use `wlan0` as the interface name but you need to substitute your interface name.

Take the interface down:
```
$ sudo ip link set wlan0 down
```

Set monitor mode:
```
$ sudo iw wlan0 set monitor control
```

Bring the interface up:
```
$ sudo ip link set wlan0 up
```

Verify the mode has changed:
```
$ sudo iw dev
```

### Reverting to Managed Mode with 'iw' and 'ip':

Take the interface down:
```
$ sudo ip link set wlan0 down
```

Set managed mode:
```
$ sudo iw wlan0 set type managed
```

Bring the interface up:
```
$ sudo ip link set wlan0 up
```

Verify the mode has changed:
```
$ sudo iw dev
```
