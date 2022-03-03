# coding:utf-8
from flask_apscheduler import APScheduler

from scenes.simulations import Simulations
import redis

# sim对象用于维持当前Menge仿真的参数信息
sims = Simulations()
# redis服务器连接
#redisServer = redis.Redis(host='10.28.195.233', port=6379, decode_responses=True)
redisServer = redis.Redis(host='127.0.0.1', port=6379, decode_responses=True)
for elem in redisServer.keys():
    redisServer.delete(elem)

scheduler = APScheduler()  # 实例化 APScheduler
