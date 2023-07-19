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


/* hook the function finit_module */
int finit_module_state;
void handle_finit_module(int type);
void hook_finit_module(void);
void restore_finit_module(void);
asmlinkage long fake_finit_module(struct pt_regs *regs);
static unsigned long (*real_finit_module_addr)(struct pt_regs *regs);
static unsigned long (*fake_finit_module_addr)(struct pt_regs *regs);


/* hook the function mount */
int mount_state;
void handle_mount(int type);
void hook_mount(void);
void restore_mount(void);
asmlinkage long fake_mount(char __user *dev_name, char __user *dir_name,
				char __user *type, unsigned long flags,
				void __user *data);
static unsigned long (*real_mount_addr)(char __user *dev_name, char __user *dir_name,
				char __user *type, unsigned long flags,
				void __user *data);
static unsigned long (*fake_mount_addr)(char __user *dev_name, char __user *dir_name,
				char __user *type, unsigned long flags,
				void __user *data);

/* hook the function umount2 */
int umount2_state;
void handle_umount2(int type);
void hook_umount2(void);
void restore_umount2(void);
asmlinkage long fake_umount2(const char *target, int flags);
static unsigned long (*real_umount2_addr)(const char *target, int flags);
static unsigned long (*fake_umount2_addr)(const char *target, int flags);


