import usb.core
import array

class LED(object):
	def __init__(self):
		self.color = [0, 0, 0]

	@property
	def R(self):
		return self.color[0]
	@R.setter
	def R(self, value):
		self.color[0] = value
	@property
	def B(self):
		return self.color[1]
	@B.setter
	def B(self, value):
		self.color[1] = value
	@property
	def G(self):
		return self.color[2]
	@G.setter
	def G(self, value):
		self.color[2] = value
	
class Protoflo(object):
	def __init__(self):
		self.dev = usb.core.find(idVendor=0x9999, idProduct=0xffff)
		if not self.dev:
			raise IOError("device not found")
		self.dev.set_configuration()
		self.LED0 = LED()
		self.otherLEDs = 6*[0]


	def setLED0(self):
		self.dev.ctrl_transfer(0x40|0x80, 0x70, self.LED0.R, 0, 0)	
		self.dev.ctrl_transfer(0x40|0x80, 0x70, self.LED0.B, 1, 0)	
		self.dev.ctrl_transfer(0x40|0x80, 0x70, self.LED0.G, 2, 0)	

	def setLEDs(self):
		i = 0
		for LEDVal in self.otherLEDs:
			self.dev.ctrl_transfer(0x40|0x80, 0x73, LEDVal, i, 0)  
			i += 1

	def getAccel(self):
		data = self.dev.ctrl_transfer(0x40|0x80, 0xAC, 0, 0, 3)
		return data

if __name__ == "__main__":
	protoflo = Protoflo()
	while True:
		print protoflo.getAccel()
