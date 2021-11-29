#coding:utf-8

def listFlowAction(influence,action_index):
    '''
    matirx: 1*36 list
    acition_index = [1,2,35] list
    '''
    influence[action_index[2]] = influence[action_index[2]]+1
    print("influence change")
    return influence