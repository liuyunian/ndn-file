# ndn-file
介绍：通过NDN网络进行文件传输，分成server和client两部分
* server: 发布可供下载的文件
* client: 下载所需的文件

## Version 1
* server只能发布一个文件，文件的数据是通过标准输入读取的 
```
Usage: 
    build/server -p /prefix/fileName.xx < filePath/fileName.xx
eg: 
    build/server -p /ndn/test.txt < /home/lyn/test/test.txt
```

* client下载文件，文件通过Name的最后一部分命名，存储路径是ndn-file目录下，不能指定目录
```
Usage: 
    buile/client -p /prefix/fileName.xx
eg: 
    build/client -p /ndn/test.txt #下载的文件名是test.txt
```

## Version 2
* server 发布指定目录下的文件，目录下可以有多个文件，但是不能有子目录
```
Usage:
    build/server -p /prefix -d shareFilePath
        -p: 默认值: /localhost
        -d: 默认值: ndn-file目录下的test目录
eg:
    build/server -p /ndn -d /home/lyn/test/
```

* client 下载指定的文件,可以指定下载之后文件存储路径
```
Usage:
    build/client -p /prefix/fileName.xx -d /savePath
        -p: 无默认值，必须指定
        -d: 默认值: ndn-file目录下的download目录
eg:
    build/client -p /ndn/test.txt -d /home/lyn/桌面
```
## Version 3
* 更新了Client，增加了界面操作
```
Usage:
    build/client进入界面，输入节点名称和前缀
    之后可以获得节点分享的文件列表
    点击下载按钮即可下载
```

## Repair Bug 1
修复不能传输大文件的问题
* server端不再是直接将文件全部读入内存,而是由请求驱动,一部分一部分的读取文件到内存中,并封装成Data
* client不再一次发送全部的Interest, 而是批量发送,这样保证每一个interst都不会超时,另外收到的Data包中的数据改为缓存之后一并写入文件,提高了效率

## Version 4
* 更新了client界面
* 支持多文件同时下载
* 改为接受到data之后再发送下一个segment的interest, 增加了程序的稳定性