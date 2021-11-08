import urllib.request
import urllib.parse
import redis
import json


# 开始指定类型的程序仿真
# http://10.28.195.233:5000/olympic/startService
request=urllib.request.Request("http://10.28.195.233:5000/olympic/startService")   # 请求处理
reponse=urllib.request.urlopen(request).read() # 读取结果
print(reponse.decode())

# 终止指定类型的程序仿真
# http://10.28.195.233:5000/olympic/stopService
request=urllib.request.Request("http://10.28.195.233:5000/olympic/stopService")   # 请求处理
reponse=urllib.request.urlopen(request).read() # 读取结果
print(reponse.decode())

# 发送action
# http://10.28.195.233:5000/olympic/action?actionType=flow&actionList=[1,2,35]
data_dict={"actionType":"flow","actionList":"[1,2,35]"}#待发送的数据
data=urllib.parse.urlencode(data_dict).encode("utf8") #使用urlencode将字典参数序列化成字符串
request=urllib.request.Request("http://10.28.195.233:5000/olympic/action", data)   # 请求处理
reponse=urllib.request.urlopen(request).read() # 读取结果
print(reponse.decode())


#从redis获取数据
redisServer = redis.Redis(host='10.28.195.233', port=6379, decode_responses=True)
data = redisServer.get("data")
jsonData = json.loads(data)
datalist = jsonData["data"]
print(datalist)

