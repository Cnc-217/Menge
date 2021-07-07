from xml.dom.minidom import parse
import numpy as np

def updateXML(Prob_choose, Prob_buy, Prob_out):
    domTree = parse("./townB.xml")
    rootNode = domTree.documentElement

    '''下面更新walk概率'''
    #找到GoalSet节点
    GoalSets = rootNode.getElementsByTagName("GoalSet")
    #for GoalSet in GoalSets:
        # 找到Goal节点,对每个goal节点的weight赋值
    Goals = rootNode.getElementsByTagName("Goal")
    shop_prob_index = 0
    walk_index = 0
    group = 0
    for Goal in Goals:
            Goal.setAttribute('weight', str(Prob_choose[group][walk_index][shop_prob_index]))  #更新数值
            shop_prob_index = shop_prob_index + 1
            if(shop_prob_index == num_shop ):
                shop_prob_index = 0
                walk_index = walk_index + 1
                if(walk_index == num_maxwalk):
                    walk_index = 0
                    group = group + 1
                    if(group == num_group):
                        break

    '''下面更新buy和out概率'''
    Transitions = rootNode.getElementsByTagName("Transition")
    Transition_prob_index = 0
    walk_index = 0
    group = 0
    for Transition in Transitions:
        # 找到Transition节点
        States = Transition.getElementsByTagName("State")
        for State in States:
            if(Transition_prob_index == 0 or Transition_prob_index == 1 ):  #购买概率
                State.setAttribute('weight', str(Prob_buy[group][walk_index][Transition_prob_index]))  #更新数值
            elif(Transition_prob_index == 2 or Transition_prob_index == 3 ):  #离开概率
                State.setAttribute('weight', str(Prob_out[group][walk_index][Transition_prob_index-2]))  # 更新数值
            Transition_prob_index = Transition_prob_index + 1
            if (Transition_prob_index == 4 or ( Transition_prob_index == 2 and walk_index == num_maxwalk - 1)): #这里的4是购买/离开有的概率有四种
                Transition_prob_index = 0
                walk_index = walk_index + 1
                if (walk_index == num_maxwalk):
                    walk_index = 0
                    group = group + 1
                    Transition_prob_index = 0


    with open('D:\\File\\Project\\Menge-master\\examples\\town\\updated_townB.xml', 'w') as f:
        # 缩进 - 换行 - 编码
        domTree.writexml(f, addindent='  ', encoding='utf-8')

def Init(num_shop, num_maxwalk, num_group):
    '''初始化每次walk到各个店概率相等=1，是否购物概率相等=1'''
    Prob_choose = np.ones([num_group, num_maxwalk, num_shop])*0.25
    Prob_buy = np.zeros([num_group, num_maxwalk, 2])
    Prob_out = np.zeros([num_group, num_maxwalk-1, 2])
    '''此处定义：不同种类游客购买的概率和离开的概率，游客回头逛之后，逛完离开的概率'''
    for j in range(0, num_group):
        for i in range(0, num_maxwalk):
            if(j == 0):                             #0 普通游客
                if(i <= num_shop - 1):
                    Prob_buy[j][i] = (0.5, 0.5)
                    Prob_out[j][i] = (0.99, 0.01)
                elif( i == num_shop ):
                    Prob_buy[j][i] = (0.7, 0.3)
                    Prob_out[j][i] = (0.4, 0.6)
                elif (i == num_shop +1  ):
                    Prob_buy[j][i] = (0.7, 0.3)


            elif (j == 1):                          #1 shopping游客
                if (i <= num_shop - 1):
                    Prob_buy[j][i] = (0.6, 0.4)
                    Prob_out[j][i] = (0.99, 0.01)
                elif (i == num_shop):
                    Prob_buy[j][i] = (0.8, 0.2)
                    Prob_out[j][i] = (0.4, 0.6)
                elif (i == num_shop + 1):
                    Prob_buy[j][i] = (0.8, 0.2)

            elif (j == 2):                          #2 观光游客
                if (i <= num_shop - 1):
                    Prob_buy[j][i] = (0.3, 0.7)
                    Prob_out[j][i] = (0.99, 0.01)
                elif (i == num_shop):
                    Prob_buy[j][i] = (0.7, 0.3)
                    Prob_out[j][i] = (0.2, 0.8)
                elif (i == num_shop + 1):
                    Prob_buy[j][i] = (0.7, 0.3)

   # Prob_buy[:, :, 0] = Prob_buy[:, :, 0] + 2

    return Prob_choose, Prob_buy, Prob_out


