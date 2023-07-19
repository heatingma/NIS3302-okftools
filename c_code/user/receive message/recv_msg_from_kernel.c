#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <pwd.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/select.h>

#define TM_FMT "%Y-%m-%d %H:%M:%S"
#define NETLINK_TEST 29
#define MAX_PAYLOAD 1024

int pid;
int sock_fd;
FILE *logfile;
struct nlmsghdr *nlh = NULL;
void remove_newlines(char *str);
void clear_output(void);
void Log(char *commandname, int uid, int pid, char *file_path);

void remove_newlines(char *str) {
    int i, j = 0;
    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] != '\n') {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
}

void killdeal_func() {

    struct sockaddr_nl src_addr, dest_addr;
    struct iovec iov;
    struct msghdr msg;

    printf("Received SIGUSR1 signal\n");
    printf("The process is killed! \n");

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
    nlh->nlmsg_flags = 99;

    strcpy(NLMSG_DATA(nlh), "Say goodbye from user!\n");

    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    memset(&msg, 0, sizeof(msg));
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    sendmsg(sock_fd, &msg, 0);

    fflush(stdout);
    close(sock_fd);
    if (logfile != NULL)
        fclose(logfile);
    if (nlh) {
        free(nlh);
        nlh = NULL;
    }
    exit(0);
}

void Log(char *commandname, int recv_uid, int recv_pid, char *file_path) {
    char logtime[64];
    char username[32];
    struct passwd *pwinfo;
    char openresult[10];
    char save_msg[1024];

    logfile = fopen("log.txt", "a+");
    if (logfile == NULL) {
        printf("Warning: can not open log file\n");
        exit(1);
    }

    time_t t = time(0);
    pwinfo = getpwuid(recv_uid);
    if (pwinfo == NULL) {
        printf("Warning: can not get user info\n");
        fclose(logfile);
        return;
    }
    strcpy(username, pwinfo->pw_name);

    strftime(logtime, sizeof(logtime), TM_FMT, localtime(&t));
    sprintf(save_msg, "%s %d %s %d %s %s", username, recv_uid, commandname, recv_pid, logtime, file_path);
    remove_newlines(save_msg);
    fprintf(logfile, "%s\n", save_msg);
    fclose(logfile);
}

void clear_output(void) {
    FILE *fp = fopen("log", "w");
    if (fp == NULL) {
        printf("ERROR: Cannot clear the file: log\n");
        return;
    }
    fclose(fp);
}

void save_buffer(char *buffer, int recv_uid, int recv_pid) {
    char commandname[100];
    char file_path[500];
    char *token;
    char *delim = ":";
    int len;
    token = strtok(buffer, delim);
    if (token == NULL) {
        // printf("Warning: invalid command format\n");
        return;
    }
    strncpy(commandname, token, sizeof(commandname) - 1);
    commandname[sizeof(commandname) - 1] = '\0';

    token = strtok(NULL, delim);
    if (token == NULL) {
        // printf("Warning: invalid command format\n");
        return;
    }
    strncpy(file_path, token, sizeof(file_path) - 1);
    file_path[sizeof(file_path) - 1] = '\0';

    len = strlen(file_path);
    if (len > 0 && file_path[len - 1] == '\n') {
        file_path[len - 1] = '\0';
    }

    Log(commandname, recv_uid, recv_pid, file_path);
}

int main()
{
    struct sockaddr_nl src_addr, dest_addr;
    struct iovec iov;
    struct msghdr msg;
    char recv_msg[1024];
    int recv_uid;
    int recv_pid;
    char *combined_msg;
    char *token, *saveptr;

    pid = getpid();
    signal(SIGUSR1,killdeal_func);
    signal(SIGINT,killdeal_func);
    
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
    nlh->nlmsg_flags = 100;

    strcpy(NLMSG_DATA(nlh), "Hello from user!\n");

    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    memset(&msg, 0, sizeof(msg));
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    sendmsg(sock_fd, &msg, 0);
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    while (1) {
        recvmsg(sock_fd, &msg, 0);
        combined_msg = (char *)NLMSG_DATA(nlh);

        token = strtok_r(combined_msg, "|", &saveptr);
        strncpy(recv_msg, token, sizeof(recv_msg));

        token = strtok_r(NULL, "|", &saveptr);
        recv_uid = atoi(token);

        token = strtok_r(NULL, "|", &saveptr);
        recv_pid = atoi(token);

        // nlh = NLMSG_NEXT(nlh, msg.msg_namelen);
        save_buffer(recv_msg, recv_uid, recv_pid);
    }
    close(sock_fd);
    if (nlh)
    {
        free(nlh);
        nlh = NULL;
    }
    return 0;
}
