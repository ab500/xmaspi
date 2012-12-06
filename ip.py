#!/usr/bin/python

# outputs all the IP addresses in binary on the lights
import binary
import driver
import rgb_strand

import socket
import fcntl
import struct
SIOCGIFADDR = 0x8915

d = driver.Driver()
bs = binary.BinaryStream()



def get_interface_ip(ifname):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        saddr = fcntl.ioctl(s.fileno(), SIOCGIFADDR, struct.pack('256s', ifname[:15]))
        return saddr[20:24]

out_str = ''
try:
    wlan_ip = get_interface_ip('wlan1')
    print socket.inet_aton(wlan_ip)
    out_str = wlan_ip + '\x00'
except:
    print 'uh oh' 

try:
    eth0_ip = get_interface_ip('eth0')
    print socket.inet_aton(eth0_ip)
    out_str += eth0_ip + '\x00' 
except:
    print 'meh'


bs.update_text(out_str)
bs.bit_offset = -3

# set beginning to green
strand = rgb_strand.RGBStrand(100)
strand.set_bulb_color(0, 70, 0, 13, 0)
strand.set_bulb_color(1, 150, 0, 13, 0)
strand.set_bulb_color(2, 0, 0, 0, 0)


# set separator byte to all off
for i in range(39, 39+8):
    strand.set_bulb_color(i, 0, 0, 0, 0)






