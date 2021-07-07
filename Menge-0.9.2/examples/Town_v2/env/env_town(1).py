
import logging
import random
import gym
import sys
import numpy as np

max = 10

def assemble(arr,strength,shop):
    #strength 取值 1,2,3
    #聚集算法
    if len(arr.shape) == 2:
        rows,cols = arr.shape
        for i in range(rows):
            pos = np.unravel_index(np.argmax(arr[i]),arr.shape)
            print(pos)
            for j in range(cols):
                if j!= pos[1]:
                    arr[i][pos[1]] = arr[i][pos[1]] + arr[i][j]*0.1*strength
                    arr[i][j] = arr[i][j]*(1-0.1*strength)

    if len(arr.shape) == 1:
        cols = arr.shape
        print(cols)
        #pos = np.unravel_index(np.argmax(arr),arr.shape)
        print(shop)
        for j in range(cols[0]):
            if j!= shop:
                arr[shop] = arr[shop] + arr[j]*0.1*strength
                arr[j] = arr[j]*(1-0.1*strength)
    return arr

def calculate_reward(arr):
    #计算收益时，假设每个店的容量上限为20，且人均收益均为50
    state = arr.copy()
    print(state)
    for i in range(len(state[0])):
        if state[0][i]>max:
            state[0][i] = max
    reward = np.sum(state)*10
    return reward

def action_transfer(action):
    trans_action = [[0,0,0],[0,0,0],0]
    flag1 = int(action/90)
    trans_action[0][flag1] = 1
    action2 = action%90
    flag3 = int(action2/30)
    trans_action[2] = flag3+1
    action3 = action2%30
    flag2 = int(action3/10)
    shop_number = action3%10
    trans_action[1][flag2] = shop_number
    return trans_action



class Tourist:
    def __init__(self,amount,preference):
        self.amount = amount
        self.preference = preference


class Town(gym.Env):
    # metadata = {
    #     'render.modes': ['human', 'rgb_array'],
    #     'video.frames_per_second': 2
    # }

    def __init__(self,Tourist_A,Tourist_B,Tourist_C,Town_size):
        self.tourist_A = Tourist_A
        self.tourist_B = Tourist_B
        self.tourist_C = Tourist_C
        self.town_size = Town_size
        self.states = np.zeros(30)
        self.states = self.states[np.newaxis, :] #状态空间

        #self.actions = ['n','e','s','w']
        self.actions = [[0,0,0],[0,0,0],0]
        self.rewards = 0        #回报的数据结构为字典
        self.turn = 0



    def getStates(self):
        return self.states

    def getAction(self):
        return self.actions


    def setAction(self,s):
        self.state=s


    def reset(self):
        self.states = np.zeros(30,dtype=np.int) #状态空间
        self.states = self.states[np.newaxis, :]
        return self.states
    
    

    def classify(self):
        #输出三类人在三类商店中的人数分布矩阵
        class_A = self.tourist_A.amount*self.tourist_A.preference
        class_B = self.tourist_B.amount*self.tourist_B.preference
        class_C = self.tourist_C.amount*self.tourist_C.preference
        self.classify_arr = np.array([class_A,class_B,class_C],dtype = np.int)
        return self.classify_arr

    def generate_transfer_arr(self):
        #根据店铺数量随机生成转移概率,A1矩阵：A类人在1型商店中的分步
        transfer_arr_A1 = np.random.dirichlet(np.ones(Town_size[0]),size=1)
        transfer_arr_B1 = np.random.dirichlet(np.ones(Town_size[0]),size=1)
        transfer_arr_C1 = np.random.dirichlet(np.ones(Town_size[0]),size=1)
        self.transfer_arr1 = np.array([transfer_arr_A1,transfer_arr_B1,transfer_arr_C1]).reshape(3,10)
        transfer_arr_A2 = np.random.dirichlet(np.ones(Town_size[1]),size=1)
        transfer_arr_B2 = np.random.dirichlet(np.ones(Town_size[1]),size=1)
        transfer_arr_C2 = np.random.dirichlet(np.ones(Town_size[1]),size=1)
        self.transfer_arr2 = np.array([transfer_arr_A2,transfer_arr_B2,transfer_arr_C2]).reshape(3,10)
        transfer_arr_A3 = np.random.dirichlet(np.ones(Town_size[2]),size=1)
        transfer_arr_B3 = np.random.dirichlet(np.ones(Town_size[2]),size=1)
        transfer_arr_C3 = np.random.dirichlet(np.ones(Town_size[2]),size=1)
        self.transfer_arr3 = np.array([transfer_arr_A3,transfer_arr_B3,transfer_arr_C3]).reshape(3,10)
        #概率转移矩阵
        print("arr1: ",self.transfer_arr1)
        print("arr2: ",self.transfer_arr2)
        print("arr3: ",self.transfer_arr3)
        self.transfer_arr = np.array([self.transfer_arr1,self.transfer_arr2,self.transfer_arr3])

    def import_transfer_arr(self,transfer_arr):
        #导入转移模型
        self.transfer_arr = transfer_arr

    def transfer(self):
        #print(self.classify_arr[:,0].T)
        #print(self.transfer_arr1)
        self.distribution_1 = np.dot(self.classify_arr[:,0].T , self.transfer_arr[0])
        self.distribution_2 = np.dot(self.classify_arr[:,1].T , self.transfer_arr[1])
        self.distribution_3 = np.dot(self.classify_arr[:,2].T , self.transfer_arr[2])
        return np.array(np.r_[self.distribution_1,self.distribution_2,self.distribution_3], dtype= np.int)
    def render(self):
        self.states = np.zeros(30)
    def close(self):
        pass
    def step(self,action): #[[1,0,1],[0,4,5],3]
        #若对A/B/C类人发放优惠券,则
        for i in range(3):
            for j in range(3):
                if action[0][i] == 1:
                    if action[1][j] > 0:
                        self.transfer_arr[i][j] = assemble(self.transfer_arr[i][j],action[2],action[1][j])
        next_state = np.array([self.transfer()])
        reward = calculate_reward(next_state)
        print(self.turn)
        self.turn = self.turn + 1
        is_terminal = False
        if self.turn > 3000:
            is_terminal = True
        
        return next_state,reward,is_terminal,{}

