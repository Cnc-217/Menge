#coding:utf-8

from flask import Flask
from modelService.controller.controller import controller



if __name__ == '__main__':
    app = Flask(__name__)
    app.register_blueprint(controller, url_prefix="/")
    app.run('127.0.0.1', 5001)

