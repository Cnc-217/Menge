#coding:utf-8
import sys
from xml.dom.minidom import parse
import numpy as np
import os
reload(sys)
sys.setdefaultencoding('utf8')

def updateXML(Prob_choose_manner, Prob_choose_shop, num_maxwalk, num_group, intensity_Group):
    domTree = parse("./Town_v2B_test2.xml")
    rootNode = domTree.documentElement

    '''下面更新Prob_choose_shop概率'''
    # 找到Goal节点,对每个goal节点的weight赋值
    Goals = rootNode.getElementsByTagName("Goal")
    manner_prob_index = 0
    shop_prob_index = 0
    walk_index = 0
    group = 0
    for Goal in Goals:
        Goal.setAttribute('weight', str(Prob_choose_shop[group][walk_index][manner_prob_index][shop_prob_index]))  #更新数值 3*10*3*10
        shop_prob_index = shop_prob_index + 1
        if (shop_prob_index == 10):
            shop_prob_index = 0
            manner_prob_index = manner_prob_index + 1
            if (manner_prob_index == 3):
                manner_prob_index = 0
                group = group + 1
                if (group == 3):
                    walk_index = walk_index + 1
                    group = 0
                    if (walk_index == num_maxwalk):
                        break

    '''下面更新Prob_choose_manner概率'''
    States = rootNode.getElementsByTagName("State")
    manner_prob_index = 0
    walk_index = 0
    group = 0
    for State in States:
        # 找到节点   Prob_choose_manner
        if(State.hasAttribute("weight")):
            State.setAttribute('weight', str(Prob_choose_manner[group][walk_index][manner_prob_index]))
            manner_prob_index = manner_prob_index + 1
            if (manner_prob_index == 4):
                walk_index = walk_index + 1
                manner_prob_index = 0
                if (walk_index == num_maxwalk):
                    walk_index = 0
                    group = group +1
                    if (group == num_group):
                        break

    with open("./Town_v2B_test2.xml", 'w') as f:
        # 缩进 - 换行 - 编码
        domTree.writexml(f, addindent='', encoding='utf-8')

    '''下面更新仿真人数'''
    domTree = parse("./Town_v2S.xml")
    rootNode = domTree.documentElement
    Generators = rootNode.getElementsByTagName("Generator")
    i = 0
    for Generator in Generators:
        if i == 0: #A group
            Generator.setAttribute('count_x', str(int(intensity_Group[0])))
            Generator.setAttribute('count_y', str(int(intensity_Group[0])))
        elif i == 1: #B group
            Generator.setAttribute('count_x', str(int(intensity_Group[1])))
            Generator.setAttribute('count_y', str(int(intensity_Group[1])))
        elif i == 2: #C group
            Generator.setAttribute('count_x', str(int(intensity_Group[2])))
            Generator.setAttribute('count_y', str(int(intensity_Group[2])))
        i = i + 1

    with open("./Town_v2S.xml", 'w') as f:
        # 缩进 - 换行 - 编码
        domTree.writexml(f, addindent='', encoding='utf-8')


def Init(num_maxwalk, num_group, characteristic_shopper):
    '''初始化类型选择矩阵，店铺选择矩阵，店铺选择概率都一样'''
    Prob_choose_manner = np.ones([num_group, num_maxwalk, 4])*0.33 #选择类型的概率矩阵,规格是3*10*4，选择4种行为，eat,play,buy以及离开
    Prob_choose_shop = np.ones([num_group, num_maxwalk, 3, 10])*0.1 #选择商店的概率矩阵，规格是3*10*3*10，在eat,play,buy中选择10种店

    for i in range(0, 10):
        '''初始化A类游客：吃喝   5:2.5:2.5   离开的概率为0.05 0.1 0.15 ... 0.5'''
        Prob_choose_manner[0, i, 3] = 0.05 + i*0.05
        Prob_choose_manner[0, i, 0] = (1 - Prob_choose_manner[0, i, 3]) * characteristic_shopper[0][0]
        Prob_choose_manner[0, i, 1] = (1 - Prob_choose_manner[0, i, 3]) * characteristic_shopper[0][1]
        Prob_choose_manner[0, i, 2] = (1 - Prob_choose_manner[0, i, 3]) * characteristic_shopper[0][2]
        '''初始化B类游客：玩乐   2.5:5:2.5'''
        Prob_choose_manner[1, i, 3] = 0.05 + i * 0.05
        Prob_choose_manner[1, i, 1] = (1 - Prob_choose_manner[0, i, 3]) * characteristic_shopper[1][1]
        Prob_choose_manner[1, i, 0] = (1 - Prob_choose_manner[0, i, 3]) * characteristic_shopper[1][0]
        Prob_choose_manner[1, i, 2] = (1 - Prob_choose_manner[0, i, 3]) * characteristic_shopper[1][2]
        '''初始化C类游客：购买   2.5:2.5:5'''
        Prob_choose_manner[2, i, 3] = 0.05 + i * 0.05
        Prob_choose_manner[2, i, 2] = (1 - Prob_choose_manner[0, i, 3]) * characteristic_shopper[2][2]
        Prob_choose_manner[2, i, 0] = (1 - Prob_choose_manner[0, i, 3]) * characteristic_shopper[2][0]
        Prob_choose_manner[2, i, 1] = (1 - Prob_choose_manner[0, i, 3]) * characteristic_shopper[2][1]

    return Prob_choose_manner, Prob_choose_shop

