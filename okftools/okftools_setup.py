import os
import shutil

ori_dir = os.getcwd()

#netlink_kernel.ko
os.chdir('netlink_kernel')
os.system("make")
cur_dir = os.getcwd()
filename = os.path.join(cur_dir,'netlink_kernel.ko')
shutil.copy(filename, ori_dir)
os.chdir(ori_dir)

#clean_log
os.chdir("user/clean_log")
os.makedirs("build", exist_ok=True)
os.chdir("build")
os.system("cmake ..")
os.system("make")
cur_dir = os.getcwd()
filename = os.path.join(cur_dir, 'clean_log')
shutil.copy(filename, ori_dir)
os.chdir(ori_dir)

# recv_msg_from_kernel
os.chdir('user/receive message')
os.makedirs("build", exist_ok=True)
os.chdir("build")
os.system("cmake ..")
os.system("make")
cur_dir = os.getcwd()
filename = os.path.join(cur_dir, 'recv_msg_from_kernel')
shutil.copy(filename, ori_dir)
os.chdir(ori_dir)

# send_msg_to_kernel
os.chdir('user/send message')
os.makedirs("build", exist_ok=True)
os.chdir("build")
os.system("cmake ..")
os.system("make")
cur_dir = os.getcwd()
filename = os.path.join(cur_dir, 'send_msg_to_kernel')
shutil.copy(filename, ori_dir)
os.chdir(ori_dir)

# send_control_path
os.chdir('user/send_control_path')
os.makedirs("build", exist_ok=True)
os.chdir("build")
os.system("cmake ..")
os.system("make")
cur_dir = os.getcwd()
filename = os.path.join(cur_dir, 'send_control_path')
shutil.copy(filename, ori_dir)
os.chdir(ori_dir)

# log_control
os.chdir('user/log_control')
os.makedirs("build", exist_ok=True)
os.chdir("build")
os.system("cmake ..")
os.system("make")
cur_dir = os.getcwd()
filename = os.path.join(cur_dir, 'log_control')
shutil.copy(filename, ori_dir)
os.chdir(ori_dir)

# okftools
os.chdir('GUI')
filename = os.path.join(os.getcwd(),'okftools.py')
os.system("python -m venv test_env")
os.system("python3 -m venv test_env")
os.chdir("test_env")
shutil.copy(filename, os.getcwd())
os.system("source bin/activate")
os.system("pip install pygame")
os.system("pip install pyinstaller")
os.system("pyinstaller --onefile okftools.py")
filename = os.path.join(os.getcwd(),'dist/okftools')
shutil.copy(filename, ori_dir)
os.chdir(ori_dir)