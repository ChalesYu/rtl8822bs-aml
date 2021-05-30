
## rtl8822bs-aml

a modified driver version `5.8.7.4_37264.20200922` is available at [test branch](https://github.com/ChalesYu/rtl8822bs-aml/tree/test-5.8.7.4)

a way to add support to [88x2bu driver](https://github.com/ChalesYu/88x2bu/tree/sdio-support) , useage : 

```
make CONFIG_USB_HCI=n CONFIG_SDIO_HCI=y
modprobe cfg80211
insmod 88x2bs.ko rtw_drv_log_level=4
```

a seems useful commit for rtw88 can be found at [here](https://github.com/xdarklight/linux/commit/80154847ef5ff284624a8abb2e66b690e41fb678)

this repo will deprecated while rtw88 driver support sdio interface.

### Possible exist Issues

```
High CPU load (idle 99%)
Not fully support iw ioctl command (iw dev del)
```

NOTE: Other issue are not fully tested.

SDIO id:

```
024c:B822
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
