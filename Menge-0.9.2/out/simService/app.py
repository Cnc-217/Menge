#coding:utf-8

from flask import Flask
from flask_apscheduler import APScheduler
from simService.controller.controller import controller


class Config(object):
    JOBS = [
        {
            'id': 'redisCollect',                # 一个标识
            'func': 'simService.services.radisService:dataCollectToRedis',     # 指定运行的函数
            'args': (),              # 传入函数的参数
            'trigger': 'interval',       # 指定 定时任务的类型
            'seconds': 5                # 运行的间隔时间
        }
    ]
    SCHEDULER_API_ENABLED = True


if __name__ == '__main__':
    app = Flask(__name__)
    app.register_blueprint(controller, url_prefix="/")
    app.config.from_object(Config())
    scheduler = APScheduler()  # 实例化 APScheduler
    scheduler.init_app(app)  # 把任务列表放入 flask
    scheduler.start()  # 启动任务列表
    app.run('10.28.195.233', 5000)

