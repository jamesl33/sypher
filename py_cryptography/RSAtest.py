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
        
    return d

print(RSA_encode("hello",3,11))

#n = pq
#z = (p-1)(q-1)
#e = 1 ~ z with no common factor to z
#d = ed == 1(mod z)
#c == m^e (mod n)
