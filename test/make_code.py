import os
import shutil

ori_dir = os.getcwd()
target_dir = 'okftools'

# netlink_kernel.ko
os.chdir('c_code/kernel')
os.system("make")
cur_dir = os.getcwd()
filename = os.path.join(cur_dir,'netlink_kernel.ko')
shutil.copy(filename, target_dir)
os.chdir(ori_dir)

#clean_log
os.chdir("c_code/user/clean_log")
os.system("mkdir build")
os.system("cd build")
os.system("cmake ..")
os.system("make")
cur_dir = os.getcwd()
filename = os.path.join(cur_dir,'clean_log')
shutil.copy(filename, target_dir)
os.chdir(ori_dir)

#recv_msg_from_kernel
os.chdir('c_code/user/receive message')
os.system("mkdir build")
os.system("cd build")
os.system("cmake ..")
os.system("make")
cur_dir = os.getcwd()
filename = os.path.join(cur_dir,'recv_msg_from_kernel')
shutil.copy(filename, target_dir)
os.chdir(ori_dir)

#send_msg_to_kernel
os.chdir('c_code/user/send message')
os.system("mkdir build")
os.system("cd build")
os.system("cmake ..")
os.system("make")
cur_dir = os.getcwd()
filename = os.path.join(cur_dir,'send_msg_to_kernel')
shutil.copy(filename, target_dir)
os.chdir(ori_dir)

#send_conrtol_path
os.chdir('c_code/user/send_conrtol_path')
os.system("mkdir build")
os.system("cd build")
os.system("cmake ..")
os.system("make")
cur_dir = os.getcwd()
filename = os.path.join(cur_dir,'send_conrtol_path')
shutil.copy(filename, target_dir)
os.chdir(ori_dir)
