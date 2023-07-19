import os
import shutil

ori_dir = os.getcwd()
target_dir = os.path.join(ori_dir,'okftools')

# netlink_kernel.ko
os.chdir('code_sources/kernel')
os.system("make")
cur_dir = os.getcwd()
filename = os.path.join(cur_dir,'netlink_kernel.ko')
shutil.copy(filename, target_dir)
os.chdir(ori_dir)

#clean_log
os.chdir("code_sources/user/clean_log")
os.makedirs("build", exist_ok=True)
os.chdir("build")
os.system("cmake ..")
os.system("make")
cur_dir = os.getcwd()
filename = os.path.join(cur_dir, 'clean_log')
shutil.copy(filename, target_dir)
os.chdir(ori_dir)

# recv_msg_from_kernel
os.chdir('code_sources/user/receive message')
os.makedirs("build", exist_ok=True)
os.chdir("build")
os.system("cmake ..")
os.system("make")
cur_dir = os.getcwd()
filename = os.path.join(cur_dir, 'recv_msg_from_kernel')
shutil.copy(filename, target_dir)
os.chdir(ori_dir)

# send_msg_to_kernel
os.chdir('code_sources/user/send message')
os.makedirs("build", exist_ok=True)
os.chdir("build")
os.system("cmake ..")
os.system("make")
cur_dir = os.getcwd()
filename = os.path.join(cur_dir, 'send_msg_to_kernel')
shutil.copy(filename, target_dir)
os.chdir(ori_dir)

# send_control_path
os.chdir('code_sources/user/send_control_path')
os.makedirs("build", exist_ok=True)
os.chdir("build")
os.system("cmake ..")
os.system("make")
cur_dir = os.getcwd()
filename = os.path.join(cur_dir, 'send_control_path')
shutil.copy(filename, target_dir)
os.chdir(ori_dir)
