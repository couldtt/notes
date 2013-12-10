#!/usr/bin/env python
# coding=utf-8
import requests
from search_config import config
import threading
import redis
import math

class Scrapy(threading.Thread):

    def __init__(self, threadname, start_num):
        threading.Thread.__init__(self, name = threadname)
        self.r = redis.StrictRedis(host='localhost', port=6379)
        self.threadname = threadname
        self.start_num = start_num
        print threadname + 'start.....'


    def run(self):
        url = config.url + self.start_num
        response = requests.get(url)
        self.r.rpush('targets', response.content)
        print self.threadname + 'end......'

def main():
    total = 600
    per_threading = 100
    threading_num = int(math.ceil(total / per_threading))
    for i in [x+1 for x in range(threading_num)]:
        end = int(i*per_threading)
        start = int((i-1) * per_threading)
        for j in range(start,end,6):
            scrapy = Scrapy('scrapy', str(i))
            scrapy.start()


if __name__ == '__main__':
    main()
