#coding:utf-8
from simService import *
from simService.socket.socketServer import SocketServer
from scenes.simulation import Simulation
import redis


#socket服务器，用于和Menge通信
socketServer = SocketServer("10.28.195.233", 12660)
#sim对象用于维持当前Menge仿真的参数信息
sim = Simulation()
#redis服务器连接
redisServer = redis.Redis(host='10.28.195.233', port=6379, decode_responses=True)
redisServer.setnx("data","")

