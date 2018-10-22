rtl8822bs-aml

Forked from github.com/9crk/rockchip_wifi/tree/master/rtl8822bs

Update for kernel 4.18 , tested on mgv2000 (`gxlx_p261_1g` with `6222B-SRB`)

### How to use

install linux-header deb

```
git clone https://github.com/ChalesYu/rtl8822bs-aml.git
cd rtl8822bs-aml/
make -j2
sudo make install
sudo modprobe 8822bs
```

need AP mode ?   see [here](https://github.com/ChalesYu/rtl8822bs-aml/)

need bluetooth ? see [here](https://github.com/ChalesYu/rtl8822bs-aml/)
