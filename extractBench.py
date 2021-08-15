class Results:
    def __init__(self, filename):
        (self.kg, self.s, self.v) = extractNumber (filename)
        
        self.name = filename.split('.')[0]

    def __str__(self):
        print("")
        print("benchmark results of {}:".format(self.name))
        print("")
        print("key gen:")
        print('    126f robust  {}'.format(self.kg[0]))
        print('    126f simple  {}'.format(self.kg[1]))
        print('    192f robust  {}'.format(self.kg[2]))
        print('    192f simple  {}'.format(self.kg[3]))
        print('    256f robust  {}'.format(self.kg[4]))
        print('    265f simple  {}'.format(self.kg[5]))
        print('    126s robust  {}'.format(self.kg[6]))
        print('    126s simple  {}'.format(self.kg[7]))
        print('    192s robust  {}'.format(self.kg[8]))
        print('    192s simple  {}'.format(self.kg[9]))
        print('    256s robust  {}'.format(self.kg[10]))
        print('    265s simple  {}'.format(self.kg[11]))
        print("signing:")
        print('    126f robust  {}'.format(self.s[0]))
        print('    126f simple  {}'.format(self.s[1]))
        print('    192f robust  {}'.format(self.s[2]))
        print('    192f simple  {}'.format(self.s[3]))
        print('    256f robust  {}'.format(self.s[4]))
        print('    265f simple  {}'.format(self.s[5]))
        print('    126s robust  {}'.format(self.s[6]))
        print('    126s simple  {}'.format(self.s[7]))
        print('    192s robust  {}'.format(self.s[8]))
        print('    192s simple  {}'.format(self.s[9]))
        print('    256s robust  {}'.format(self.s[10]))
        print('    265s simple  {}'.format(self.s[11]))
        print("verifying:")
        print('    126f robust  {}'.format(self.v[0]))
        print('    126f simple  {}'.format(self.v[1]))
        print('    192f robust  {}'.format(self.v[2]))
        print('    192f simple  {}'.format(self.v[3]))
        print('    256f robust  {}'.format(self.v[4]))
        print('    265f simple  {}'.format(self.v[5]))
        print('    126s robust  {}'.format(self.v[6]))
        print('    126s simple  {}'.format(self.v[7]))
        print('    192s robust  {}'.format(self.v[8]))
        print('    192s simple  {}'.format(self.v[9]))
        print('    256s robust  {}'.format(self.v[10]))
        print('    265s simple  {}'.format(self.v[11]))
        return ""

    def adapt_layout(self):
        (self.kg, self.s, self.v) = rearrangeNumber (self.kg, self.s, self.v)

    def compare (self, other):
        print("comparing")
        print('    {}'.format(self.name))
        print("to")
        print('    {}'.format(other.name))
        print('')
        print("key gen:")
        print('    256f simple  {}'.format( round((1 - (other.kg[5]  / self.kg[5])  ) * 100, 1)))
        print('    256s simple  {}'.format( round((1 - (other.kg[11] / self.kg[11]) ) * 100, 1)))
        print('    192f simple  {}'.format( round((1 - (other.kg[3]  / self.kg[3])  ) * 100, 1)))
        print('    192s simple  {}'.format( round((1 - (other.kg[9]  / self.kg[9])  ) * 100, 1)))
        print('    128f simple  {}'.format( round((1 - (other.kg[1]  / self.kg[1])  ) * 100, 1)))
        print('    128s simple  {}'.format( round((1 - (other.kg[7]  / self.kg[7])  ) * 100, 1)))
        print('    256f robust  {}'.format( round((1 - (other.kg[4]  / self.kg[4])  ) * 100, 1)))
        print('    256s robust  {}'.format( round((1 - (other.kg[10] / self.kg[10]) ) * 100, 1)))
        print('    192f robust  {}'.format( round((1 - (other.kg[2]  / self.kg[2])  ) * 100, 1)))
        print('    192s robust  {}'.format( round((1 - (other.kg[8]  / self.kg[8])  ) * 100, 1)))
        print('    128f robust  {}'.format( round((1 - (other.kg[0]  / self.kg[0])  ) * 100, 1)))
        print('    128s robust  {}'.format( round((1 - (other.kg[6]  / self.kg[6])  ) * 100, 1)))
        print('')
        print("signing:")
        print('    256f simple  {}'.format( round((1 - (other.s[5]  / self.s[5])  ) * 100, 1)))
        print('    256s simple  {}'.format( round((1 - (other.s[11] / self.s[11]) ) * 100, 1)))
        print('    192f simple  {}'.format( round((1 - (other.s[3]  / self.s[3])  ) * 100, 1)))
        print('    192s simple  {}'.format( round((1 - (other.s[9]  / self.s[9])  ) * 100, 1)))
        print('    128f simple  {}'.format( round((1 - (other.s[1]  / self.s[1])  ) * 100, 1)))
        print('    128s simple  {}'.format( round((1 - (other.s[7]  / self.s[7])  ) * 100, 1)))
        print('    256f robust  {}'.format( round((1 - (other.s[4]  / self.s[4])  ) * 100, 1)))
        print('    256s robust  {}'.format( round((1 - (other.s[10] / self.s[10]) ) * 100, 1)))
        print('    192f robust  {}'.format( round((1 - (other.s[2]  / self.s[2])  ) * 100, 1)))
        print('    192s robust  {}'.format( round((1 - (other.s[8]  / self.s[8])  ) * 100, 1)))
        print('    128f robust  {}'.format( round((1 - (other.s[0]  / self.s[0])  ) * 100, 1)))
        print('    128s robust  {}'.format( round((1 - (other.s[6]  / self.s[6])  ) * 100, 1)))
        print('')
        print("verifying:")
        print('    256f simple  {}'.format( round((1 - (other.v[5]  / self.v[5])  ) * 100, 1)))
        print('    256s simple  {}'.format( round((1 - (other.v[11] / self.v[11]) ) * 100, 1)))
        print('    192f simple  {}'.format( round((1 - (other.v[3]  / self.v[3])  ) * 100, 1)))
        print('    192s simple  {}'.format( round((1 - (other.v[9]  / self.v[9])  ) * 100, 1)))
        print('    128f simple  {}'.format( round((1 - (other.v[1]  / self.v[1])  ) * 100, 1)))
        print('    128s simple  {}'.format( round((1 - (other.v[7]  / self.v[7])  ) * 100, 1)))
        print('    256f robust  {}'.format( round((1 - (other.v[4]  / self.v[4])  ) * 100, 1)))
        print('    256s robust  {}'.format( round((1 - (other.v[10] / self.v[10]) ) * 100, 1)))
        print('    192f robust  {}'.format( round((1 - (other.v[2]  / self.v[2])  ) * 100, 1)))
        print('    192s robust  {}'.format( round((1 - (other.v[8]  / self.v[8])  ) * 100, 1)))
        print('    128f robust  {}'.format( round((1 - (other.v[0]  / self.v[0])  ) * 100, 1)))
        print('    128s robust  {}'.format( round((1 - (other.v[6]  / self.v[6])  ) * 100, 1)))


