import usb
import sys

# USB framework PID
dev = usb.core.find(idVendor=0x59e3, idProduct=0xf000)
# R
dev.ctrl_transfer(0x40|0x80, 0x70, int(sys.argv[1]), 0, 0)
# G
dev.ctrl_transfer(0x40|0x80, 0x70, int(sys.argv[2]), 1, 0)
# B
dev.ctrl_transfer(0x40|0x80, 0x70, int(sys.argv[3]), 2, 0)
