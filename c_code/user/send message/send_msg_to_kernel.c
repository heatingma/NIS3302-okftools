#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>

#define NETLINK_TEST 29
#define MAX_PAYLOAD 1024

int pid;
int sock_fd;
FILE *logfile;
struct nlmsghdr *nlh = NULL;

void send_message_to_kernel(int flag, int type)
{
    struct sockaddr_nl src_addr, dest_addr;
    struct iovec iov;
    struct msghdr msg;
    char *buffer;
    unsigned int uid,flags,ret;
    char * file_path;
    char * commandname;

    sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_TEST);

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = pid;

    bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; /* For Linux Kernel */
    dest_addr.nl_groups = 0; /* unicast */

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));

    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = flag;
    nlh->nlmsg_type = type;

    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    memset(&msg, 0, sizeof(msg));
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    sendmsg(sock_fd, &msg, 0);
    close(sock_fd);
    if (nlh)
    {
    	free(nlh);
    	nlh = NULL;
    }
    return;
}


int main(int argc, char * argv[])
{
    int flag;
    int type;

    /* argv[1] */
    if (strcmp(argv[1], "hook") == 0) type = 0;
    else if (strcmp(argv[1], "restore") == 0) type = 1;

    /* argv[2] */
    if (strcmp(argv[2], "openat") == 0) flag = 1;
    else if (strcmp(argv[2], "unlink") == 0) flag = 2;
    else if (strcmp(argv[2], "execve") == 0) flag = 3;
    else if (strcmp(argv[2], "shutdown") == 0) flag = 4;
    else if (strcmp(argv[2], "reboot") == 0) flag = 5;
    else if (strcmp(argv[2], "finit_module") == 0) flag = 6;
    else if (strcmp(argv[2], "mount") == 0) flag = 7;
    else if (strcmp(argv[2], "umount2") == 0) flag = 8;
    else if (strcmp(argv[2], "mknodat") == 0) flag = 9;

    send_message_to_kernel(flag,type);

    return 0;
}
