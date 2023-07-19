#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/unistd.h>
#include <linux/mm_types.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <asm/unistd.h>
#include <linux/utsname.h>
#include <asm/pgtable.h>
#include <linux/kallsyms.h>
#include <linux/kprobes.h>
#include <asm/uaccess.h>
#include <asm/ptrace.h>
#include <linux/limits.h>
#include <linux/ctype.h>
#include <linux/netlink.h>
#include <net/netlink.h>
#include <net/net_namespace.h>
#include <linux/time64.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/timekeeping.h>
#include <linux/fs_struct.h>

MODULE_AUTHOR("heatingma");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("the module to change function");
MODULE_ALIAS("hw");

/*----------------------------------* DEFINATION *------------------------------------*/



/* get the sys_call_table */
int ret;
unsigned int level;
pte_t *pte;
static struct kprobe kp = {
   .symbol_name = "sys_call_table"
};
static unsigned long **sys_call_table;
void get_sys_call_table(void);

/* utils function and variable*/
#define MAX_LENGTH 256
char control_path[128];
void get_fullname(const char *pathname,char *fullname);
char *get_cur_time(void);



/* hook the function openat */
int openat_state;
void handle_openat(int type);
void hook_openat(void);
void restore_openat(void);
asmlinkage long fake_openat(struct pt_regs *regs);
static unsigned long (*real_openat_addr)(struct pt_regs *regs);
static unsigned long (*fake_openat_addr)(struct pt_regs *regs);


/* hook the function unlink */
int unlink_state;
void handle_unlink(int type);
void hook_unlink(void);
void restore_unlink(void);
asmlinkage long fake_unlink(const char __user *pathname);
static unsigned long (*real_unlink_addr)(const char __user *);
static unsigned long (*fake_unlink_addr)(const char __user *);

/* hook the function execve */
int execve_state;
void handle_execve(int type);
void hook_execve(void);
void restore_execve(void);
asmlinkage long fake_execve(struct pt_regs *regs);
static unsigned long (*real_execve_addr)(struct pt_regs *regs);
static unsigned long (*fake_execve_addr)(struct pt_regs *regs);

/* hook the function shutdown */
int shutdown_state;
void handle_shutdown(int type);
void hook_shutdown(void);
void restore_shutdown(void);
asmlinkage long fake_shutdown(int sock, int howto);
static unsigned long (*real_shutdown_addr)(int sock, int howto);
static unsigned long (*fake_shutdown_addr)(int sock, int howto);

/* hook the function reboot */
int reboot_state;
void handle_reboot(int type);
void hook_reboot(void);
void restore_reboot(void);
asmlinkage long fake_reboot(struct pt_regs *regs);
static unsigned long (*real_reboot_addr)(struct pt_regs *regs);
static unsigned long (*fake_reboot_addr)(struct pt_regs *regs);

/* netlink */
#define NLMSG_MAX_PAYLOAD 1024
#define NETLINK_TEST 29
int can_send;
int user_pid;   
const struct cred *cred; 
struct sock *netlink_sock = NULL;
static void netlink_receive_message_handle(struct sk_buff *sock_buffer);
struct netlink_kernel_cfg cfg = {
    .input = netlink_receive_message_handle, /* set recv callback */
};
void send_message(char *msg);


/*------------------------------------* KERNEL *--------------------------------------*/


/* kernel_init */
static int __init netlink_kernel_init(void)
{
    char init_path[] = "/home/"; 
    printk(KERN_INFO "netlink_kernel_init\n");
    get_sys_call_table();
    can_send = 0;
    openat_state = 1;
    unlink_state = 1;
    execve_state = 1;
    shutdown_state = 1;
    reboot_state = 1;
    strcpy(control_path,init_path);
    netlink_sock = netlink_kernel_create(&init_net, NETLINK_TEST, &cfg);
    if (!netlink_sock) {
       printk(KERN_ALERT "Error creating socket.\n");
       return -10;
    }
    return 0;
}

