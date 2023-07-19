import os
import glob
import shutil

ori_dir = os.getcwd()
target_dir = 'okftools'

# netlink_kernel.ko
os.chdir('c_code/kernel')
os.system("make")
shutil.copy('/netlink_kernel.ko', target_dir)
os.chdir(ori_dir)

#clean_log
os.chdir('c_code/user/clean_log/build')
os.system("make")
shutil.copy('clean_log', target_dir)
os.chdir(ori_dir)

#recv_msg_from_kernel
os.chdir('c_code/user/receive message/build')
os.system("make")
shutil.copy('recv_msg_from_kernel', target_dir)
os.chdir(ori_dir)

#send_msg_to_kernel
os.chdir('c_code/user/send message/build')
os.system("make")
shutil.copy('send_msg_to_kernel', target_dir)
os.chdir(ori_dir)

#send_conrtol_path
os.chdir('c_code/user/send_conrtol_path/build')
os.system("make")
shutil.copy('send_conrtol_path', target_dir)
os.chdir(ori_dir)
