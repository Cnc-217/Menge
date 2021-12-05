from RL_brain import DeepQNetwork
from wsgiref.simple_server import make_server
from RL_brain import action_transfer
import json
import numpy as np
import redis
# def wsgireq(env, res):
# 	print(env)
# 	res("200 OK",[("Content-Type","application/json")])
# 	body = "{'id':1}"
# 	return [body.encode("utf-8")]

RL = DeepQNetwork(n_actions=108,
                n_features=36,
                learning_rate=0.01, e_greedy=0.9,
                replace_target_iter=100, memory_size=2000,
                e_greedy_increment=0.001,)
#导入智能体
def application(environ, start_response):
    try:
        request_body_size = int(environ.get('CONTENT_LENGTH', 0))
    except (ValueError):
        request_body_size = 0

    # 当请求方式是POST时, 变量将会被放在存在域wsgi.input文件中的HTTP请求信息中, 由WSGI 服务器一起发送.
    request_body = environ['wsgi.input'].read(request_body_size)
    d = json.loads(request_body)
    pid = d["pid"]
    print(str(request_body,encoding='utf-8'))
    print(pid)
    #接收与解析post请求体，获取pid

    redisServer = redis.Redis(host='10.28.164.81', port=6379, decode_responses=True)
    data = redisServer.get(pid)
    jsonData = json.loads(data)
    real_world_data = jsonData["data"]#获取真实世界数据
    arr_data = np.array(real_world_data)
    np.reshape(arr_data,[1,36])
    action = RL.choose_action(arr_data)
    trans_action = action_transfer(action)#转向量
    print("trans_action:  ",trans_action)
    status = '200 OK'
    response_body = "{'shop':"+str(trans_action[0])+",'strength':"+str(trans_action[1])+"}"#第一个是发优惠券的店铺类型，第二个是优惠力度


    response_headers = [
        ('Content-Type', 'application/json'),
        ('Content-Length', str(len(response_body)))
    ]

    start_response(status, response_headers)
    return [response_body.encode("utf-8")]


if __name__ == "__main__":
	httpd = make_server("0.0.0.0", 8000, application)
	print("Serving http on port 8000")
	httpd.serve_forever()

