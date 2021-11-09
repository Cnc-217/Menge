#coding:utf-8



if __name__ == '__main__':
    with open("D:/File/Project/git/Menge-0.9.2/examples/Olympic/matrix.txt", "r") as f:
        data = f.readlines()

    strTmp = ""
    l = []
    for i in range(len(data)):  # len(data)为数据行数
        ls = []
        for c in data[i]:
            if(c==" " or c=="\n" ):
                ls.append(strTmp)
                strTmp = ""
                continue
            strTmp = strTmp + c
        l.append(ls)



    for i in range(len(l)):
        for j in range(len(l[i])-1,-1,-1):
            if(l[i][j]==""):
                l[i].pop(j)

    #print(len(l))
    for i in range(len(l)):
        #print(len(l[i]))
        for j in range(len(l[i])):
            l[i][j] = float(l[i][j])

    l[35].append(float(0))
    print(len(l))
    print(len(l[35]))
    print(l)


