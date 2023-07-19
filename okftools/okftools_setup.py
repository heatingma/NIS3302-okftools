import os
import shutil

ori_dir = os.getcwd()

os.chdir('netlink_kernel')
os.system("make")
cur_dir = os.getcwd()
filename = os.path.join(cur_dir,'netlink_kernel.ko')
shutil.copy(filename, ori_dir)
os.chdir(ori_dir)
