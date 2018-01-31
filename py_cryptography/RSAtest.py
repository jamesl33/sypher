import math

def RSA_encode(message,p,q):

    asc2_m = [ord(c) for c in message]
    n = p * q
    z = (p-1)*(q-1)
    e_candidates = range(2,z-1)
    e = next(i for i in e_candidates if math.gcd(i, z) == 1)

    found = False
    d_candidate = 1
    while not found:
        d_candidate += z
        if d_candidate % e == 0:
            found = True
            d = d_candidate / e

    print (d)
        
    code = [((i**e) % n) for i in asc2_m]
    return code

def RSA_decode(code,n):
    d = int(input('Enter the private key for decoding: '))
    decode_asc2 = [((i**d) % n) for i in code]
    message_list = [chr(c) for c in decode_asc2]
    message = ''.join(message_list)
    return message
    
    

print( RSA_decode((RSA_encode("incredible i can encode things",17,11)),187))

#n = pq
#z = (p-1)(q-1)
#e = 1 ~ z with no common factor to z
#d = ed == 1(mod z)
#c == m^e (mod n)
#m == c^d (mod n)
