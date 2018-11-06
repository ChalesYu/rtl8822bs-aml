rtl8822bs-aml

Reference : tieba.baidu.com/p/5289928584

Update for kernel 4.19 , tested on mgv2000 (`gxlx_p261_1g` with `6222B-SRB`)

### How to use

install linux-header deb

```
git clone https://github.com/ChalesYu/rtl8822bs-aml.git
cd rtl8822bs-aml/
make -j2
sudo make install
sudo modprobe 8822bs
```

need AP mode ?   see [here](https://github.com/ChalesYu/rtl8822bs-aml/tree/master/getAP)

need bluetooth ? see [here](https://github.com/ChalesYu/rtl8822bs-aml/tree/master/bluetooth)
