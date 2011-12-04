import usb.core

class Protoflo(object):
	unTwos = lambda self, n: n - (1<<12) if n>2048 else n   

	def __init__(self):
		self.init()

	def init(self):
		self.dev = usb.core.find(idVendor=0x9999, idProduct=0xffff)
		if not self.dev:
			raise IOError("device not found")
			
		self.dev.set_configuration()

	def setLED0(self, channel, value):
		self.dev.ctrl_transfer(0x40|0x80, 0x70, value, channel, 0)	

	def getAccel(self):
		data = self.dev.ctrl_transfer(0x40|0x80, 0xAC, 0, 0, 6)
		return [data[0]<<8|data[1], data[2]<<8|data[3], data[4]<<8|data[5]]
