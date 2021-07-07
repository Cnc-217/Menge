import torch
import torch.nn as nn
from torch.autograd import Variable
import torch.nn.functional as F
import numpy as np

#DQN
class Net(nn.Module):
    def __init__(self, ):
        super(Net, self).__init__()
        self.fc1 = nn.Linear(30 , 62 )
        #self.fc1.weight.data.normal_(0, 0.1)   # initialization
        self.out = nn.Linear(62 , 3)
        #self.out.weight.data.normal_(0, 0.1)   # initialization

    def forward(self, x):
        x = self.fc1(x)
        x = F.relu(x)
        actions_value = self.out(x)
        return actions_value

class DQN(object):
    def __init__(self):
        self.eval_net, self.target_net = Net(), Net()

        self.learn_step_counter = 0                                     # for target updating
        self.memory_counter = 0                                         # for storing memory
        self.memory = np.zeros((MEMORY_CAPACITY, 62 ))     # initialize memory
        self.optimizer = torch.optim.Adam(self.eval_net.parameters(), lr=LR)
        self.loss_func = nn.MSELoss()

    def choose_action(self, x):
        x = Variable(torch.unsqueeze(torch.FloatTensor(x), 0))
        # input only one sample
        if np.random.uniform() < EPSILON:   # greedy
            actions_value = self.eval_net.forward(x)
            action = torch.max(actions_value, 1)[1].data.numpy()[0]     # return the argmax
        else:   # random
            action = np.random.randint(0, N_ACTIONS)
        return action

    def store_transition(self, s, a, r, s_):
        transition = np.hstack((s, [a, r], s_))
        # replace the old memory with new memory
        index = self.memory_counter % MEMORY_CAPACITY
        self.memory[index, :] = transition
        self.memory_counter += 1

    def learn(self):
        # target parameter update
        if self.learn_step_counter % TARGET_REPLACE_ITER == 0:
            self.target_net.load_state_dict(self.eval_net.state_dict())
        self.learn_step_counter += 1

        # sample batch transitions
        sample_index = np.random.choice(MEMORY_CAPACITY, BATCH_SIZE)
        b_memory = self.memory[sample_index, :]
        b_s = Variable(torch.FloatTensor(b_memory[:, :N_STATES]))
        b_a = Variable(torch.LongTensor(b_memory[:, N_STATES:N_STATES+1].astype(int)))
        b_r = Variable(torch.FloatTensor(b_memory[:, N_STATES+1:N_STATES+2]))
        b_s_ = Variable(torch.FloatTensor(b_memory[:, -N_STATES:]))

        # q_eval w.r.t the action in experience
        q_eval = self.eval_net(b_s).gather(1, b_a)  # shape (batch, 1)
        q_next = self.target_net(b_s_).detach()     # detach from graph, don't backpropagate
        q_target = b_r + GAMMA * q_next.max(1)[0]   # shape (batch, 1)
        loss = self.loss_func(q_eval, q_target)

        self.optimizer.zero_grad()
        loss.backward()
        self.optimizer.step()
        
    def save_model(self, PATH=None):
        torch.save(self.target_net.state_dict(),PATH)
        
    def load_model(self, PATH=None):
        self.target_net.load_state_dict(torch.load(PATH))


#初始化参数
# Hyper Parameters
BATCH_SIZE = 32
LR = 0.01                   # learning rate
EPSILON = 0.9               # greedy policy
GAMMA = 0.9                 # reward discount
TARGET_REPLACE_ITER = 100
MEMORY_CAPACITY = 400
N_ACTIONS = 3 #env.action_space
N_STATES = 30 # env.observation_space.shape[0]

# town.guestsSize = 1000
# town.storesSize = 30
# town.until = 1000

s = {}
a = {}
# town.reward = 0
# dqn = None # !

rlstep = 30

#构建dqn模型
dqn = DQN()

#计算
def reward_function(time,next_time):
    pass

def loss_funtion():
    pass

def dynamicRL(now, state):

    #将当前状态键入字典 字典key：value= 时间戳：此时状态
    s.update({now : state})
    #根据当前状态选取动作
    action_s = dqn.choose_action(s[now])
    #将选取动作键入字典 字典key：value= 时间戳：此时动作
    a.update({now: action_s})
    #if 语句跳过第一次选取动作
    if now !=rlstep:
        #将[前一时刻的状态，前一时刻的动作，前一时刻和当前时刻计算的reward，此时刻的状态]压入记忆池，进行初始预训练
        dqn.store_transition(s.pop(now - rlstep) , a.pop([now-rlstep]), reward_function(now-rlstep,now), s[now])
    
    #进行DQN模型的学习
    if dqn.memory_counter > MEMORY_CAPACITY:
        dqn.learn()

# if __name__ == '__main__': # CORR
#     #训练
#     for i in range(40000):
#         dynamicRL(now,stores)
#     #保存模型
#     dqn.save_model('./dqn_model')

#     #加载模型
#     #dqn.load_model('./dqn_model')


# # runWithDQN()

