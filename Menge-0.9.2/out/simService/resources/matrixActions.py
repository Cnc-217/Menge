#coding:utf-8

def matrixFlowAction(maritx,action_index):
    '''
    matirx: 36*36 list
    acition_index = [1,2,35] list
    '''
    maritx_res = maritx
    for num in action_index:
        temp_index =[]
        for i in range(36):
            if maritx[i][num]!=0:
                temp_index.append(i)
                maritx_res[i][num]*=0.5
        for index in temp_index:
            for i in range(36):
                if i!=num:
                    maritx_res[index][i]+=maritx[index][num]*(1-0.5)/35
    print("matrix change")
    return maritx_res


# def matrixFlowAction(maritx,action_index):
#
#     print("matrix change")
#     return maritx



# def matrixBusinessAction(maritx,strength,shop):
#     #strength 取值 1,2,3
#     #聚集算法
#     rows,cols = maritx.shape
#     for i in range(rows):
#         for j in range(cols):
#             if j!= shop:
#                 maritx[i][shop] = maritx[i][shop] + maritx[i][j]*0.1*strength
#                 maritx[i][j] = maritx[i][j]*(1-0.1*strength)
#
#     return maritx

def matrixBusinessAction(matrix,action):
    # acition = [strength,shopID]
    shop = action[0]
    strength = action[1]
    for i in range(len(matrix)):
        for j in range(len(matrix[i])):
            if j!=shop:
                matrix[i][shop] += matrix[i][j]*0.03*strength
                matrix[i][j] = matrix[i][j]*(1-0.03*strength)
    # print("matrix change")
    print(matrix)
    return matrix