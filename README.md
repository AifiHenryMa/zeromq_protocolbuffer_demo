# zeromq_protocolbuffer_demo

这是一个 使用ZeroMQ(简称ZMQ),结合protobuf序列化实现客户端和服务端的通信demo

第一步：生成.cc 和 头文件 protoc --cpp_out=. protocolbufTest.proto 在当前目录下可以看到多了 protocolbufTest.pb.cc protocolbufTest.pb.h 这两个文件

第二步：将protocolbufTest.pb.cc protocolbufTest.pb.h添加到工程中

第三步：别忘了build的时候连接上libprotobuf.so 库 和libzmq.so 库

参考了 http://blog.csdn.net/cjf_wei/article/details/61628552 这篇文章
