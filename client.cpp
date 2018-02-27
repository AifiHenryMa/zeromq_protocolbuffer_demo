//for client
#include <iostream>
#include <string>
//for protobuf
#include "protocolbufTest.pb.h" 
//for zmq
#include <zmq.h>

using namespace std;
using namespace protocolbufTest::protobuf ;

const int BUFFSIZE = 128;

int main()
{
    //socket通信所需的上下文环境
    void *context = zmq_ctx_new(); //create a context
    
    //根据context建立的socket的链接，客户端使用ZMQ_REQ套接字
    void *requester = zmq_socket(context, ZMQ_REQ); // for client
    
    //客户端要尝试连接到服务端提供的地址
    if( -1 == zmq_connect(requester,"tcp://localhost:5555"))
    {
        cout<<"Connect to server failed..."<<endl;
        zmq_ctx_destroy(context);
        return -1;
    }
    cout<<"Connect to server success..."<<endl;

    Information myprotobuf;
    while(1)
    {
        myprotobuf.set_type("client");
        myprotobuf.set_ip("192.168.1.100");
        myprotobuf.set_port(5555);

        char buff[BUFFSIZE];
        myprotobuf.SerializeToArray(buff,BUFFSIZE);

        //客户端发送请求
        int len = strlen(buff);
        zmq_msg_t req;
        if(0 != zmq_msg_init_size(&req,len))
        {
            cout<<"zmq_msg_init failed..."<<endl;
            break;
        }
        memcpy(zmq_msg_data(&req),buff,len);
        if(len != zmq_msg_send(&req,requester,0))
        {
            zmq_msg_close(&req);
            cout<<"send faliled..."<<endl;
            break;
        }
        //成功发送后，在控制台打印发送消息的内容
        cout<<"Type:"<<myprotobuf.type()<<"\t"
            <<"IP:"<<myprotobuf.ip()<<"\t"
            <<"Port:"<<myprotobuf.port()<<"\n";
        zmq_msg_close(&req);

        //清空发送缓存
        memset(buff,0,BUFFSIZE*sizeof(char));

        //客户端接收来自服务端的相应
        zmq_msg_t reply;
        zmq_msg_init(&reply);
        int size = zmq_msg_recv(&reply,requester,0);
        memcpy(buff,zmq_msg_data(&reply),size);
        Information receive;
        receive.ParseFromArray(buff,BUFFSIZE);
        cout<<"Type:"<<receive.type()<<"\t"
            <<"IP:"<<receive.ip()<<"\t"
            <<"Port:"<<receive.port()<<"\n";
        zmq_msg_close(&reply);
    }
    zmq_close(&requester);
    zmq_ctx_destroy(context);
    return 0;
}
