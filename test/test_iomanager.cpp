#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#include "../log/log.h"
#include "../iomanager.h"

static int m_close_log = 0;

int sockfd;

void watch_io_read();

// 写事件 回调，只执行一次，判断非阻塞socket是否connect成功
void do_io_write() {
    LOG_DEBUG("%s", "write callback");
    int so_err;
    socklen_t len = size_t(so_err);
    getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &so_err, &len);
    if (so_err) {
        LOG_DEBUG("do_io_write:: %s", "connect fail");
        return ;
    }
    LOG_DEBUG("do_io_write:: %s", "connect success");
}

// 读事件 回调， 每次读取之后如果socket未关闭，需要重新添加
void do_io_read() {
    LOG_DEBUG("do_io_read::%s", "read callback");
    char buf[1024] = {0};
    int readLen = 0;
    readLen = read(sockfd, buf, sizeof(buf));
    if (readLen > 0) {
        buf[readLen] = '\0';
        LOG_DEBUG("do_io_read, read n=%d bytes, read: %s", readLen, buf);
    } else if (readLen == 0) {
        LOG_DEBUG("%s", "do_io_read, peer closed");
        close(sockfd);
        return ;
    } else {
        LOG_ERROR("do_io_read, ERROR, errno=%s", strerror(errno));
        close(sockfd);
        return ;
    }
    IOManager::GetThis()->schedule(watch_io_read); //注意这里还是在read事件当中，不应该直接addEvent否则就是重复添加事件了
    //IOManager::GetThis()->addEvent(sockfd, IOManager::READ, do_io_read);
}

void watch_io_read() {
    LOG_DEBUG("%s", "watch_io_read START");
    IOManager::GetThis()->addEvent(sockfd, IOManager::READ, do_io_read);
}

void test_io() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    LOG_ASSERT(sockfd > 0);
    fcntl(sockfd, F_SETFL, O_NONBLOCK); //设置了socket IPV4 TCP 非阻塞模式

    sockaddr_in servaddr; //定义网络主机的协议、地址和端口
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr.s_addr); //把ip地址转化为用于网络传输的二进制数值 搭配netcat进行测试

    int rt = connect(sockfd, (const sockaddr *) &servaddr, sizeof(servaddr)); //将fd连接到网络主机中
    if (rt != 0) {
        if (errno == EINPROGRESS) {
            LOG_INFO("test_io:: %s", "EINPROGRESS");
            // 非阻塞TCP socket的connet一般无法立即建立连接，需要通过socket可写判断是否connect成功
            IOManager::GetThis()->addEvent(sockfd, IOManager::WRITE, do_io_write);
            IOManager::GetThis()->addEvent(sockfd, IOManager::READ, do_io_read);
        } else {
            LOG_ERROR("test_io:: %s, errno: %s", "connect error", strerror(errno));
        }

    } else {
        LOG_ERROR("test_io, else error, errno: %s", strerror(errno));
    }
}

void test_manager() {
    // IOManager iom;
    IOManager iom(10, true);
    iom.schedule(test_io);
}

int main() {
    Log::get_instance()->init("./IOManagerLog", 0, 2000, 800000, 800);
    LOG_DEBUG("%s", "IOManager Test Begin");
    test_manager();
    LOG_DEBUG("%s", "IOManager Test End");
    sleep(1);
}

