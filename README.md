# ndn-file
介绍：通过NDN网络进行文件传输，分成server和client两部分
* server: 发布可供下载的文件
* client: 下载所需的文件

## Version 1
* server只能发布一个文件，文件的数据是通过标准输入读取的 
```
Usage: build/server -p /prefix/fileName.xx < filePath/fileName.xx
eg: build/server -p /ndn/test.txt < /home/lyn/test/test.txt
```

* client下载文件，文件通过Name的最后一部分命名，不能指定名称，存储路径是ndn-file目录下，不能制定目录
```
Usage: buile/client -p /prefix/fileName.xx
eg: build/client -p /ndn/test.txt #下载的文件名是test.txt
```