if __name__ == "__main__":



#三类游客各自的数量
    amount = [100,100,100]


    preference_A = np.array([0.4,0.2,0.2],dtype = np.float)
    preference_B = np.array([0.2,0.4,0.2],dtype = np.float)
    preference_C = np.array([0.2,0.2,0.4],dtype = np.float)


    #第一个是店铺的种类，第二个向量矩阵将三种游客分别引到第几号店铺，第三个是力度
    action = [[1,0,1],[0,4,5],3]


    #保存的一个转移矩阵,三种类型的人群，每种类型人群选择三种类型的10个店铺的的概率
    trans_arr1 = np.array([[0.13198159,0.12267926,0.03394324,0.00853216,0.15546241,0.11173108,0.17270837,0.11110223,0.00125339,0.15060627],
    [0.11745992,0.0037841 ,0.21572155,0.02644156,0.05170315,0.03453734,0.03382342,0.05225177,0.28768236,0.17659483],
    [0.17804484,0.08000113,0.04215921,0.1061185,0.17488579,0.10628587,0.03288828,0.21665625,0.04439381,0.01856631]],dtype = np.float)
    trans_arr2 = np.array([[0.02572236,0.13146062,0.02931368,0.14156693,0.0154575,0.0755613,0.16483648,0.05938262,0.27741651,0.07928201],
    [0.01322397,0.07758511,0.04279755,0.08040708,0.53967355,0.19977114,0.00364045,0.02705851,0.00178564,0.01405701],
    [0.15212575,0.01342299,0.04251936,0.1842854,0.07146598,0.09665776,0.03347203,0.09933323,0.19595715,0.11076036]],dtype = np.float)
    trans_arr3 = np.array([[0.19875452,0.00569667,0.11067424,0.17226742,0.03405054,0.07842376,0.09091379,0.18142275,0.0869916,0.04080472],
    [0.11684422,0.05221967,0.31353433,0.03469894,0.10709484,0.03113836,0.12920174,0.00918875,0.07191168,0.13416748],
    [0.02989699,0.02912282,0.11722163,0.06163385,0.20368597,0.03293389,0.22671352,0.19890621,0.01180807,0.08807703]],dtype = np.float)

    trans_arr = np.array([trans_arr1,trans_arr2,trans_arr3])

    #小镇尺寸，即各类商店数量
    Town_size = [10,10,10]
    Tourist_A = Tourist(amount[0],preference_A)
    Tourist_B = Tourist(amount[1],preference_B)
    Tourist_C = Tourist(amount[2],preference_C)



    town_test = Town(Tourist_A,Tourist_B,Tourist_C,Town_size)
    #数据结构测试
    print(town_test.tourist_A.amount)
    #第一次分类转移测试
    print(town_test.classify())

    #town_test.generate_transfer_arr()
    town_test.import_transfer_arr(trans_arr)

    # res = town_test.transfer()
    # print("转移结果:")
    # print(res)
    # print(np.sum(res))

    print(town_test.step(action))
    print(town_test.reset())


