from dqn_HKpark_restate import DQN
from flask import Flask, request
from flask_cors import CORS
import urllib.request
import urllib.parse
import redis
import json
import os, sys

app = Flask(__name__)

@app.route('/', methods=['GET','POST'])
def choose_action_api():
    if request.method == 'POST':
        pid = int(request.form.get('pid'))

    action_dic={
    1:"[0,1,2]",
    2:"[3,4,5,6,7,8,9]",
    3:"[10]",
    4:"[11,12]",
    5:"[13,14]",
    6:"[15,16,17,18,19]",
    7:"[20,21,22,23,24]",
    8:"[25,26,27]",
    9:"[28,29,30,31,32,33]",
    0:"[34,35]"
    }

    dqn = DQN()
    #加载模型
    dqn.load_model(sys.path[0]+'/dqn_model_restate_time')
    print('dqn 已加载')
    #获取环境状态
    redisServer = redis.Redis(host='10.28.164.81',port=6379, decode_responses=True)
    data = redisServer.get(pid)
    jsonData = json.loads(data)
    data_result = jsonData["data"]
    # print('state 已获得')
    aciton_index = dqn.choose_realaction(data_result+[0]*28)
    print({'action_list': action_dic[aciton_index]})
    # aciton_index = dqn.choose_realaction([0]*64)


    return  {'action_list': action_dic[aciton_index]}


# @app.route('/get_pid/', methods=['GET','POST'])
# def get_pid():
#     pid = 0 #get new pid
#     return pid 

if __name__=='__main__':
    CORS(app, resources=r'/*')
    app.run(host='0.0.0.0',port=8000)  #host：定义主机ip地址