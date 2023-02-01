
## rtl8822bs-aml

a modified rtw88 driver from [lwfinger repo](https://github.com/lwfinger/rtw88) but with [other patch](https://github.com/xdarklight/linux/commits/rtw88-test-20220611) is available at [rtw88-sdio-test branch](https://gitlab.com/ChalesYu/rtl8822bs-aml/tree/rtw88-sdio-test).

a modified driver version `v5.13.1-20-gbd7c7eb9d.20210702` is available at [test branch](https://github.com/ChalesYu/rtl8822bs-aml/tree/test-5.13.1-20-230201),fork from [88x2bu-20210702](https://github.com/morrownr/88x2bu-20210702).

a way to add support to [88x2bu driver](https://gitlab.com/ChalesYu/rtl8822bs-aml/tree/test-5.13.1-20-230201) , useage :

```
make CONFIG_USB_HCI=n CONFIG_SDIO_HCI=y
modprobe cfg80211
insmod 88x2bs.ko rtw_drv_log_level=4
```

a seems useful commit for rtw88 can be found at [here](https://github.com/xdarklight/linux/commit/80154847ef5ff284624a8abb2e66b690e41fb678)

this repo will deprecated while rtw88 driver support sdio interface.

### About firmware load fail issue

Pervious this issue happen because compiler will ignore firmware array in `.c` files for som reason,
 so of course firmware will load failed. But now this issue solved. On branch `test-5.13.1-20-220617` , driver will load firmware from `/lib/firmware/rtw88/rtw8822b_fw.bin`.

Some user report this issue still happen. It looks like hardware issue, check:

```
Power quality for wifi hardware is not good. (e.g Need Vdd 3.3V but only get 2.7~2.9V , Similar problem happen on some board with XR819 wifi)
Data transfer have mistake in sdio interface between wifi and host. (slow down mmc controller clk speed may help, or ask your platform vendor how to avoid data transfer error.)
```

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
