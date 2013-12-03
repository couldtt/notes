####Linux下一些极其零碎的小笔记
- **gdebi** 命令可以在安装deb包的时候自动安装以来，免去了`sudo apt-get -f install`的过程

- zsh
  - 使用[oh-my-zsh](https://github.com/robbyrussell/oh-my-zsh)
  - 设置`zsh`为默认的shell： `chsh -s /bin/zsh`

- **id_rsa** 只有权限在600的时候，才有权限连接远程服务器,不能太OPEN了
