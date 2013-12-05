##PHP编译安装遇到的一些问题

####ubuntu下
- configure: error: Please reinstall the BZip2 distribution
  - `sudo apt-get install libbz2-dev`

- configure: error: cURL version 7.10.5 or later is required to compile php with cURL support
  - `sudo apt-get install curl libcurl4-openssl-dev`
  - `sudo apt-get install php5-curl`

- Cannot find MySQL header files
  - `apt-get install libmysqlclient15-dev`
