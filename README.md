
## rtl8822bs-aml


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
