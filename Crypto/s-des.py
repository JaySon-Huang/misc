#encoding=utf-8

POSITION_1 = 0x01
POSITION_2 = 0x02
POSITION_3 = 0x04
POSITION_4 = 0x08
POSITION_5 = 0x10
POSITION_6 = 0x20
POSITION_7 = 0x40
POSITION_8 = 0x80
POSITION_9 = 0x100
POSITION_10 = 0x200

S_BOX = {
	0 : [[1,0,3,2],
		 [3,2,1,0],
		 [0,2,1,3],
		 [3,1,3,2]],
	1 : [[0,1,2,3],
		 [2,0,1,3],
		 [3,0,1,0],
		 [2,1,0,3]],
}
class Cipher:


	def IP(self,origin):
		'''
		初始置换IP：2 6 3 1  4 8 5 7
		'''
		ret = 0
		ret |= (origin&POSITION_7) << 1
		ret |= (origin&POSITION_3) << 4
		ret |= (origin&POSITION_6)
		ret |= (origin&POSITION_8) >> 3
		ret |= (origin&POSITION_5) >> 1
		ret |= (origin&POSITION_1) << 2
		ret |= (origin&POSITION_4) >> 2
		ret |= (origin&POSITION_2) >> 1
		return ret

	def test_IP(self):
		'''
		IP函数测试
		1:00000001 -> 00000100
		2:00000010 -> 00000001
		3:00000100 -> 01000000
		4:00001000 -> 00000010
		5:00010000 -> 00001000
		6:00100000 -> 00100000
		7:01000000 -> 10000000
		8:10000000 -> 00010000
		'''
		for i in xrange(8):
			print '{0:d}:{1:08b} -> {2:08b}'.format( i+1, 1<<i, IP(1<<i) )
	# test_IP()

	def rIP(self,origin):
		ret = 0
		ret |= (origin&POSITION_5) << 3
		ret |= (origin&POSITION_8) >> 1
		ret |= (origin&POSITION_6) 
		ret |= (origin&POSITION_4) << 1
		ret |= (origin&POSITION_2) << 2
		ret |= (origin&POSITION_7) >> 4
		ret |= (origin&POSITION_1) << 1
		ret |= (origin&POSITION_3) >> 2
		return ret

	def P10(self,origin):
		'''
		进行P10置换：3 5 2 7 4  10 1 9 8 6
		'''
		ret = 0
		ret |= (origin&POSITION_8) << 2
		ret |= (origin&POSITION_6) << 3
		ret |= (origin&POSITION_9) >> 1
		ret |= (origin&POSITION_4) << 3
		ret |= (origin&POSITION_7) >> 1
		ret |= (origin&POSITION_1) << 4
		ret |= (origin&POSITION_10)>> 6
		ret |= (origin&POSITION_2) << 1
		ret |= (origin&POSITION_3) >> 1
		ret |= (origin&POSITION_5) >> 4
		return ret

	def test_P10(self):
		'''
		 1:0000000001 -> 0000010000
		 2:0000000010 -> 0000000100
		 3:0000000100 -> 0000000010
		 4:0000001000 -> 0001000000
		 5:0000010000 -> 0000000001
		 6:0000100000 -> 0100000000
		 7:0001000000 -> 0000100000
		 8:0010000000 -> 1000000000
		 9:0100000000 -> 0010000000
		10:1000000000 -> 0000001000
		'''
		for i in xrange(10):
			print '{0:2d}:{1:010b} -> {2:010b}'.format( i+1, 1<<i, P10(1<<i) )
	# test_P10()

	def P8(self,origin):
		'''
		进行P8置换：6 3 7 4  8 5 10 9
		'''
		ret = 0
		ret |= (origin&POSITION_5) << 3
		ret |= (origin&POSITION_8) >> 1
		ret |= (origin&POSITION_4) << 2
		ret |= (origin&POSITION_7) >> 2
		ret |= (origin&POSITION_3) << 1
		ret |= (origin&POSITION_6) >> 3
		ret |= (origin&POSITION_1) << 1
		ret |= (origin&POSITION_2) >> 1
		return ret

	def test_P8(self,):
		'''
		P8函数测试
		 1:0000000001 -> 00000010
		 2:0000000010 -> 00000001
		 3:0000000100 -> 00001000
		 4:0000001000 -> 00100000
		 5:0000010000 -> 10000000
		 6:0000100000 -> 00000100
		 7:0001000000 -> 00010000
		 8:0010000000 -> 01000000
		 9:0100000000 -> 00000000
		10:1000000000 -> 00000000
		'''
		for i in xrange(10):
			print '{0:2d}:{1:010b} -> {2:08b}'.format( i+1, 1<<i, P8(1<<i) )
	# test_P8()

	def generate_key(self,key):
		'''
		密钥生成函数
		'''
		# 进行P10置换
		key = self.P10(key)
		# 分为左右两部分，每部分5bit
		lpart,rpart = key>>5 ,key&0x1f
		# print '{0:010b}->{1:05b},{2:05b}'.format(key,lpart,rpart)
		# 循环左移一位
		lpart = ((lpart<<1) | ((lpart&POSITION_5) >> 4))&(0x1f)
		rpart = ((rpart<<1) | ((rpart&POSITION_5) >> 4))&(0x1f)
		# print '{0:05b},{1:05b}'.format(lpart,rpart)
		# 两部分合起来，进行P8置换：
		k1 = self.P8(lpart<<5 | rpart)
		# 循环左移两位
		lpart = ((lpart<<2) | ((lpart&(POSITION_5|POSITION_4)) >> 3))&(0x1f)
		rpart = ((rpart<<2) | ((rpart&(POSITION_5|POSITION_4)) >> 3))&(0x1f)
		# print '{0:05b},{1:05b}'.format(lpart,rpart)
		k2 = self.P8(lpart<<5 | rpart)
		return (k1,k2)

	def test_generate_key(self):
		'''
		测试密钥生成函数
		Key:1010000010
		k1:10100100, k2:01000011
		'''
		key = int('1010000010',2)
		k1, k2 = generate_key(key)
		print 'Key:{0:08b}\nk1:{1:08b}, k2:{2:08b}'.format(key,k1,k2)
	# test_generate_key()

	def E_P(self,origin):
		'''E/P运算'''
		ret = 0
		ret |= (origin&POSITION_1) << 7
		ret |= (origin&POSITION_4) << 3
		ret |= (origin&POSITION_3) << 3
		ret |= (origin&POSITION_2) << 3
		ret |= (origin&POSITION_3) << 1
		ret |= (origin&POSITION_2) << 1
		ret |= (origin&POSITION_1) << 1
		ret |= (origin&POSITION_4) >> 3
		return ret

	def S(self,origin,box):
		row = ((origin&POSITION_4)>>2) | (origin&POSITION_1)
		col = (origin&(POSITION_3|POSITION_2)) >> 1
		return S_BOX[box][row][col]

	def test_S(self):
		print S(int('0100',2),0)
		print S(int('0101',2),0)
		print S(int('1010',2),1)
	# test_S()

	def P4(self,origin):
		'''P4置换'''
		ret = 0
		ret |= (origin&POSITION_3) << 1
		ret |= (origin&POSITION_1) << 2
		ret |= (origin&POSITION_2)
		ret |= (origin&POSITION_4) >> 3
		return ret

	def F(self,right, k):
		tmp = self.E_P(right)
		# print ' E/P :{0:08b}'.format(tmp)
		tmp ^= k
		# print 'xor k:{0:08b}'.format(tmp)
		# 分为左右两部分，每部分4bit
		lpart = (tmp & 0xf0)>>4
		rpart = tmp & 0x0f
		tmp = (self.S(lpart,0)<<2) | self.S(rpart,1)
		tmp = self.P4(tmp)
		return tmp

	def test_F(self):
		print '{0:04b}'.format(F(int('1001',2),int('10011001',2)))
	# test_F()

	def f(self,lpart, rpart, k):
		lpart ^= self.F(rpart, k)
		return (lpart, rpart)

	def test_f(self):
		f(int('1011',2),int('1101',2),int('10100100',2))
	# test_f()

	def __init__(self, key):
		self.plaintext = plaintext
		self.key = key

	def block_encrypt(self, block):
		'''
		输入：8位明文组和10位密钥
		输出：8位密文组
		'''
		# print 'Encrypting...'
		# 根据10bitkey生成k1,k2
		k1,k2 = self.generate_key(self.key)
		# print ' ori :{0:04b},{1:04b}'.format((block&0xf0) >> 4, block&0x0f)

		# 对8位明文进行IP置换
		block = self.IP(block)
		lpart = (block&0xf0) >> 4
		rpart = block&0x0f
		# print '  IP :{0:04b},{1:04b}'.format(lpart, rpart)

		# 与k1进行f
		lpart, rpart = self.f(lpart, rpart ,k1)
		# print 'f(k1):{0:04b},{1:04b}'.format(lpart, rpart)
		# 左右块互换后,与k2进行f
		lpart, rpart = self.f(rpart, lpart ,k2)
		# print 'f(k2):{0:04b},{1:04b}'.format(lpart, rpart)

		# 进行IP'操作
		block = self.rIP((lpart<<4)|rpart)
		# print ' rIP :{0:04b},{1:04b}'.format((block&0xf0) >> 4, block&0x0f)

		# 返回密文
		return block

	def encrypt(self, plaintext):
		'''
		输入：plaintext:待加密的字符串
		输出：S-DES加密后的字符串
		'''
		# 对每一个8位组(一字节)转换为整型，并做加密处理
		# bytes = [self.block_encrypt( ord(ch) ) for ch in plaintext]
		# print bytes
		# 与上面的一样，改用函数式的map
		bytes = map(lambda x: self.block_encrypt(ord(x)) , plaintext)
		# print bytes
		# 连接成字符串返回
		return ''.join( map(chr, bytes) )
		
	def block_decrypt(self, block):
		'''
		输入：8位密文组和10位密钥
		输出：8位明文组
		'''
		# print 'Decrypting...'
		# 根据10bitkey生成k1,k2
		k1,k2 = self.generate_key(self.key)
		# print ' ori :{0:04b},{1:04b}'.format((block&0xf0) >> 4, block&0x0f)

		# 对8位密文进行IP置换
		block = self.IP(block)
		lpart = (block&0xf0) >> 4
		rpart = block&0x0f
		# print '  IP :{0:04b},{1:04b}'.format(lpart, rpart)

		# 与k2进行f
		lpart, rpart = self.f(lpart, rpart, k2)
		# print 'f(k2):{0:04b},{1:04b}'.format(lpart, rpart)

		# 左右块互换后,与k1进行f
		lpart, rpart = self.f(rpart, lpart, k1)
		# print 'f(k1):{0:04b},{1:04b}'.format(lpart, rpart)

		# 进行IP'操作
		block = self.rIP((lpart<<4)|rpart)
		# print ' rIP :{0:04b},{1:04b}'.format((block&0xf0) >> 4, block&0x0f) 

		# 返回密文
		return block

	def decrypt(self, ciphertext):
		'''
		输入：ciphertext:经过S-DES加密的字符串
		输出：解密的字符串
		'''
		bytes = map(lambda x: self.block_decrypt(ord(x)), ciphertext)
		return ''.join( map(chr, bytes) )