def extractNumber (filename):
    kg =[]
    s = []
    v = []

    i = 0
    with open(filename) as f:
        for line in f:
            if i == 4:
                words = line.split()
                #print(words)
                number = words[-2].replace(',', '')
                #print(number)
                kg.append(int(number))
                i = i+1
            elif i == 6:
                words = line.split()
                number = words[-2].replace(',', '')
                #print(number)
                s.append(int(number))
                i = i+1
            elif i == 9:
                words = line.split()
                number = words[-2].replace(',', '')
                #print(number)
                v.append(int(number))
                i = i+1
            elif i == 14:
                i = 0
            else:
                i = i+1
    return (kg, s, v)
#rearrangeNumber(kg, s, v)


def addToMatrix(matrix, simple, robust, offset):
    matrix[offset] = simple
    matrix[offset+3] = robust
    return matrix

def rearrangeNumber (kg, s, v):

    kg_simple = []
    kg_robust = []
    s_simple = []
    s_robust = []
    v_simple = []
    v_robust = []
    i = 0
    for key in kg:
        if i % 2 == 0:
            kg_robust.append(key)
        else:
            kg_simple.append(key)
        i = i+1

    i = 0
    for sig in s:
        if i % 2 == 0:
            s_robust.append(sig)
        else:
            s_simple.append(sig)
        i = i+1
    i = 0

    for ver in v:
        if i % 2 == 0:
            v_robust.append(ver)
        else:
            v_simple.append(ver)
        i = i+1

    kg_simple_correct = [kg_simple [2], kg_simple[5], kg_simple[1], kg_simple[4], kg_simple[0], kg_simple[3]]
    kg_robust_correct = [kg_robust [2], kg_robust[5], kg_robust[1], kg_robust[4], kg_robust[0], kg_robust[3]]
    s_simple_correct  = [s_simple [2],  s_simple[5],  s_simple[1],  s_simple[4],  s_simple[0],  s_simple[3]]
    s_robust_correct  = [s_robust [2],  s_robust[5],  s_robust[1],  s_robust[4],  s_robust[0],  s_robust[3]]
    v_simple_correct  = [v_simple [2],  v_simple[5],  v_simple[1],  v_simple[4],  v_simple[0],  v_simple[3]]
    v_robust_correct  = [v_robust [2],  v_robust[5],  v_robust[1],  v_robust[4],  v_robust[0],  v_robust[3]]

    return (kg_simple_correct,kg_robust_correct,s_simple_correct,s_robust_correct,v_simple_correct,v_robust_correct)



