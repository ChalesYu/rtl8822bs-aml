
## rtl8822bs-aml

this repo is mainly for `rtl8822bs` support , but also include some modified vendor driver code for support other realtek chip , test with hardware is always need.

#### Support chip include

USB interface: `rtl8822bu` , `rtl8188eu` , `rtl8723bu` .
SDIO interface: `rtl8822bs` , `rtl8189es` , `rtl8723bs` .

rtl8822bu/rtl8822bs: driver version `v5.13.1-20-gbd7c7eb9d.20210702` at [this branch](https://gitlab.com/ChalesYu/rtl8822bs-aml/tree/test-5.13.1-20-230201).
rtl8188eu/rtl8189es: driver version `v5.13.3-17-gb1925f81a.20210615` at [this branch](https://gitlab.com/ChalesYu/rtl8822bs-aml/tree/rtl8188e_usb_sdio_5.13.3-17-test) , driver version `v5.7.6.1_35670.20191106` at [this branch](https://gitlab.com/ChalesYu/rtl8822bs-aml/tree/rtl8188e_usb_sdio_5.7.6.1-test).
rtl8723bu/rtl8723bs: driver version `v5.8.4_33660.20190516_COEX20181129-7272` at [this branch](https://gitlab.com/ChalesYu/rtl8822bs-aml/tree/rtl8723b_usb_sdio_5.8.4-test) .

The `rtw88` driver can found at [lwfinger repo](https://github.com/lwfinger/rtw88) , it has usb/sdio interface support for rtl8822b/rtl8822c already , and need waiting for upstream merge.



a modified driver version `v5.13.1-20-gbd7c7eb9d.20210702` is available at [test branch](https://github.com/ChalesYu/rtl8822bs-aml/tree/test-5.13.1-20-230201),fork from [88x2bu-20210702](https://github.com/morrownr/88x2bu-20210702).

a way to add support to vendor [88x2bu driver](https://gitlab.com/ChalesYu/rtl8822bs-aml/tree/test-5.13.1-20-230201) , useage :

```
make CONFIG_USB_HCI=n CONFIG_SDIO_HCI=y
modprobe cfg80211
insmod 88x2bs.ko rtw_drv_log_level=4
```

Like the `rtl88x2bu` driver, add some SDIO interface support file can support `rtl8822bs`. The same thing happened on `rtl8188eus` driver, add SDIO support file can support `rtl8189es` , but the quality is still need test, modify driver at branch `rtl8188e_usb_sdio_5.13.3-17-test` and `rtl8188e_usb_sdio_5.7.6.1-test`.

A `rtl8723bu` modify driver can also support `rtl8723bs`, test branch is `rtl8723b_usb_sdio_5.8.4-test` .

### About firmware load fail issue

Pervious this issue happen because compiler will ignore firmware array in `.c` files for some reason,
 so of course firmware will load failed. But now this issue solved. On branch `test-5.13.1-20-220617` , driver will load firmware from `/lib/firmware/rtw88/rtw8822b_fw.bin`.

a way to corvent vendor driver source firmware to `rtw8822b_fw.bin` can be found at [dumpVendorFirmware/readme.txt](https://github.com/ChalesYu/rtl8822bs-aml/tree/master/dumpVendorFirmware/).

Some user report this issue still happen. It looks like hardware issue, check:

```
Power quality for wifi hardware is not good. (e.g Need Vdd 3.3V but only get 2.7~2.9V , Similar problem happen on some board with XR819 wifi)
Data transfer have mistake in sdio interface between wifi and host. (slow down mmc controller clk speed may help, or ask your platform vendor how to avoid data transfer error.)
```

Please also follow Hardware_Reference_design to check if your board missing something.

Change to another hardware platform or use another SBC to test again is also a better choice.

### Possible exist Issues

```
High CPU load (idle 99%)
Not fully support iw ioctl command (iw dev del)
firmware load failed (suggest try test branch)
```

NOTE: Other issue are not fully tested.

SDIO id:

```
024c:b822
```

### How to use

install linux-header deb, set your platform in Makefile

```
git clone https://github.com/ChalesYu/rtl8822bs-aml.git
cd rtl8822bs-aml/
make -j2
sudo make install
sudo modprobe 8822bs
```

need AP mode ?   see [here](https://github.com/ChalesYu/rtl8822bs-aml/tree/master/getAP)

need bluetooth ? see [here](https://github.com/ChalesYu/rtl8822bs-aml/tree/master/bluetooth)
