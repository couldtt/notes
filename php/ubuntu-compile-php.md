##Ubuntu下PHP编译安装遇到的一些问题

####ubuntu compile PHP(the essential packages)

######编译环境

```
sudo apt-get install build-essential 
```

######xml

```sudo apt-get install libxml2-dev```

######pcre

```sudo apt-get install libpcre3-dev```

######jpeg

```sudo apt-get install libjpeg62-dev```

######freetype

```sudo apt-get install libfreetype6-dev```

######png

```sudo apt-get install libpng12-dev libpng3 libpnglite-dev```

######iconv

```sudo apt-get install libiconv-hook-dev libiconv-hook1```

######mycrypt

```sudo apt-get install libmcrypt-dev libmcrypt4```

######mhash

```sudo apt-get install libmhash-dev libmhash2```

######openssl

```sudo apt-get install libltdl-dev libssl-dev```

######curl

```sudo apt-get install libcurl4-openssl-dev```

######mysql

```sudo apt-get install libmysqlclient-dev```

######imagick

```sudo apt-get install libmagickcore-dev libmagickwand-dev```

######ubuntu 无法找到 iconv
```
sudo ln -s /usr/lib/libiconv_hook.so.1.0.0 /usr/lib/libiconv.so

sudo ln -s /usr/lib/libiconv_hook.so.1.0.0 /usr/lib/libiconv.so.1
```
######安装PHP扩展

```sudo apt-get install autoconf automake m4```


####一些小错误的修正
- configure: error: Please reinstall the BZip2 distribution
  - `sudo apt-get install libbz2-dev`

- configure: error: cURL version 7.10.5 or later is required to compile php with cURL support
  - `sudo apt-get install curl libcurl4-openssl-dev`
  - `sudo apt-get install php5-curl`

- Cannot find MySQL header files
  - `apt-get install libmysqlclient15-dev`

- configure: error: SNMP sanity check failed. Please check config.log for more information.
  - `apt-get install libsnmp-dev`