if __name__ == '__main__':
	# plaintext = '\x16'
	# key = int('0111111101',2)

	plaintext = 'NOTICE:attack at dawn!'
	key = int('1010001100',2)
	cipher = Cipher(key)
	print('plaintext  :',plaintext)
	# print 'plaintext : {0:08b}'.format(ord(plaintext))
	print('    key    :','{0:010b}'.format(key))

	ciphertext = cipher.encrypt(plaintext)
	print('ciphertext :',ciphertext)
	# print plaintext,'->',ciphertext
	# print 'ciphertext: {0:08b}'.format(ord(ciphertext))

	decrypted = cipher.decrypt( ciphertext )
	print('decrypted  :',decrypted)
	# print ciphertext,'->',decrypted
	# print 'decrypted : {0:08b}'.format(ord(decrypted))
	print '===='

	plaintext = 'a'
	key = int('1010001100',2)
	cipher = Cipher(key)
	print('plaintext  :',plaintext)
	print('    key    :','{0:010b}'.format(key))
	ciphertext = cipher.encrypt(plaintext)
	print('ciphertext :',ciphertext)
	decrypted = cipher.decrypt( ciphertext )
	print('decrypted  :',decrypted)
	print '===='

	plaintext = 'b'
	key = int('1010001100',2)
	cipher = Cipher(key)
	print('plaintext  :',plaintext)
	print('    key    :','{0:010b}'.format(key))
	ciphertext = cipher.encrypt(plaintext)
	print('ciphertext :',ciphertext)
	decrypted = cipher.decrypt( ciphertext )
	print('decrypted  :',decrypted)
	print '===='

	plaintext = 'a'
	key = int('1010001101',2)
	cipher = Cipher(key)
	print('plaintext  :',plaintext)
	print('    key    :','{0:010b}'.format(key))
	ciphertext = cipher.encrypt(plaintext)
	print('ciphertext :',ciphertext)
	decrypted = cipher.decrypt( ciphertext )
	print('decrypted  :',decrypted)
	print '===='
