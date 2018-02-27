// for server
#include <iostream>
#include <string>
//for protobuf
#include "protocolbufTest.pb.h"
//for zmq
#include <zmq.h>
#include <unistd.h>

using namespace std;
using namespace protocolbufTest::protobuf ;

const int BUFFSIZE = 128;

int main()
{
    //使用ZMQ进行通信，首先要创建一个上下文环境，然后使用它创建套接字。
    void *context = zmq_ctx_new(); //create a context
    
    //根据context建立的socket的链接，服务端使用ZMQ_REP套接字
    void *responder = zmq_socket(context, ZMQ_REP); // for server
    
    //服务端将socket绑定到一个周知的地址和端口
    if( -1 == zmq_bind(responder,"tcp://*:5555"))
    {
        cout<<"bind socket to server failed..."<<endl;
        return -1;
    }

    Information myprotobuf;
    while(1)
    {
        char buff[BUFFSIZE];
        //接收客户端请求
        //要把数据写入消息需要使用zmq_msg_init_size()来初始化消息
        //而读取消息由于未知消息的长度只能使用zmq_msg_init()来创建一个空的消息。
        zmq_msg_t request;
        zmq_msg_init(&request);  //initialise empty 0MQ message
        int size = zmq_msg_recv(&request,responder,0); //receive a message part from a socket
        memcpy(buff,zmq_msg_data(&request),size); //zmq_msg_data - retrieve pointer to message content
        Information receive;
        receive.ParseFromArray(buff,BUFFSIZE); //Parse a protocol buffer contained in an array of bytes
        cout<<"Type:"<<receive.type()<<"\t"
            <<"IP:"<<receive.ip()<<"\t"
            <<"Port:"<<receive.port()<<"\n";
        zmq_msg_close(&request); //release 0MQ message

        //清空接收缓存
        memset(buff,0,BUFFSIZE*sizeof(char));
        sleep(2); //停2秒

        myprotobuf.set_type("server");
        myprotobuf.set_ip("192.168.1.100");
        myprotobuf.set_port(5555);
        myprotobuf.SerializeToArray(buff,BUFFSIZE);

        //服务端发送响应
        int len = strlen(buff);
        zmq_msg_t reply;
        if(0 != zmq_msg_init_size(&reply,len)) //zmq_msg_init_size - initialise 0MQ message of a specified size
        {
            cout<<"zmq_msg_init failed..."<<endl;
            break;
        }
        memcpy(zmq_msg_data(&reply),buff,len);
        if(len != zmq_msg_send(&reply,responder,0)) //zmq_msg_send - send a message part on a socket
        {
            zmq_msg_close(&reply); //release 0MQ message
            cout<<"send faliled..."<<endl;
            break;
        }
        //成功发送后，在控制台打印发送消息的内容
        cout<<"Type:"<<myprotobuf.type()<<"\t"
            <<"IP:"<<myprotobuf.ip()<<"\t"
            <<"Port:"<<myprotobuf.port()<<"\n";
        zmq_msg_close(&reply); //release 0MQ message
    }
    
    //最后需要关闭套接字，并销毁上下文
    zmq_close(&responder); //关闭套接字
    zmq_ctx_destroy(context); //销毁上下文
    return 0;
}

