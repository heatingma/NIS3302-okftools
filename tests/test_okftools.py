import os
from utils import cmds
import time
ori_dir = os.getcwd()

os.chdir('okftools')
shell = cmds()

shell.insmod()

shell.hook_openat()
shell.hook_unlink()
shell.hook_execve()
shell.hook_shutdown()
shell.hook_reboot()
shell.hook_finit_module()
shell.hook_mount()
shell.hook_umount2()
shell.hook_mknodat()

time.sleep(1)

shell.restore_openat()
shell.restore_unlink()
shell.restore_execve()
shell.restore_shutdown()
shell.restore_reboot()
shell.restore_finit_module()
shell.restore_mount()
shell.restore_umount2()
shell.restore_mknodat()

shell.rmmod()

os.chdir(ori_dir)
