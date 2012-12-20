import os, sys
pathToBootloader = os.path.join(os.path.dirname(__file__), 'usb', 'bootloader')
sys.path.append(pathToBootloader)
from flash import *

import json, base64

DEVICE_MATCH="com.nonolithlabs.framework"
DEVICE="Nonolith Framework"
HWVERSION="1.0"
MEMSIZE=32767
PAGESIZE=256

def hex_to_dict(fname, version):
	ih = IntelHex(fname)
	
	maxaddr = ih.maxaddr()
	if maxaddr > MEMSIZE:
		raise IOError("Input file size (%s) is too large to fit in flash (%s)"%(maxaddr, MEMSIZE))

	maxaddr = maxaddr + (PAGESIZE - (maxaddr)%PAGESIZE - 1) #round up to nearest page
	data = ih.tobinstr(start=0, end=maxaddr, pad=0xff)

	return {
		'crc': atmel_crc(ih.tobinstr(start=0, end=MEMSIZE, pad=0xff)),
		'size': ih.maxaddr(),
		'data': base64.b64encode(data),
		'device_match': DEVICE_MATCH,
		'device': DEVICE,
		'hwVersion': HWVERSION,
		'fwVersion': version,
	}
	
if __name__=="__main__":
	d = hex_to_dict(sys.argv[1], sys.argv[2])
	out = sys.argv[1].replace('.hex', '.json')
	f=open(sys.argv[1].replace('.hex', '.json'), 'w')
	json.dump(d, f)
	f.close()
	
	
