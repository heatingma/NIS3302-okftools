#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>

#define NETLINK_TEST 29
#define MAX_PAYLOAD 1024

int main(int argc, char * argv[])
{
    char * control_path;
    int pid;
    int sock_fd;
    struct nlmsghdr *nlh = NULL;
    struct sockaddr_nl src_addr, dest_addr;
    struct iovec iov;
    struct msghdr msg;

    pid = getpid();
    control_path = argv[1];

    sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_TEST);
    if (sock_fd < 0) {
        printf("Error creating socket\n");
        return -1;
    }

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
    nlh->nlmsg_pid = pid;
    nlh->nlmsg_flags = 98;

    strcpy(NLMSG_DATA(nlh), control_path);

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
    return 0;
}
