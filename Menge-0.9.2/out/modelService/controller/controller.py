# coding:utf-8
from flask import Blueprint, request
from modelService.model import model

controller = Blueprint('controller', __name__)


# 开始指定类型的程序仿真，与menge建立socket连接，启动后对menge参数进行初始化
# http://127.0.0.1:5001/prob
@controller.route('/prob',methods=['POST'])
def simStart():
    dataString = request.values.get("data")
    dataList = dataString.split('-')
    dataList = [int(num) for num in dataList]
    print(dataList)
    probList = model(dataList)
    res = ""
    for num in probList:
        res = res + str(num) +" "
    return res
