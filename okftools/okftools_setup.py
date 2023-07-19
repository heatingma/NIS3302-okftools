import os
import shutil

ori_dir = os.getcwd()

os.chdir('netlink_kernel')
os.system("make")
cur_dir = os.getcwd()
filename = os.path.join(cur_dir,'netlink_kernel.ko')
shutil.copy(filename, ori_dir)
os.chdir(ori_dir)

os.chdir('GUI')
filename = os.path.join(os.getcwd(),'okftools.py')

os.system("python -m venv test_env")
os.chdir("test_env")

os.system("source bin/activate")
os.system("pip install pygame")
os.system("pip install pyinstaller")
os.system("pyinstaller --onefile okftools.py")

filename = os.path.join(os.getcwd(),'dist/okftools')
shutil.copy(filename, ori_dir)

os.chdir(ori_dir)
