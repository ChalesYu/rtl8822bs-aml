### 88x2bu

### Linux Driver for the RealTek RTL8812BU and RTL8822BU Chipsets.

- Driver Version: 5.8.7.2.36899.20200819
- Note: Improvements are made on an ongoing basis.

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
- Monitor mode (needs testing)
- Soft AP mode (needs testing)
- WiFi-Direct

### Supported Kernels:

- Kernels: 2.6.24 ~ 5.6
- Update: Tested on kernel 5.8.

### Tested Linux Distributions:

- Ubuntu 20.10 (daily - 09-05-20 - kernel 5.8)
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

### Removal of the Driver
Open a terminal in the directory with the source code and execute the following command:
```
$ sudo ./dkms-remove.sh
```

### Secure Mode:

- If your system is in Secure Mode and all is well, DKMS will handle signing the driver. 

### Bugs:

- None noted at this time.
