import usb.core
import array

class LED(object):
	def __init__(self):
		self.R = 0
		self.G = 0
		self.B = 0
		self.color = array.array('B', [self.R, self.B, self.G])

	def toString(self):
		return self.color.toString()
	
class Protoflo(object):
	def __init__(self):
		self.dev = usb.core.find(idVendor=0x9999, idProduct=0xffff)
		if not self.dev:
			raise IOError("device not found")
		self.dev.set_configuration()
		self.LED0 = LED()


	def setLED0(self):
		self.dev.ctrl_transfer(0x40|0x80, 0x70, self.LED0.R, 0, 0)	
		self.dev.ctrl_transfer(0x40|0x80, 0x70, self.LED0.B, 1, 0)	
		self.dev.ctrl_transfer(0x40|0x80, 0x70, self.LED0.G, 2, 0)	

	def getAccel(self):
		data = self.dev.ctrl_transfer(0x40|0x80, 0xAC, 0, 0, 3)
		return data

if __name__ == "__main__":
	protoflo = Protoflo()
	while True:
		print protoflo.getAccel()
