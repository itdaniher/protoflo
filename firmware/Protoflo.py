import usb.core

class Protoflo(object):
	def __init__(self):
		self.dev = usb.core.find(idVendor=0x9999, idProduct=0xffff)
		if not self.dev:
			raise IOError("device not found")
		self.dev.set_configuration()

	def setLED0(self, channel, value):
		self.dev.ctrl_transfer(0x40|0x80, 0x70, value, channel, 0)	

	def getAccel(self):
		data = self.dev.ctrl_transfer(0x40|0x80, 0xAC, 0, 0, 3)
		return data

if __name__ == "__main__":
	protoflo = Protoflo()
	while True:
		print protoflo.getAccel()
