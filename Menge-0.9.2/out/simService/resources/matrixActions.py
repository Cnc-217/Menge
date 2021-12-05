#coding:utf-8

# def matrixFlowAction(maritx,action_index):
#     '''
#     matirx: 36*36 list
#     acition_index = [1,2,35] list
#     '''
#     maritx_res = maritx
#     for num in action_index:
#         temp_index =[]
#         for i in range(36):
#             if maritx[i][num]!=0:
#                 temp_index.append(i)
#                 maritx_res[i][num]*=0.5
#         for index in temp_index:
#             for i in range(36):
#                 if i!=num:
#                     maritx_res[index][i]+=maritx[index][num]*(1-0.5)/35
#     print("matrix change")
#     return maritx_res

def matrixFlowAction(maritx,action_index):

    print("matrix change")
    return maritx