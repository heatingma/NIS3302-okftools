### 项目名称：

        基于系统调用重载的系统级资源访问审计

### 项目团队

        麻家乐 王梓睿 孙靖轩 张天烁 汪何希

### 主要工作

        1. 获取系统调用表 sys_call_table。
        2. 实现系统函数重载和地址写入。
        3. 实现内核和用户层间的信息交互。
        4. 将信息保存为日志及尝试相关扩展处理。
        5. 实现了图形化界面，提供可视化操作方便用户使用。

### 开发环境

        Ubuntu 22.04  linux-5.19

### 使用方法
        1.pip install okftools
<div><center>
<img src=docs/pip.png width=70% height=70% >
<br>
<strong><font face="仿宋" size=2>图1 pip install okftools</font>
</strong>
</center></div>

        2.pip show okftools
<div><center>
<img src=docs/show.png width=70% height=70% >
<br>
<strong><font face="仿宋" size=2>图2 pip show okftools</font>
</strong>
</center></div>

        3.cd the okftools path

        4.python okftools_setup.py or python3 okftools_setup.py 
<div><center>
<img src=docs/setup.png width=70% height=70% >
<br>
<img src=docs/pyinstaller.png width=70% height=70% >
<br>
<strong><font face="仿宋" size=2>图4 python okftools_setup.py</font>
</strong>
</center></div>

        5. input the following command to use okftools
                ./oktfools
<div><center>
<img src=docs/okftools.png width=70% height=70% >
<br>
<strong><font face="仿宋" size=2>图5 oktfools</font>
</strong>
</center></div>


        * If the following error occurs:
                libGL error: failed to load driver: swrast.
                libGL error: failed to load driver: vmwgfx
        * please try:
                conda install -c conda-forge gcc=12.1.0
        