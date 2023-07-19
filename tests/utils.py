import subprocess
import time
import os
import signal

class cmds:
    def __init__(self):
        self.procs = dict()
        self.pids = dict()

    def get_psw(self, psw):
        self.psw = psw

    def run_command(self, command, command_name):
        proc = subprocess.Popen(command, stdin=subprocess.PIPE, universal_newlines=True)
        time.sleep(0.1)
        self.pids[command_name] = proc.pid
        self.procs[command_name] = proc
        time.sleep(0.1)

    def clean(self):
        command = ['./clean_log']
        self.run_command(command, 'clean')

    def insmod(self):
        command = ['sudo', 'insmod', 'netlink_kernel.ko']
        self.run_command(command, 'insmod')
        self.recv_msg_from_kernel()
        
    def recv_msg_from_kernel(self):
        command = ['./recv_msg_from_kernel']
        self.run_command(command, 'recv_msg')
        
    def rmmod(self):
        os.kill(self.pids['recv_msg'], signal.SIGUSR1)
        time.sleep(0.5)
        for i in range(len(self.pids)):
            try:
                os.system("sudo kill -9 {}".format(self.pids[i]))
            except:
                pass

        command = ['sudo', 'rmmod', 'netlink_kernel']
        self.run_command(command, 'rmmod')
    
    def path_control(self,path):
        command = ['./send_control_path', path]
        self.run_command(command, 'control_path')        
    
    def hook_openat(self):
        command = ['./send_msg_to_kernel', 'hook', 'openat']
        self.run_command(command, 'hook_openat')

    def restore_openat(self):        
        command = ['./send_msg_to_kernel', 'restore', 'openat']
        self.run_command(command, 'restore_openat')

    def hook_unlink(self):
        command = ['./send_msg_to_kernel', 'hook', 'unlink']
        self.run_command(command, 'hook_unlink')

    def restore_unlink(self):
        command = ['./send_msg_to_kernel', 'restore', 'unlink']
        self.run_command(command, 'restore_unlink')

    def hook_execve(self):
        command = ['./send_msg_to_kernel', 'hook', 'execve']
        self.run_command(command, 'hook_execve')

    def restore_execve(self):
        command = ['./send_msg_to_kernel', 'restore', 'execve']
        self.run_command(command, 'restore_execve')
        
    def hook_shutdown(self):
        command = ['./send_msg_to_kernel', 'hook', 'shutdown']
        self.run_command(command, 'hook_shutdown')

    def restore_shutdown(self):
        command = ['./send_msg_to_kernel', 'restore', 'shutdown']
        self.run_command(command, 'restore_shutdown')

    def hook_reboot(self):
        command = ['./send_msg_to_kernel', 'hook', 'reboot']
        self.run_command(command, 'hook_reboot')

    def restore_reboot(self):
        command = ['./send_msg_to_kernel', 'restore', 'reboot']
        self.run_command(command, 'restore_reboot')

    def hook_finit_module(self):
        command = ['./send_msg_to_kernel', 'hook', 'finit_module']
        self.run_command(command, 'hook_finit_module')

    def restore_finit_module(self):
        command = ['./send_msg_to_kernel', 'restore', 'finit_module']
        self.run_command(command, 'restore_finit_module')

    def hook_mount(self):
        command = ['./send_msg_to_kernel', 'hook', 'mount']
        self.run_command(command, 'hook_mount')

    def restore_mount(self):
        command = ['./send_msg_to_kernel', 'restore', 'mount']
        self.run_command(command, 'restore_mount')

    def hook_umount2(self):
        command = ['./send_msg_to_kernel', 'hook', 'umount2']
        self.run_command(command, 'hook_umount2')

    def restore_umount2(self):
        command = ['./send_msg_to_kernel', 'restore', 'umount2']
        self.run_command(command, 'restore_umount2')
        
    def hook_mknodat(self):
        command = ['./send_msg_to_kernel', 'hook', 'mknodat']
        self.run_command(command, 'hook_mknodat')

    def restore_mknodat(self):
        command = ['./send_msg_to_kernel', 'restore', 'mknodat']
        self.run_command(command, 'restore_mknodat')
        
    def test_log(self):
        command = ['./log_control']
        self.run_command(command, 'log_control')
        
        
        self.procs['log_control'].stdin.write("sort 6\n")
        self.procs['log_control'].stdin.flush()
        time.sleep(0.1)
        print('testing sort 6')
        time.sleep(1)
        
        self.procs['log_control'].stdin.write("search /home\n")
        self.procs['log_control'].stdin.flush()
        time.sleep(0.1)
        print('testing search /home/test.txt')
        time.sleep(5)
  
        self.procs['log_control'].stdin.write('merge\n')
        self.procs['log_control'].stdin.flush()
        time.sleep(0.1)
        print('testing merge')
        time.sleep(5)
        
        self.procs['log_control'].stdin.write('reload\n')
        self.procs['log_control'].stdin.flush()
        time.sleep(0.1)
        print('testing reload')
        time.sleep(5)
        
        self.procs['log_control'].stdin.write('clear\n')
        self.procs['log_control'].stdin.flush()
        time.sleep(0.1)
        print('testing clear')
        time.sleep(5)
        
        self.procs['log_control'].stdin.write('exit\n')
        self.procs['log_control'].stdin.flush()
        time.sleep(1)
        
        