/* hook the function mknodat*/
int mknodat_state;
void handle_mknodat(int type);
void hook_mknodat(void);
void restore_mknodat(void);
asmlinkage int fake_mknodat(struct pt_regs *reg);
static unsigned int (*real_mknodat_addr)(struct pt_regs *reg);
static unsigned int (*fake_mknodat_addr)(struct pt_regs *reg);


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
    finit_module_state = 1;
    mount_state = 1;
    umount2_state = 1;
    mknodat_state = 1;
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
    if (finit_module_state == 0){
        finit_module_state = 1;
        restore_finit_module();
    }
    if (mount_state == 0){
        mount_state = 1;
        restore_mount();
    }
    if (umount2_state == 0){
        umount2_state = 1;
        restore_umount2();
    }
    if (mknodat_state == 0){
        mknodat_state = 1;
        restore_mknodat();
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
    int i;
    filename = (char *)regs->si;
    flags = regs->dx;
    if ((flags & O_CREAT)== 0) return real_openat_addr(regs);

    nbytes = strncpy_from_user(buffer, filename, sizeof(buffer));
    if (nbytes <= 0) return real_openat_addr(regs);

    buffer[nbytes] = '\n';
    buffer[nbytes + 1] = '\0';

    if (path_cmp(buffer,control_path) == 0) return real_openat_addr(regs);
    
    valid_string = true;
    for (i = 0; i < nbytes; ++i) {
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
	char tmp[256];
    char filename[256];

    uid = current_uid().val;


    memcpy_ret = copy_from_user(tmp, (char *)(regs->di), 256);
	memset(filename, 0, 256);
	get_fullname(tmp, filename);
    if(memcpy_ret != 0)
    {
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


/*-----------------------------------* FINIT_MODULE *---------------------------------*/

void handle_finit_module(int type)
{
    if (type == 0){
        if(finit_module_state == 1){
            finit_module_state = 0;
            hook_finit_module();
        }
    }
    else{
    	if(finit_module_state == 0){
            finit_module_state = 1;
            restore_finit_module();
        }
    }
}

void Int_To_Str(int x,char *Str)
{ 
 int t; 
 char *Ptr,Buf[5]; 
 int i = 0; 
 Ptr = Str; 
 if(x < 10)  // 当整数小于10，转换为0x格式 
 {  
   *Ptr ++ = '0';  
   *Ptr ++ = x+0x30;  
 } 
 else 
 {  
   while(x > 0)  
 {   
   t = x % 10;   
   x = x / 10;   
   Buf[i++] = t+0x30; // 通过计算把数字编成ASCII码形式  
 }  
 i -- ;  
 for(;i >= 0;i --)   // 将得到的字符串倒序  
 {   
   *(Ptr++) = Buf[i];  
 } 
} 
 *Ptr = '\0';
}

asmlinkage long fake_finit_module(struct pt_regs *regs)
{
    char msg[300] = "FINIT_MODULE:A new module is initialized, fd ";
    uid_t uid = current_uid().val;
    unsigned int fd = regs->di;
    char Msg[20]="";   
    Int_To_Str(fd,Msg);    
    printk(KERN_INFO "Module initialized by user: %u, fd: %u\n", uid, fd);
    strcat(msg,Msg);
    send_message(msg);
    return real_finit_module_addr(regs);
}

void hook_finit_module(void)
{
    char msg[100] = "HOOK:FINIT_MODULE\n";

    fake_finit_module_addr = (unsigned long (*)(struct pt_regs *regs)) fake_finit_module;
    real_finit_module_addr = (unsigned long (*)(struct pt_regs *regs)) sys_call_table[__NR_finit_module];
	printk(KERN_INFO "real_finit_module:%lx\n",(long)real_finit_module_addr);

	pte = lookup_address((unsigned long) sys_call_table, &level);
    set_pte_atomic(pte, pte_mkwrite(*pte));
    printk(KERN_INFO "Disable write-protection of page with sys_call_table\n");
    sys_call_table[__NR_finit_module] = (unsigned long *) fake_finit_module_addr;
    set_pte_atomic(pte, pte_clear_flags(*pte, _PAGE_RW));
    printk(KERN_INFO "Restart write-protection successfully\n");
    
    printk(KERN_INFO "the finit_module function has been successfully hooked\n");

    send_message(msg);
}

void restore_finit_module(void)
{
    char msg[100] = "RESTORE:FINIT_MODULE\n";

    pte = lookup_address((unsigned long) sys_call_table, &level);
    set_pte_atomic(pte, pte_mkwrite(*pte));
    printk(KERN_INFO "Disable write-protection of page with sys_call_table\n");
    sys_call_table[__NR_finit_module] = (unsigned long *) real_finit_module_addr;
    set_pte_atomic(pte, pte_clear_flags(*pte, _PAGE_RW));
    printk(KERN_INFO "Restart write-protection successfully!\n");
    
    printk(KERN_INFO "the finit_module function has been successfully restored\n");

    send_message(msg);
}


/*--------------------------------------* MOUNT *-------------------------------------*/

void handle_mount(int type){
    if (type == 0){
        if(mount_state == 1){
            mount_state = 0;
            hook_mount();
        }
    }
    else{
    	if(mount_state == 0){
            mount_state = 1;
            restore_mount();
        }
    }
}

asmlinkage long fake_mount(char __user *dev_name, char __user *dir_name,
                            char __user *type, unsigned long flags,
                            void __user *data) {
    char msg[300] = "MOUNT:";
    char dev_path[256];
    char dir_path[256];
    unsigned long len1;
    unsigned long len2;
    len1 = strncpy_from_user(dev_path, dev_name,256);
    len2 = strncpy_from_user(dir_path, dir_name, 256);
    strcat(msg,dev_path);
    strcat(msg,"is_at_");
    strcat(msg,dir_path);
    printk(KERN_INFO "%s is mounted at %s\n", dev_path, dir_path);
    send_message(msg);
    return real_mount_addr(dev_name, dir_name, type, flags, data);
}

void hook_mount(void)
{
    char msg[100] = "HOOK:MOUNT\n";
    fake_mount_addr = (unsigned long (*)(char __user *dev_name, char __user *dir_name,
				char __user *type, unsigned long flags,
				void __user *data)) fake_mount;
    real_mount_addr = (unsigned long (*)(char __user *dev_name, char __user *dir_name,
				char __user *type, unsigned long flags,
				void __user *data)) sys_call_table[__NR_mount];

    printk(KERN_INFO "real_mount%lx\n",(long)real_mount_addr);

    pte = lookup_address((unsigned long)sys_call_table,&level);
    set_pte_atomic(pte,pte_mkwrite(*pte));
    printk(KERN_INFO "Disable write-protection of page with sys_call_table\n");
    sys_call_table[__NR_mount] = (unsigned long *) fake_mount_addr;
    set_pte_atomic(pte,pte_clear_flags(*pte, _PAGE_RW));
    printk(KERN_INFO "Restart write-protection successfully\n");

    printk(KERN_INFO "the mount function has been successfully hooked\n");
    send_message(msg);
}

void restore_mount(void)
{
    char msg[100] = "RESTORE:MOUNT\n";
    pte = lookup_address((unsigned long) sys_call_table, &level);
    set_pte_atomic(pte, pte_mkwrite(*pte));
    printk(KERN_INFO "Disable write-protection of page with sys_call_table\n");
    sys_call_table[__NR_mount] = (unsigned long *) real_mount_addr;
    set_pte_atomic(pte, pte_clear_flags(*pte, _PAGE_RW));
    printk(KERN_INFO "Restart write-protection successfully!\n");
    
    printk(KERN_INFO "the mount function has been successfully restored\n");
    send_message(msg);
}


/*-------------------------------------* UNMOUNT2 *------------------------------------*/

void handle_umount2(int type)
{
    if (type == 0){
        if(umount2_state == 1){
            umount2_state = 0;
            hook_umount2();
        }
    }
    else{
    	if(umount2_state == 0){
            umount2_state = 1;
            restore_umount2();
        }
    }
}

asmlinkage long fake_umount2(const char *target, int flags){
    char msg[300] = "UMOUNT2:";
    char pathname[256];
    unsigned long len;
    if (target != NULL && target[0] != '\0') { // 判断 target 是否为空
        printk(KERN_INFO "%s is unmounted", target);
    }
    len = strncpy_from_user(pathname, target, 256);
    strcat(msg,pathname);
    send_message(msg);
    return real_umount2_addr(target, flags);
}

void hook_umount2(void)
{
    char msg[100] = "HOOK:UMOUNT2\n";
    fake_umount2_addr = (unsigned long (*)(const char *target,int flags )) fake_umount2;
    real_umount2_addr = (unsigned long (*)(const char *target,int flags)) sys_call_table[__NR_umount2];

    printk(KERN_INFO "real_umount2 : %lx\n",(long)real_umount2_addr);

    pte = lookup_address((unsigned long)sys_call_table,&level);
    set_pte_atomic(pte,pte_mkwrite(*pte));
    printk(KERN_INFO "Disable write-protection of page with sys_call_table\n");
    sys_call_table[__NR_umount2] = (unsigned long *) fake_umount2_addr;
    set_pte_atomic(pte,pte_clear_flags(*pte, _PAGE_RW));
    printk(KERN_INFO "Restart write-protection successfully\n");

    printk(KERN_INFO "the umount2 function has been successfully hooked\n");
    send_message(msg);
}

void restore_umount2(void)
{
    char msg[100] = "RESTORE:UMOUNT2\n";
    pte = lookup_address((unsigned long) sys_call_table, &level);
    set_pte_atomic(pte, pte_mkwrite(*pte));
    printk(KERN_INFO "Disable write-protection of page with sys_call_table\n");
    sys_call_table[__NR_umount2] = (unsigned long *) real_umount2_addr;
    set_pte_atomic(pte, pte_clear_flags(*pte, _PAGE_RW));
    printk(KERN_INFO "Restart write-protection successfully!\n");
    printk(KERN_INFO "the unmount function has been successfully restored\n");
    send_message(msg);
}


/*-------------------------------------* MKNODAT *------------------------------------*/

void handle_mknodat(int type)
{
    if (type == 0){
        if(mknodat_state == 1){
            mknodat_state = 0;
            hook_mknodat();
        }
    }
    else{
    	if(mknodat_state == 0){
            mknodat_state = 1;
            restore_mknodat();
        }
    }
}

asmlinkage int fake_mknodat(struct pt_regs *reg){
            char msg[300] = "MKNODAT:";
            char pathname[256];
            unsigned long len = strncpy_from_user(pathname, (const char __user *) reg->si, 256);

            if (len > 0) {
                pathname[len] = '\0'; 
                printk(KERN_INFO "pathname: %s\n", pathname); 
            } 
            else {
                printk(KERN_ERR "Failed to copy pathname from user space\n");
            }

            printk(KERN_INFO "mknod:mkdir number %d device is created successfully at %s\n",(int)reg->di,pathname);
            strcat(msg,pathname);
            send_message(msg);
            return real_mknodat_addr(reg);
          }

void hook_mknodat(void)
{
    char msg[100] = "HOOK:MKNODAT\n";
    fake_mknodat_addr = (unsigned int (*)(struct pt_regs *reg)) fake_mknodat;
    real_mknodat_addr = (unsigned int (*)(struct pt_regs *reg)) sys_call_table[__NR_mknodat];

    printk(KERN_INFO "real_mknodat is :%lx\n",(long)real_mknodat_addr);

    pte = lookup_address((unsigned long)sys_call_table,&level);
    set_pte_atomic(pte,pte_mkwrite(*pte));
    printk(KERN_INFO "Disable write-protection of page with sys_call_table\n");
    sys_call_table[__NR_mknodat] = (unsigned long *) fake_mknodat_addr;
    set_pte_atomic(pte,pte_clear_flags(*pte, _PAGE_RW));
    printk(KERN_INFO "Restart write-protection successfully\n");

    printk(KERN_INFO "the mknod function has been successfully hooked\n");
    send_message(msg);
}

void restore_mknodat(void)
{
    char msg[100] = "RESTORE:MKNODAT\n";
    pte = lookup_address((unsigned long) sys_call_table, &level);
    set_pte_atomic(pte, pte_mkwrite(*pte));
    printk(KERN_INFO "Disable write-protection of page with sys_call_table\n");
    sys_call_table[__NR_mknodat] = (unsigned long *) real_mknodat_addr;
    set_pte_atomic(pte, pte_clear_flags(*pte, _PAGE_RW));
    printk(KERN_INFO "Restart write-protection successfully!\n");
    
    printk(KERN_INFO "the mknod function has been successfully restored\n");
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
            handle_shutdown(type);
            break;
        case 5:
            handle_reboot(type);
            break;
        case 6:
            handle_finit_module(type);
            break;
        case 7:
            handle_mount(type);
            break;
        case 8:
            handle_umount2(type);
            break;
        case 9:
            handle_mknodat(type);
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