/* kernel_exit */
static void __exit netlink_kernel_exit(void)
{
    if (openat_state == 0){
        openat_state = 1;
        restore_openat();
    }
    if (unlink_state == 0){
        unlink_state = 1;
        restore_unlink();
    }
    if (execve_state == 0){
        execve_state = 1;
        restore_execve();
    }
    if (shutdown_state == 0){
        shutdown_state = 1;
        restore_shutdown();
    }
    if (reboot_state == 0){
        reboot_state = 1;
        restore_reboot();
    }
    netlink_kernel_release(netlink_sock);
    printk(KERN_INFO "netlink_kernel_exit\n");
}

module_init(netlink_kernel_init);
module_exit(netlink_kernel_exit);

/*----------------------------------* GST_SYS_TABLE *-----------------------------------*/

void get_sys_call_table(void)
{
    sys_call_table = NULL;
    ret = -1;
    ret = register_kprobe(&kp);
    if(ret<0)
        printk(KERN_INFO "register_kprobe faild, error:%d\n",ret);
    sys_call_table = (unsigned long **)kp.addr;
    unregister_kprobe(&kp);
    printk(KERN_INFO "sys_call_table is at %lx\n", (long) sys_call_table);
    return;
}


/*--------------------------------------* UTILS *---------------------------------------*/

void get_fullname(const char *pathname,char *fullname)
{
	struct dentry *parent_dentry = current->fs->pwd.dentry;
    char buf[MAX_LENGTH];

	if (*(parent_dentry->d_name.name)=='/'){
	    strcpy(fullname,pathname);
	    return;
	}

	for(;;){
	    if (strcmp(parent_dentry->d_name.name,"/")==0)
            buf[0]='\0';//reach the root dentry.
	    else
	        strcpy(buf,parent_dentry->d_name.name);
        strcat(buf,"/");
        strcat(buf,fullname);
        strcpy(fullname,buf);

        if ((parent_dentry == NULL) || (*(parent_dentry->d_name.name)=='/'))
            break;

        parent_dentry = parent_dentry->d_parent;
	}

	strcat(fullname,pathname);
	return;
}

