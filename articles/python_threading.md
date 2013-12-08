##python数据采集与多线程效率分析

以前一直使用PHP写爬虫，用`Snoopy`配合`simple_html_dom`用起来也挺好的，至少能够解决问题。

PHP一直没有一个好用的多线程机制，虽然可以使用一些trick的手段来实现并行的效果（例如借助apache或者nginx服务器等，或者fork一个子进程，或者直接动态生成多个PHP脚本多进程运行），但是无论从代码结构上，还是从使用的复杂程度上，用起来都不是那么顺手。还听说过一个[pthreads](https://github.com/krakjoe/pthreads)的PHP的扩展，这是一个真正能够实现PHP多线程的扩展，看github上它的介绍：Absolutely, this is not a hack, we don't use forking or any other such nonsense, what you create are honest to goodness posix threads that are completely compatible with PHP and safe ... this is true multi-threading :)

扯远了，PHP的内容在本文中不再赘述，既然决定尝试一下Python的采集，同时一定要学习一下Python的多线程知识的。以前一直听各种大牛们将Python有多么多么好用，不真正用一次试试，自己也没法明确Python具体的优势在哪，处理哪些问题用Python合适。

废话就说这么多吧，进入正题

- 采集目标：淘宝
- 采集数据：某一关键词领域的淘宝店铺名称、URL地址、店铺等级
- 用到的第三方packages：
  - requests（话说是看了前两天的一篇文章 **Python modules you should know** [(传送门)](http://devcharm.com/pages/11-python-modules-you-should-know) 才知道的，以前只知道urllib2）
  - BeautifulSoup（现在貌似有新版本bs4了，不过我用的是旧版本的）
  - Redis

####采集
#####单线程版本
######代码：
`search_config.py`
```
#!/usr/bin/env python
# coding=utf-8
class config:
    keyword = '青岛'
    search_type = 'shop'
    url = 'http://s.taobao.com/search?q=' + keyword + '&commend=all&search_type='+ search_type +'&sourceId=tb.index&initiative_id=tbindexz_20131207&app=shopsearch&s='

```

`single_scrapy.py`
```
#!/usr/bin/env python
# coding=utf-8
import requests
from search_config import config

class Scrapy():

    def __init__(self, threadname, start_num):
        self.threadname = threadname
        self.start_num = start_num
        print threadname + 'start.....'


    def run(self):
        url = config.url + self.start_num
        response = requests.get(url)
        print self.threadname + 'end......'

def main():
    for i in range(0,13,6):
        scrapy = Scrapy('scrapy', str(i))
        scrapy.run()


if __name__ == '__main__':
    main()

```

######运行分析：
![截图](http://i.imm.io/1lOmu.png)
这是最简单最常规的一种采集方式，按照顺序循环进行网络连接，获取页面信息。看截图可知，这种方式的效率其实是极低的，一个连接进行网络I/O的时候，其他的必须等待前面的连接完成才能进行连接，换句话说，就是前面的连接阻塞的后面的连接。

#####多线程版本
######代码：
```
#!/usr/bin/env python
# coding=utf-8
import requests
from search_config import config
import threading

class Scrapy(threading.Thread):

    def __init__(self, threadname, start_num):
        threading.Thread.__init__(self, name = threadname)
        self.threadname = threadname
        self.start_num = start_num
        print threadname + 'start.....'

    #重写Thread类的run方法
    def run(self):
        url = config.url + self.start_num
        response = requests.get(url)
        print self.threadname + 'end......'

dpef main():
    for i in range(0,13,6):
        scrapy = Scrapy('scrapy', str(i))
        scrapy.start()


if __name__ == '__main__':
    main()
```

######运行分析：
![截图](http://i.imm.io/1lOmL.png)
通过截图可以看到，采集同样数量的页面，通过开启多线程，时间缩短了很多，但是CPU利用率高了。


####页面信息解析
html页面信息拿到以后，我们需要对其进行解析操作，从中提取出我们所需要的信息，包含：

- 店铺名称
- 店铺URL
- 店铺等级

使用BeautifulSoup这个库，可以直接按照class或者id等html的attr来进行提取，比直接写正则直观不少，难度也小了很多，当然，执行效率上，相应的也就大打折扣了。

######代码：
- 这里使用`Queue`实现一个**生产者**和**消费者**模式
  - 生产者消费者模式：
   - 生产者将数据依次存入队列，消费者依次从队列中取出数据。
   - 本例中，通过scrapy线程不断提供数据，parse线程从队列中取出数据进行相应解析

- Queue模块
  - Python中的Queue对象也提供了对线程同步的支持，使用Queue对象可以实现多个生产者和多个消费者形成的FIFO的队列。
  - 当共享信息需要安全的在多线程之间交换时，Queue非常有用。
  - Queue的默认长度是无限的，但是可以设置其构造函数的maxsize参数来设定其长度。

```
#!/usr/bin/env python
# coding=utf-8
import requests
from BeautifulSoup import BeautifulSoup
from search_config import config

from Queue import Queue
import threading

class Scrapy(threading.Thread):

    def __init__(self, threadname, queue, out_queue):
        threading.Thread.__init__(self, name = threadname)
        self.sharedata = queue
        self.out_queue= out_queue
        self.threadname = threadname
        print threadname + 'start.....'


    def run(self):
        url = config.url + self.sharedata.get()
        response = requests.get(url)
        self.out_queue.put(response)
        print self.threadname + 'end......'

class Parse(threading.Thread):
    def __init__(self, threadname, queue, out_queue):
        threading.Thread.__init__(self, name = threadname)
        self.sharedata = queue
        self.out_queue= out_queue
        self.threadname = threadname
        print threadname + 'start.....'
        
    def run(self):
        response = self.sharedata.get()
        body = response.content.decode('gbk').encode('utf-8')
        soup = BeautifulSoup(body)
        ul_html = soup.find('ul',{'id':'list-container'})
        lists = ul_html.findAll('li',{'class':'list-item'})
        stores = []
        for list in lists:
            store= {}
            try:
                infos = list.findAll('a',{'trace':'shop'})
                for info in infos:
                    attrs = dict(info.attrs)
                    if attrs.has_key('class'):
                        if 'rank' in attrs['class']:
                            rank_string = attrs['class']
                            rank_num = rank_string[-2:]
                            if (rank_num[0] == '-'):
                                store['rank'] = rank_num[-1]
                            else:
                                store['rank'] = rank_num
                    if attrs.has_key('title'):
                        store['title'] = attrs['title']
                        store['href'] = attrs['href']
            except AttributeError:
                pass
            if store:
                stores.append(store)

        for store in stores:
            print store['title'] + ' ' + store['rank']
        print self.threadname + 'end......'

def main():
    queue = Queue()
    targets = Queue()
    stores = Queue()
    scrapy = []
    for i in range(0,13,6):
	#queue 原始请求
	#targets 等待解析的内容
	#stores解析完成的内容，这里为了简单直观，直接在线程中输出了内容，并没有使用该队列
        queue.put(str(i))
        scrapy = Scrapy('scrapy', queue, targets)
        scrapy.start()
        parse = Parse('parse', targets, stores)
        parse.start()

if __name__ == '__main__':
    main()

```
######运行结果
![结果](http://i.imm.io/1lOvE.png)
看这个运行结果，可以看到，我们的scrapy过程很快就完成了，我们的parse也很早就开始了，可是在运行的时候，却卡在parse上好长时间才出的运行结果，每一个解析结果出现，都需要3～5秒的时间，虽然我用的是台老IBM破本，但按理说使用了多线程以后不应该会这么慢的啊。

同样的数据，我们再看一下单线程下，运行结果。这里为了方便，我在上一个multi_scrapy里加入了redis，使用redis存储爬行下来的原始页面，这样在single_parse.py里面可以单独使用，更方便一些。

#####单线程版本：
######代码：
```
#!/usr/bin/env python
# coding=utf-8
from BeautifulSoup import BeautifulSoup
import redis


class Parse():
    def __init__(self, threadname, content):
        self.threadname = threadname
        self.content = content
        print threadname + 'start.....'
        
    def run(self):
        response = self.content
        if response:
            body = response.decode('gbk').encode('utf-8')
            soup = BeautifulSoup(body)
            ul_html = soup.find('ul',{'id':'list-container'})
            lists = ul_html.findAll('li',{'class':'list-item'})
            stores = []
            for list in lists:
                store= {}
                try:
                    infos = list.findAll('a',{'trace':'shop'})
                    for info in infos:
                        attrs = dict(info.attrs)
                        if attrs.has_key('class'):
                            if 'rank' in attrs['class']:
                                rank_string = attrs['class']
                                rank_num = rank_string[-2:]
                                if (rank_num[0] == '-'):
                                    store['rank'] = rank_num[-1]
                                else:
                                    store['rank'] = rank_num
                        if attrs.has_key('title'):
                            store['title'] = attrs['title']
                            store['href'] = attrs['href']
                except AttributeError:
                    pass
                if store:
                    stores.append(store)

            for store in stores:
                try:
                    print store['title'] + ' ' + store['rank']
                except KeyError:
                    pass
            print self.threadname + 'end......'
        else:
            pass

def main():
    r = redis.StrictRedis(host='localhost', port=6379)
    while True:
        content = r.lpop('targets')
        if (content):
            parse = Parse('parse', content)
            parse.run()
        else:
            break

if __name__ == '__main__':
    main()

```
######运行结果：
![result](http://i.imm.io/1lOBU.png)
结果可以看到，单线程版本中，耗时其实和多线程是差不多的，上文中的多线程版本，虽然包含了获取页面的时间，但是地一个例子里我们已经分析了，使用多线程以后，三个页面的抓取，完全可以在1s内完成的，也就是说，使用多线程进行数据解析，并没有获得实质上的效率提高。

####分析原因
1. 看两个运行的CPU占用，第一个127%,第二个98%,都是非常高的，这说明，在处理字符串解析匹配提取等运算密集型的工作时，并行的概念并没有很好得得到发挥
2. 由于共享数据不存在安全问题，所以上面的例子都是非线程安全的，并没有为共享数据加锁，只是实现了最简单的FIFO，所以也不会是因为锁的开销导致效率没有得到真正提高
3. 网上搜索资料，发现**python多线程似乎并不能利用多核**，问题似乎就是出在这里了，在python上开启多个线程，由于GIL的存在，每个单独线程都会在竞争到GIL后才运行，这样就干预OS内部的进程(线程)调度，结果在多核CPU上，python的多线程实际是串行执行的，并不会同一时间多个线程分布在多个CPU上运行。Python由于有全锁局的存在（同一时间只能有一个线程执行），并不能利用多核优势。所以，如果你的多线程进程是CPU密集型的，那多线程并不能带来效率上的提升，相反还可能会因为线程的频繁切换，导致效率下降；如果是IO密集型，多线程进程可以利用IO阻塞等待时的空闲时间执行其他线程，提升效率。
4. 问题答案：**由于数据解析操作是CPU密集型的操作，而网络请求是I/O密集型操作，所以出现了上述结果。**

####解决方法
GIL既然是针对一个python解释器进程而言的，那么，如果解释器可以**多进程**解释执行，那就不存在GIL的问题了。同样，他也不会导致你多个解释器跑在同一个核上。
所以，最好的解决方案，是**多线程+多进程**结合。通过多线程来跑I/O密集型程序，通过控制合适数量的进程来跑CPU密集型的操作，这样就可以跑慢CPU了:)