def main():

    ref = Results('ref.txt')
    print(ref)
    refx4 = Results('refx4.txt')
    print(refx4)
    neon = Results('transpose2.txt')
    print(neon)
    ref.compare(refx4)
    ref.compare(neon)
    refx4.compare(neon)


    kg_matrix = [None] * 6
    s_matrix = [None] * 6
    v_matrix = [None] * 6


    #(kg, s, v) = extractNumber('ref.txt')
    #(kgs_c, kgr_c, ss_c, sr_c, vs_c, vr_c) = rearrangeNumber(kg, s, v)
    #kg_matrix = addToMatrix(kg_matrix, kgs_c, kgr_c, 0)
    #s_matrix = addToMatrix(s_matrix, ss_c, sr_c, 0)
    #v_matrix = addToMatrix(v_matrix, vs_c, vr_c, 0)
#
    #(kg, s, v) = extractNumber('refx4.txt')
    #(kgs_c, kgr_c, ss_c, sr_c, vs_c, vr_c) = rearrangeNumber(kg, s, v)
    #kg_matrix = addToMatrix(kg_matrix, kgs_c, kgr_c, 1)
    #s_matrix = addToMatrix(s_matrix, ss_c, sr_c, 1)
    #v_matrix = addToMatrix(v_matrix, vs_c, vr_c, 1)
#
    #(kg, s, v) = extractNumber('transpose2.txt')
    #(kgs_c, kgr_c, ss_c, sr_c, vs_c, vr_c) = rearrangeNumber(kg, s, v)
    #kg_matrix = addToMatrix(kg_matrix, kgs_c, kgr_c, 2)
    #s_matrix = addToMatrix(s_matrix, ss_c, sr_c, 2)
    #v_matrix = addToMatrix(v_matrix, vs_c, vr_c, 2)
#
    #for m in kg_matrix:
    #    print(m)
    #for m in s_matrix:
    #    print(m)
    #for m in v_matrix:
    #    print(m)
#
if __name__ == "__main__":
    main()