def Transformation(Prob_choose, Prob_buy, Prob_out ):
    coupon = np.zeros([num_group, 4, num_shop]) #3个种类的人群,4种折扣方式,4个店铺
    coupon_shopchoice_transfer = np.ones([num_group, 4, num_shop])
    coupon_buy_transfer = np.zeros([num_group, 4])
    coupon_out_transfer = np.zeros([num_group, 4])
    #for i in range(0, 3):
        #for j in range(0, 3):
            #coupon[i][j] = input("please input the number of coupon %d for group %d:" %((i+1), (j+1)))
            #代金券i对group1 2 3 进行投放，产生概率影响
            #这里coupon的值为 0 1 2 可以理解为强度（折扣力度/代金券个数），3及以上强度的这代金券视为无效，边际效益低
    coupon[:, 0:2, :] = 1
    coupon[:, 3, :] = 1
    #coupon[0][0][2] = 1 #店铺3给所有人群投放强度为1的A类消费券
    #coupon[1][0][2] = 1
    #coupon[2][0][2] = 1
    #coupon[0][1][2] = 1#店铺3给所有人群投放强度为1的类消B券
   # coupon[1][1][2] = 1
    #coupon[2][1][2] = 1
    #coupon[0][0][3] = 1
    #coupon[1][0][3] = 1
    #coupon[0][3][0] = 2
    #coupon[1][3][0] = 2
    #coupon[0][3][0] = 2
    #coupon[1][3][0] = 2
    #coupon[2][3][0] = 2
    #coupon[2][2][3] = 1
    for i in range(0, num_group):
        for k in range(0, num_shop):
            # 折扣A规则转换，折扣A只影响进店率
            if(coupon[i][0][k] == 1 and i != 1): coupon_shopchoice_transfer[i][0][k] = 1.6  #对于普通型和观光型进店强度为1.6
            elif(coupon[i][0][k] >= 2 and i != 1): coupon_shopchoice_transfer[i][0][k] = 2
            elif(coupon[i][0][k] == 1 and i == 1): coupon_shopchoice_transfer[i][0][k] = 1.8  #对于购物型进店强度为2
            elif(coupon[i][0][k] >= 2 and i == 1): coupon_shopchoice_transfer[i][0][k] = 2

            # 折扣B规则转换，折扣B只影响进店率
            if (coupon[i][1][k] == 1 and i != 2): coupon_shopchoice_transfer[i][1][k] = 1.4  # 对于普通型和购物型进店强度为1.4
            elif (coupon[i][1][k] >= 2 and i != 2): coupon_shopchoice_transfer[i][1][k] = 1.8
            elif (coupon[i][1][k] == 1 and i == 2): coupon_shopchoice_transfer[i][1][k] = 1.6  # 对于观光型进店强度为1.6
            elif (coupon[i][1][k] >= 2 and i == 2): coupon_shopchoice_transfer[i][1][k] = 2

            # 折扣C规则转换，折扣C影响进店率和额外进店率
            if (coupon[i][2][k] == 1 and i != 2):
                coupon_shopchoice_transfer[i][2][k] = 1.6  # 对于普通型和购物型进店强度为1.6，额外进店率增加0.2
                coupon_out_transfer[i][2] = 0.1
            elif (coupon[i][2][k] >= 2 and i != 2):
                coupon_shopchoice_transfer[i][2][k] = 1.8
                coupon_out_transfer[i][2] = 0.2
            elif (coupon[i][2][k] == 1 and i == 2):  coupon_out_transfer[i][2] = 0.05  # 对于观光型额外进店增加0.1
            elif (coupon[i][2][k] >= 2 and i == 2):  coupon_out_transfer[i][2] = 0.1

            # 折扣D的规则转换，折扣D影响购买率
            if (coupon[i][3][k] == 1 and i != 2): coupon_buy_transfer[i][3] = 0.1 # 对于普通型和购物型 购买率增加0.1
            elif (coupon[i][3][k] >= 2 and i != 2): coupon_buy_transfer[i][3] = 0.15
            elif (coupon[i][3][k] == 1 and i == 2): coupon_buy_transfer[i][3] = 0.05  # 对于观光型 购买率增加0.05
            elif (coupon[i][3][k] >= 2 and i == 2): coupon_buy_transfer[i][3] = 0.1


    for coupon_style in range(0,4):  #四种折扣方式,三种人群，通过强度的方式更新进店概率
        for group_index in range(0, num_group):
            #更新进店概率
            Prob_choose[group_index][0:num_maxwalk-2] = Prob_choose[group_index][0:num_maxwalk-2]*coupon_shopchoice_transfer[group_index][coupon_style]
            #更新离开小镇概率
            Prob_out[group_index][num_maxwalk-2][0] = Prob_out[group_index][num_maxwalk-2][0]+coupon_out_transfer[group_index][coupon_style]
            Prob_out[group_index][num_maxwalk-2][1] = Prob_out[group_index][num_maxwalk-2][1]-coupon_out_transfer[group_index][coupon_style]
            #更新购买概率
            Prob_buy[group_index][0:num_maxwalk-2, 0] = Prob_buy[group_index][0:num_maxwalk-2, 0]+coupon_buy_transfer[group_index][coupon_style]
            Prob_buy[group_index][0:num_maxwalk-2, 1] = Prob_buy[group_index][0:num_maxwalk-2, 1]-coupon_buy_transfer[group_index][coupon_style]

    #下面是惩罚人数过多
    for walk in range(0, num_maxwalk-2):
        for shop_index in range(0, num_shop):
            prob = 0
            for group_index in range(0, num_group):
                prob = prob + Prob_choose[group_index][walk][shop_index]/Prob_choose[group_index][walk].sum()
            if(prob >= 1.13):
              while(prob >= 0.90):
                  Prob_choose[0][walk][shop_index] = Prob_choose[0][walk][shop_index] - 0.02
                  Prob_choose[1][walk][shop_index] = Prob_choose[1][walk][shop_index] - 0.01
                  Prob_choose[2][walk][shop_index] = Prob_choose[2][walk][shop_index] - 0.03
                  if(Prob_choose[2][walk][shop_index] <= 0): Prob_choose[2][walk][shop_index] = 0
                  prob = 0
                  for group_index in range(0, num_group):
                      prob = prob + Prob_choose[group_index][walk][shop_index] / Prob_choose[group_index][walk].sum()
        # 惩罚购买率过高
    if(Prob_buy[0][0][0]>=0.65):
        Prob_out[0][num_maxwalk-2][0] = Prob_out[0][num_maxwalk-2][0] - 0.2
        Prob_out[0][num_maxwalk-2][1] = Prob_out[0][num_maxwalk-2][1] + 0.2




    return Prob_choose, Prob_buy, Prob_out


if __name__ == '__main__':
    num_shop = 4
    num_maxwalk = 6
    num_state = 2 #状态为买或不买
    list_group = ['group', 'group_shopping', 'group_sightseeing']
    num_group = len(list_group)
    Prob_choose, Prob_buy, Prob_out = Init(num_shop, num_maxwalk, num_group)
    Transformation(Prob_choose, Prob_buy, Prob_out)
    print("num_group= " + str(num_group))
    print(Prob_choose)
    print(Prob_buy)
    print(Prob_out)
    updateXML(Prob_choose, Prob_buy, Prob_out)
