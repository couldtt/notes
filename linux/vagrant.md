##vagrant问题解决
####问题：
命令补全的时候，会出现`bash: warning: setlocale: LC_CTYPE: cannot change locale (zh_CN.UTF-8)`的错误

####问题分析：
运行`locale`命令，发现`locale: Cannot set LC_CTYPE to default locale: No such file or directory`错误，
设置里面是：`LC_CTYPE=zh_CN.UTF-8`, 但是没有zh_CN.UTF-8这个文件，将其改成en_US.UTF-8就好了

####问题解决：
```
vi ~/.profile
```
加入`export "LC_CTYPE=en_US.UTF-8"`

```
source ~/.profile
```