def Template(Prob_choose_shop, coupon_group, coupon_manner, coupon_strenth, coupon_shopindex,num_maxwalk, num_group):
    if(coupon_group == coupon_manner): #例子：给吃喝型游客发放吃喝型优惠券 该店铺概率和其他店铺概率为：
        for m in range(0, num_maxwalk):
            temp = Prob_choose_shop[coupon_group, m, coupon_manner, coupon_shopindex]
            Prob_choose_shop[coupon_group, m, coupon_manner, :] = Prob_choose_shop[coupon_group, m, coupon_manner, :] * (1-0.07*coupon_strenth)
            Prob_choose_shop[coupon_group, m, coupon_manner, coupon_shopindex] = temp + temp * 0.07 * coupon_strenth * 9
            #strenth = 1: choose_shop = 0.163, not_choose_shop = 0.093
            #strenth = 2: choose_shop = 0.226, not_choose_shop = 0.086
            #strenth = 3: choose_shop = 0.289, not_choose_shop = 0.079
    else:  # 例子：给吃喝型游客发放其他优惠券，该店铺概率和其他店铺概率为：
        for m in range(0, num_maxwalk):
            temp = Prob_choose_shop[coupon_group, m, coupon_manner, coupon_shopindex]
            Prob_choose_shop[coupon_group, m, coupon_manner, :] = Prob_choose_shop[coupon_group, m, coupon_manner, :] * (1 - 0.05 * coupon_strenth)
            Prob_choose_shop[coupon_group, m, coupon_manner, coupon_shopindex] = temp + temp * 0.05 * coupon_strenth * 9
            # strenth = 1: choose_shop = 0.145, not_choose_shop = 0.095
            # strenth = 2: choose_shop = 0.19, not_choose_shop = 0.09
            # strenth = 3: choose_shop = 0.235, not_choose_shop = 0.085

    return Prob_choose_shop

def Trans(Prob_choose_shop, coupon_input,num_maxwalk, num_group):
    '''只影响店铺进店率，不影响人选择游玩方式的概率'''
    for Group in range(0, 3): #三种人群
        for Manner in range(0, 3): ##三种游玩方式
            for shop_index in range(0, 10): #十个店铺
                if(coupon_input[Group, Manner, shop_index] != 0):
                    Prob_choose_shop = Template(Prob_choose_shop, Group, Manner, coupon_input[Group, Manner, shop_index], shop_index,num_maxwalk, num_group)

    return Prob_choose_shop


def Input_coupon(M0,M1,M2,Sh0,Sh1,Sh2,S):
    '''只投放利润最大的店（设置好的）的优惠券'''
    #print coupon_group, coupon_manner, coupon_strenth
    #coupon_group, coupon_manner, coupon_strenth = 1, 0, 2
    #shopindex_eat_profit = 3  # 对应编号为4的店,利润最大
    #shopindex_play_profit = 3  # 对应编号为27的店，利润最大
    #shopindex_buy_profit = 7  # 对应编号为2的店，利润最大
    coupon_input = np.zeros([3, 3, 10]) #三种人群，3种游玩方式，10家店铺的优惠券，强度为0-2，考虑多个优惠券同时发放
    if M0 == 1:
        if Sh0 != 0:
            coupon_input[0, 0, Sh0 - 1] = S
        if Sh1 != 0:
            coupon_input[1, 0, Sh1 - 1] = S
        if Sh2 != 0:
            coupon_input[2, 0, Sh2 - 1] = S
    if M1 == 1:
        if Sh0 != 0:
            coupon_input[0, 1, Sh0 - 1] = S
        if Sh1 != 0:
            coupon_input[1, 1, Sh1 - 1] = S
        if Sh2 != 0:
            coupon_input[2, 1, Sh2 - 1] = S
    if M2 == 1:
        if Sh0 != 0:
            coupon_input[0, 2, Sh0 - 1] = S
        if Sh1 != 0:
            coupon_input[1, 2, Sh1 - 1] = S
        if Sh2 != 0:
            coupon_input[2, 2, Sh2 - 1] = S
    return coupon_input

def XML(M0,M1,M2,Sh0,Sh1,Sh2,S):
    shop_eat = [1, 28, 24, 4, 12, 6, 15, 19, 18, 29]
    shop_play = [0, 11, 3, 27, 14, 25, 5, 16, 8, 21]
    shop_buy = [9, 20, 7, 22, 10, 23, 17, 2, 26, 13]
    #num_shop = 30
    num_maxwalk = 10
    num_group = 3
    intensity_Group = [10, 10, 10]
    characteristic_shopper = [[0.5, 025, 0.25], [0.25, 0.5, 0.25], [0.25, 0.25, 0.5]]
    Prob_choose_manner, Prob_choose_shop = Init(num_maxwalk, num_group, characteristic_shopper)
    coupon_input = Input_coupon(M0,M1,M2,Sh0,Sh1,Sh2,S)
    #print coupon_input
    Prob_choose_shop = Trans(Prob_choose_shop, coupon_input, num_maxwalk, num_group)
    #print("num_group= " + str(num_group))
    #print(Prob_choose_manner)
    #print(Prob_choose_shop)
    #print Prob_choose_shop[0]
    updateXML(Prob_choose_manner, Prob_choose_shop, num_maxwalk, num_group, intensity_Group)


if __name__ == '__main__':
    XML(1,0,1,0,2,3,3)