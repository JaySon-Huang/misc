#encoding=utf-8
import random

def exgcd(a, b):
	'''
	对于 s*a + t*b = gcd(a, b)
	返回 (gcd(a,b), s, t)
	'''
	s, t = 0, 1
	s0,s1 = 1,0
	t0,t1 = 0,1
	r = a%b
	q = (a-r)/b
	while r != 0:
		s = s0-q*s1; t = t0-q*t1
		s0 = s1; s1 = s
		t0 = t1; t1 = t
		a=b; b=r; r=a%b; q=(a-r)/b;
	return (b, s, t)

def multiplicative_inver(a, m):
	'''
	对于存在a' 使得a' * a = 1 (mod m)
	返回 a'
	否则 返回None
	'''
	gcd, s, t = exgcd(a, m)
	if gcd != 1:
		return None
	else :
		if s < 0 :
			s += m
		return s

def generate_keys(p, q):
	N = p*q
	euler_N = (p-1) * (q-1)
	while True:
		e = random.randint(1,euler_N)
		if e == 1 : continue
		tmp = exgcd(e, euler_N)
		print('exgcd of %3d && %d : %s'%(e, euler_N, tmp))
		if tmp[0] == 1:
			break
	# e = 7
	d = multiplicative_inver(e, euler_N)
	keys = {
		'public_keys'  : (e, N),
		'private_keys' : (d, N),
	}
	return keys

def power_mod(a, n, m):
	'''
	返回 (a^n) % m
	'''
	ret = 1; b = a;
	while n != 0:
		if n&1 != 0:
			ret = (ret*b)%m
		b = (b*b)%m
		n >>= 1
	return ret

keys = generate_keys(17, 11)
print(keys)
plaintext = 'attack at dawn!'
bytes = [ power_mod(ord(ch), keys['public_keys'][0], keys['public_keys'][1]) for ch in plaintext ]
encrypted = ''.join(map(chr,bytes))
print('encrypted:',encrypted)
bytes = [ power_mod(byte, keys['private_keys'][0], keys['private_keys'][1]) for byte in bytes]
decrypted = ''.join(map(chr,bytes))
print('decrypted:',decrypted)

plaintext = 'abc'
bytes = [ power_mod(ord(ch), keys['public_keys'][0], keys['public_keys'][1]) for ch in plaintext ]
encrypted = ''.join(map(chr,bytes))
print('encrypted:',encrypted)
bytes = [ power_mod(byte, keys['private_keys'][0], keys['private_keys'][1]) for byte in bytes]
decrypted = ''.join(map(chr,bytes))
print('decrypted:',decrypted)

plaintext = 'cba'
bytes = [ power_mod(ord(ch), keys['public_keys'][0], keys['public_keys'][1]) for ch in plaintext ]
encrypted = ''.join(map(chr,bytes))
print('encrypted:',encrypted)
bytes = [ power_mod(byte, keys['private_keys'][0], keys['private_keys'][1]) for byte in bytes]
decrypted = ''.join(map(chr,bytes))
print('decrypted:',decrypted)
