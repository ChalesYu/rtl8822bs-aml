Forked from github.com/Staars/RTL8822BS_BT

# Note

You should check if your kernel support 3-wire-protocol

```
sudo modprobe hci_uart
dmesg |grep Three-wire
```

it should have output like this:
```
Bluetooth: HCI UART protocol Three-wire (H5) registered
```

If no output : Unfortunately , you can't use this bluetooth

# How to use

install linux-header deb

```
cd bluetooth
make
sudo make install
sudo ./start_bt.sh
```

## RTL8822BS_BT

A modified version of rtk_hciattach and a script to turn on bluetooth on a mgv2000 with rtl8822bs. Probably this is not useful for other boxes.

On the mgv2000 (1/8) BT is connected via ttyAML1 (UART_A) , use GPIOX_17 for BT_EN pin , and uses 3-wire-protocol.

Copy firmware and config to /lib/firmware/rtlbt/ and remove the .bin-endings.

You must disconnect your USB-UART-adapter, if you use it as an external serial console. For me it is enough to unplug it from the USB-port and leave the cables connected to the PCB.
