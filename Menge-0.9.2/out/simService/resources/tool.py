#coding:utf-8

def matrixToStr(matrix):
    # 将矩阵转换为为字符串
    matrixStr = ""
    rowNum = len(matrix)
    columnNUm = len(matrix[0])
    sumNum = 0
    for i in matrix:
        for j in i:
            matrixStr = matrixStr + str(j) + " "
            sumNum = sumNum + 1
    if (sumNum != rowNum ** 2 or rowNum != columnNUm):
        print("error!!!")
        exit()
    return matrixStr


def listToStr(list):
    # 将列表转为字符串
    listStr = ""
    for i in list:
        listStr = listStr + str(i) + " "

    return listStr