# Ping

## 1������

[ping](https://baike.baidu.com/item/ping/6235) ��һ�����繤�ߣ������������ݰ��ܷ�ͨ�� IP Э�鵽���ض�������������������Ķ�ʧ���ݰ��ʣ������ʣ������ݰ�����ʱ�䣨����ʱ�ӣ�Round-trip delay time����

## 2��ʹ��

ping ֧�ַ��� `IP ��ַ` �� `����` ��ʹ�� Finsh/MSH ������в��ԣ�����ʹ��Ч�����£�

### 2.1 Ping ����

```
msh />ping rt-thread.org
60 bytes from 116.62.244.242 icmp_seq=0 ttl=49 time=11 ticks
60 bytes from 116.62.244.242 icmp_seq=1 ttl=49 time=10 ticks
60 bytes from 116.62.244.242 icmp_seq=2 ttl=49 time=12 ticks
60 bytes from 116.62.244.242 icmp_seq=3 ttl=49 time=10 ticks
msh />
```

### 2.2 Ping IP

```
msh />ping 192.168.10.12
60 bytes from 192.168.10.12 icmp_seq=0 ttl=64 time=5 ticks
60 bytes from 192.168.10.12 icmp_seq=1 ttl=64 time=1 ticks
60 bytes from 192.168.10.12 icmp_seq=2 ttl=64 time=2 ticks
60 bytes from 192.168.10.12 icmp_seq=3 ttl=64 time=3 ticks
msh />

```
