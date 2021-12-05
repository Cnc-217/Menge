import urllib.request
import urllib.parse
import redis
import json
def test():


    # 发送action
    # http://10.28.195.233:5000/olympic/action?actionType=flow&actionList=[1,2,35]
    data_dict={"actionType":"flow","actionList":"[1,2,35]","pid": 1}#待发送的数据
    data=urllib.parse.urlencode(data_dict).encode("utf8") #使用urlencode将字典参数序列化成字符串
    request=urllib.request.Request("http://10.28.195.233:5000/olympic/action", data)   # 请求处理
    reponse=urllib.request.urlopen(request).read() # 读取结果
    print(reponse.decode())

    # 终止指定类型的程序仿真
    # http://10.28.195.233:5000/olympic/stopService
    data_dict = {"pid": 115720}  # 待发送的数据
    data = urllib.parse.urlencode(data_dict).encode("utf8")  # 使用urlencode将字典参数序列化成字符串
    request = urllib.request.Request("http://10.28.195.233:5000/olympic/stopService", data)  # 请求处理
    reponse = urllib.request.urlopen(request).read()  # 读取结果
    print(reponse.decode())

    # 开始指定类型的程序仿真
    # http://10.28.195.233:5000/olympic/startService
    data_dict = {"sceneType": "matrix"}  # 待发送的数据
    data = urllib.parse.urlencode(data_dict).encode("utf8")  # 使用urlencode将字典参数序列化成字符串
    request = urllib.request.Request("http://10.28.195.233:5000/olympic/startService", data)  # 请求处理
    reponse = urllib.request.urlopen(request).read()  # 读取结果
    print(reponse.decode())
    recive = reponse.decode()
    jsonData = json.loads(recive)
    print(jsonData["pid"])


if __name__ == '__main__':
    # 从redis获取数据
    redisServer = redis.Redis(host='10.28.195.233', port=6379, decode_responses=True)
    data = redisServer.get(23004)
    jsonData = json.loads(data)
    datalist = jsonData["data"]
    print(datalist)




