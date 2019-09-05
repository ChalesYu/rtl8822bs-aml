
## rtl8822bs-aml


### Known Issues

```
High CPU load (idle 99%)
Not fully support iw ioctl command (iw dev del)
```

NOTE: Other issue are not fully tested, Not for daily use.

### How to use

install linux-header deb

```
git clone https://github.com/ChalesYu/rtl8822bs-aml.git
cd rtl8822bs-aml/
make -j2
sudo make install
sudo modprobe 88x2bs
```

need AP mode ?   see [here](https://github.com/ChalesYu/rtl8822bs-aml/tree/master/getAP)

need bluetooth ? see [here](https://github.com/ChalesYu/rtl8822bs-aml/tree/master/bluetooth)