char *get_cur_time(void)
{
    char *cur_time = NULL;
    struct tm tm_time;
    time64_t now;

    now = ktime_get_real_seconds();
    time64_to_tm(now, 0, &tm_time);

    cur_time = kmalloc(20, GFP_KERNEL);
    if (cur_time != NULL) {
        sprintf(cur_time, "%04ld/%02d/%02d %02d:%02d:%02d",
                tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    }

    return cur_time;
}

int path_cmp(const char *str1, const char *str2)
{
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if (len1 < len2) {
        return 0;
    }
    return strncasecmp(str1, str2, len2) == 0;
}


/*---------------------------------------* OPENAT *------------------------------------*/

void handle_openat(int type)
{
    if (type == 0){
        if(openat_state == 1){
            openat_state = 0;
            hook_openat();
        }
    }
    else{
    	if(openat_state == 0){
            openat_state = 1;
            restore_openat();
        }
    }
}

asmlinkage long fake_openat(struct pt_regs *regs)
{
    char msg[300] = "OPENAT:";
    char buffer[200];
    long nbytes;
    bool valid_string;
    char * filename;
    char fullname[256];
    int flags;

    // char buffer_bx[100];
    // char buffer_cx[100];
    // char buffer_dx[100];

    filename = (char *)regs->si;
    flags = regs->dx;
    if ((flags & O_CREAT)== 0) return real_openat_addr(regs);

    nbytes = strncpy_from_user(buffer, filename, sizeof(buffer));
    if (nbytes <= 0) return real_openat_addr(regs);
    // printk(KERN_INFO "rsi=%s\n", buffer);

    // nbytes = strncpy_from_user(buffer_bx, (char *)regs->bx, sizeof(buffer_bx));
    // if (nbytes <= 0) return real_openat_addr(regs);
    // printk(KERN_INFO "rbx=%s\n", buffer_bx);
    
    // nbytes = strncpy_from_user(buffer_cx, (char *)regs->cx, sizeof(buffer_cx));
    // if (nbytes <= 0) return real_openat_addr(regs);
    // printk(KERN_INFO "rcx=%s\n", buffer_cx);

    // nbytes = strncpy_from_user(buffer_dx, (char *)regs->dx, sizeof(buffer_dx));
    // if (nbytes <= 0) return real_openat_addr(regs);
    // printk(KERN_INFO "rdx=%s\n", buffer_dx);


    buffer[nbytes] = '\n';
    buffer[nbytes + 1] = '\0';

    if (path_cmp(buffer,control_path) == 0) return real_openat_addr(regs);
    // printk(KERN_INFO "buffer is :%s\n",buffer);
    // printk(KERN_INFO "control_path is :%s\n",control_path);
    
    valid_string = true;
    for (int i = 0; i < nbytes; ++i) {
        if (!isprint(buffer[i]) && buffer[i] != '\0') {
            valid_string = false;
            break;
        }
    }
    
    if (valid_string && strcmp(buffer,"/dev/null") && strncmp(buffer,"log",3)) {
        if (buffer[0] != '/'){
            memset(fullname, 0, 256);
            get_fullname(buffer,fullname);
            strcat(msg,fullname);
        }
        else{
            strcat(msg,buffer);
        }
        send_message(msg);
    }
    return real_openat_addr(regs);
}

void hook_openat(void)
{
    char msg[100] = "HOOK:OPENAT\n";

    fake_openat_addr = (unsigned long (*)(struct pt_regs *regs)) fake_openat;
    real_openat_addr = (unsigned long (*)(struct pt_regs *regs))sys_call_table[__NR_openat];
    printk(KERN_INFO "real_open:%lx\n",(long)real_openat_addr);

    pte = lookup_address((unsigned long) sys_call_table, &level);
    set_pte_atomic(pte, pte_mkwrite(*pte));
    printk(KERN_INFO "Disable write-protection of page with sys_call_table\n");
    sys_call_table[__NR_openat] = (unsigned long *) fake_openat_addr;
    set_pte_atomic(pte, pte_clear_flags(*pte, _PAGE_RW));
    printk(KERN_INFO "Restart write-protection successfully\n");
    
    printk(KERN_INFO "the openat function has been successfully hooked\n");

    send_message(msg);
}

void restore_openat(void)
{
    char msg[100] = "RESTORE:OPENAT\n";

	pte = lookup_address((unsigned long) sys_call_table, &level);
    set_pte_atomic(pte, pte_mkwrite(*pte));
    printk(KERN_INFO "Disable write-protection of page with sys_call_table\n");
    sys_call_table[__NR_openat] = (unsigned long *) real_openat_addr;
    set_pte_atomic(pte, pte_clear_flags(*pte, _PAGE_RW));
    printk(KERN_INFO "Restart write-protection successfully!\n");
    
    printk(KERN_INFO "the openat function has been successfully restored\n");

    send_message(msg);
}


/*---------------------------------------* UNLINK *------------------------------------*/

void handle_unlink(int type)
{
    if (type == 0){
        if(unlink_state == 1){
            unlink_state = 0;
            hook_unlink();
        }
    }
    else{
    	if(unlink_state == 0){
            unlink_state = 1;
            restore_unlink();
        }
    }
}

asmlinkage long fake_unlink(const char __user *pathname)
{
    char msg[300] = "UNLINK:";
    char buffer[256];

    if (path_cmp(pathname,control_path) == 0) return real_unlink_addr(pathname);

    if (copy_from_user(buffer, pathname, sizeof(buffer))) {
        printk(KERN_ERR "Fail to copy pathname from user\n");
        return real_unlink_addr(pathname);
    }
    strcat(msg,buffer);
    send_message(msg);
    
    return real_unlink_addr(pathname);
}

void hook_unlink(void)
{
    char msg[100] = "HOOK:UNLINK\n";

    fake_unlink_addr = (unsigned long (*)(const char __user *pathname)) fake_unlink;
    real_unlink_addr = (unsigned long (*)(const char __user *pathname)) sys_call_table[__NR_unlink];
	printk(KERN_INFO "real_unlink%lx\n",(long)real_unlink_addr);

	pte = lookup_address((unsigned long) sys_call_table, &level);
    set_pte_atomic(pte, pte_mkwrite(*pte));
    printk(KERN_INFO "Disable write-protection of page with sys_call_table\n");
    sys_call_table[__NR_unlink] = (unsigned long *) fake_unlink_addr;
    set_pte_atomic(pte, pte_clear_flags(*pte, _PAGE_RW));
    printk(KERN_INFO "Restart write-protection successfully\n");
    
    printk(KERN_INFO "the unlink function has been successfully hooked\n");

    send_message(msg);
}

void restore_unlink(void)
{
    char msg[100] = "RESTORE:UNLINK\n";

    pte = lookup_address((unsigned long) sys_call_table, &level);
    set_pte_atomic(pte, pte_mkwrite(*pte));
    printk(KERN_INFO "Disable write-protection of page with sys_call_table\n");
    sys_call_table[__NR_unlink] = (unsigned long *) real_unlink_addr;
    set_pte_atomic(pte, pte_clear_flags(*pte, _PAGE_RW));
    printk(KERN_INFO "Restart write-protection successfully!\n");
    
    printk(KERN_INFO "the unlink function has been successfully restored\n");

    send_message(msg);
}

/*---------------------------------------* EXECVE *------------------------------------*/

void handle_execve(int type)
{
    if (type == 0){
        if(execve_state == 1){
            execve_state = 0;
            hook_execve();
        }
    }
    else{
    	if(execve_state == 0){
            execve_state = 1;
            restore_execve();
        }
    }
}

asmlinkage long fake_execve(struct pt_regs *regs)
{
    char msg[300] = "EXECVE:";
    uid_t uid;
    int memcpy_ret = -1;
    char filename[512]; // linux 下路经最长为4096byte

    uid = current_uid().val;

    // kernel 不能直接操作user space的内存，因此需要现复制
    // 如果成功返回0；如果失败，返回有多少个Bytes未完成copy
    memcpy_ret = copy_from_user(filename, (char *)(regs->di), 512);
    if(memcpy_ret != 0)
    {
        // 从用户态复制数据出错
        printk(KERN_ERR "fake_execve: Error copying data from user space\n");
    }
    else
    {
        printk(KERN_INFO "Program %s is executed by user %u\n", filename, uid);
        strcat(msg,filename);
        send_message(msg);
    }
    return real_execve_addr(regs);
}

void hook_execve(void)
{
    char msg[100] = "HOOK:EXECVE\n";

    fake_execve_addr = (unsigned long (*)(struct pt_regs *regs)) fake_execve;
    real_execve_addr = (unsigned long (*)(struct pt_regs *regs))sys_call_table[__NR_execve];
	printk(KERN_INFO "real_execve:%lx\n",(long)real_execve_addr);

	pte = lookup_address((unsigned long) sys_call_table, &level);
    set_pte_atomic(pte, pte_mkwrite(*pte));
    printk(KERN_INFO "Disable write-protection of page with sys_call_table\n");
    sys_call_table[__NR_execve] = (unsigned long *) fake_execve_addr;
    set_pte_atomic(pte, pte_clear_flags(*pte, _PAGE_RW));
    printk(KERN_INFO "Restart write-protection successfully\n");
    
    printk(KERN_INFO "the execve function has been successfully hooked\n");

    send_message(msg);
}

void restore_execve(void)
{
    char msg[100] = "RESTORE:EXECVE\n";

	pte = lookup_address((unsigned long) sys_call_table, &level);
    set_pte_atomic(pte, pte_mkwrite(*pte));
    printk(KERN_INFO "Disable write-protection of page with sys_call_table\n");
    sys_call_table[__NR_execve] = (unsigned long *) real_execve_addr;
    set_pte_atomic(pte, pte_clear_flags(*pte, _PAGE_RW));
    printk(KERN_INFO "Restart write-protection successfully!\n");
    
    printk(KERN_INFO "the execve function has been successfully restored\n");

    send_message(msg);
}


/*--------------------------------------* SHUTDOWN *-----------------------------------*/

void handle_shutdown(int type)
{
    if (type == 0){
        if(shutdown_state == 1){
            shutdown_state = 0;
            printk(KERN_INFO "will hook_shutdown\n");
            hook_shutdown();
        }
    }
    else{
    	if(shutdown_state == 0){
            shutdown_state = 1;
            restore_shutdown();
        }
    }
}

asmlinkage long fake_shutdown(int sock, int howto)
{
    char msg[300] = "SHUTDOWN:";
    strcat(msg,get_cur_time());
    // "SHUTDOWN:2020-01-10 "
    send_message(msg);
    printk(KERN_INFO "After 10 seconds, the computer will be shutdown\n");
    msleep(10000);
    return real_shutdown_addr(sock, howto);
}

void hook_shutdown(void)
{
    char msg[100] = "HOOK:SHUTDOWN\n";

    printk(KERN_INFO "in hook_shutdown\n");
    fake_shutdown_addr = (unsigned long (*)(int sock, int howto)) fake_shutdown;
    real_shutdown_addr = (unsigned long (*)(int sock, int howto)) sys_call_table[__NR_shutdown];
	printk(KERN_INFO "real_shutdown:%lx\n",(long)real_shutdown_addr);

	pte = lookup_address((unsigned long) sys_call_table, &level);
    set_pte_atomic(pte, pte_mkwrite(*pte));
    printk(KERN_INFO "Disable write-protection of page with sys_call_table\n");
    sys_call_table[__NR_shutdown] = (unsigned long *) fake_shutdown_addr;
    set_pte_atomic(pte, pte_clear_flags(*pte, _PAGE_RW));
    printk(KERN_INFO "Restart write-protection successfully\n");
    
    printk(KERN_INFO "the shutdown function has been successfully hooked\n");

    send_message(msg);
}

void restore_shutdown(void)
{
    char msg[100] = "RESTORE:SHUTDOWN\n";

    pte = lookup_address((unsigned long) sys_call_table, &level);
    set_pte_atomic(pte, pte_mkwrite(*pte));
    printk(KERN_INFO "Disable write-protection of page with sys_call_table\n");
    sys_call_table[__NR_shutdown] = (unsigned long *) real_shutdown_addr;
    set_pte_atomic(pte, pte_clear_flags(*pte, _PAGE_RW));
    printk(KERN_INFO "Restart write-protection successfully!\n");
    
    printk(KERN_INFO "the shutdown function has been successfully restored\n");

    send_message(msg);
}


/*--------------------------------------* REBOOT *-----------------------------------*/

void handle_reboot(int type)
{
    if (type == 0){
        if(reboot_state == 1){
            reboot_state = 0;
            hook_reboot();
        }
    }
    else{
    	if(reboot_state == 0){
            reboot_state = 1;
            restore_reboot();
        }
    }
}

asmlinkage long fake_reboot(struct pt_regs *regs)
{
    char msg[300] = "REBOOT: reboot time is ";
    strcat(msg,get_cur_time());
    send_message(msg);
    printk(KERN_INFO "After 10 seconds, the computer will be reboot\n");
    msleep(10000);
	
    return real_reboot_addr(regs);
}

void hook_reboot(void)
{
    char msg[100] = "HOOK:REBOOT\n";

    fake_reboot_addr = (unsigned long (*)(struct pt_regs *regs)) fake_reboot;
    real_reboot_addr = (unsigned long (*)(struct pt_regs *regs)) sys_call_table[__NR_reboot];
	printk(KERN_INFO "real_reboot:%lx\n",(long)real_reboot_addr);

	pte = lookup_address((unsigned long) sys_call_table, &level);
    set_pte_atomic(pte, pte_mkwrite(*pte));
    printk(KERN_INFO "Disable write-protection of page with sys_call_table\n");
    sys_call_table[__NR_reboot] = (unsigned long *) fake_reboot_addr;
    set_pte_atomic(pte, pte_clear_flags(*pte, _PAGE_RW));
    printk(KERN_INFO "Restart write-protection successfully\n");
    
    printk(KERN_INFO "the reboot function has been successfully hooked\n");

    send_message(msg);
}

void restore_reboot(void)
{
    char msg[100] = "RESTORE:REBOOT\n";

    pte = lookup_address((unsigned long) sys_call_table, &level);
    set_pte_atomic(pte, pte_mkwrite(*pte));
    printk(KERN_INFO "Disable write-protection of page with sys_call_table\n");
    sys_call_table[__NR_reboot] = (unsigned long *) real_reboot_addr;
    set_pte_atomic(pte, pte_clear_flags(*pte, _PAGE_RW));
    printk(KERN_INFO "Restart write-protection successfully!\n");
    
    printk(KERN_INFO "the reboot function has been successfully restored\n");

    send_message(msg);
}


/*--------------------------------------* NETLINK *------------------------------------*/

static void netlink_receive_message_handle(struct sk_buff *sock_buffer) 
{          
    int flag;
    int type;
    struct nlmsghdr *netlink_head;
    char *msg = "NETLINK:Hello from kernel\n";
    netlink_head = (struct nlmsghdr*) sock_buffer->data;
    
    flag = netlink_head->nlmsg_flags;
    type = netlink_head->nlmsg_type;
    printk(KERN_INFO "Receive the message from user: flag = %d, type = %d \n",flag,type);
    switch (flag)
    {
        case 1:
            handle_openat(type);
            break;
        case 2:
            handle_unlink(type);
            break;
        case 3:
            handle_execve(type);
            break;
        case 4:
            printk(KERN_INFO "will handle shutdown\n");
            handle_shutdown(type);
            break;
        case 5:
            handle_reboot(type);
            break;
        case 98:
            strcpy(control_path, (char *)nlmsg_data(netlink_head));
            break;
        case 99:
            can_send = 0;
            break;
        case 100:
            user_pid = netlink_head->nlmsg_pid;
            can_send = 1;
            break;
        default:
            break;
    }
    send_message(msg);
}

void send_message(char *msg)
{
    int back_pid;
    int back_uid;
    char str_pid[10];
    char str_uid[10];
    char combined_msg[800];
    struct nlmsghdr *netlink_head;
    struct sk_buff *sock_buffer_out;        
    int msg_size;                                  
    int res;

    if (can_send == 0) {
        return;
    }

    cred = current_cred();
    back_pid = current->pid;
    back_uid = cred->uid.val;
    snprintf(str_pid, sizeof(str_pid), "%d", back_pid);
    snprintf(str_uid, sizeof(str_uid), "%d", back_uid);

    sprintf(combined_msg, "%s|%s|%s", msg, str_uid, str_pid);
    msg_size = strlen(combined_msg);

    sock_buffer_out = nlmsg_new(msg_size, GFP_KERNEL);
    if (!sock_buffer_out) {
        printk(KERN_ERR "Failed to allocate new sock_buffer\n");
        return;
    }

    netlink_head = nlmsg_put(sock_buffer_out, 0, 0, NLMSG_DONE, msg_size, 0);
    NETLINK_CB(sock_buffer_out).dst_group = 0;
    strncpy(nlmsg_data(netlink_head), combined_msg, msg_size);

    res = nlmsg_unicast(netlink_sock, sock_buffer_out, user_pid);
    if (res < 0) {
        printk(KERN_ERR "Error while sending back to user\n");
    }
}


