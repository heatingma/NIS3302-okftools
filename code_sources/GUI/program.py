import os
import pygame as pg
import sys
import subprocess
# import concurrent.futures
import signal
# import psutil
import time
from datetime import datetime



#############################################################
####                    BASIC FUNCTION                   ####
#############################################################

def check_event(button:list,input:list=None,dropdown:list=None,timedropdown:list=None,lists:list=None):
    for event in pg.event.get():
        if event.type == pg.QUIT:
            sys.exit()
        elif event.type == pg.MOUSEBUTTONDOWN:
            mouse_x,mouse_y = pg.mouse.get_pos()
            for i in range(len(button)):
                if button[i].show or button[i].force_check:
                    button[i].check_active(mouse_x,mouse_y)
            if lists:
                for i in range(len(lists)):
                    lists[i].check_selected_box(mouse_x,mouse_y)
                    lists[i].check_delete_box(mouse_x,mouse_y)
            if input:
                for i in range(len(input)):
                    input[i].click = False
                for i in range(len(input)):
                    input[i].check_active(mouse_x,mouse_y)
            if dropdown:
                for i in range(len(dropdown)):
                    dropdown[i].handle_event(event)
            if timedropdown:
                for i in range(len(timedropdown)):
                    timedropdown[i].handle_event(event)
        elif event.type == pg.KEYDOWN:
            if(event.key == 13):
                if(button[0].msg == "SUBMIT" or button[0].msg == 'REGISTER'):
                    button[0].active = True
            if(input):
                for i in range(len(input)):
                    if input[i].click:
                        input[i].handle_event(event) 

def format_date_time(year, month, day, hour, minute):
    year = int(year)
    month = int(month)
    day = int(day)
    hour = int(hour)
    minute = int(minute)
    dt = datetime(year, month, day, hour, minute)
    return dt.strftime("%Y-%m-%d %H:%M:%S")

#############################################################
####                    MAIN CLASS                       ####
#############################################################

class LOG():
    def __init__(self):
        self.check = False
        self.page = "menu"
        self.last_page = list()
        self.shell = cmds()
        self.screen_size = (800, 600)
        self.screen_color = pg.Color((230, 230, 230))
        self.screen = pg.display.set_mode(self.screen_size)
        self.title = textBox(200, 50, 400, 70, font=35, color=self.screen_color, font_color='red')
        self.exit_button = Button(300, 490, 200, 40, "EXIT", font=20, button_color='paleturquoise', text_color='black')
        self.home_button = Button(600, 490, 100, 30, "HOME", font=16, button_color='#aaaaaa', text_color='black')
        self.run_state = 0
        self.openat_state = 0
        self.unlink_state = 0
        self.execve_state = 0
        self.shutdown_state = 0
        self.reboot_state = 0
        self.finit_module_state = 0
        self.mount_state = 0
        self.umount2_state = 0
        self.mknodat_state = 0

    def exit(self):
        if (self.openat_state == 1):
            self.shell.restore_openat()
        if (self.unlink_state == 1):
            self.shell.restore_unlink()
        if (self.execve_state == 1):
            self.shell.restore_execve()
        if (self.shutdown_state == 1):
            self.shell.restore_shutdown()
        if (self.reboot_state == 1):
            self.shell.restore_reboot()
        if (self.finit_module_state == 1):
            self.shell.restore_finit_module()
        if (self.mount_state == 1):
            self.shell.restore_mount()
        if (self.umount2_state == 1):
            self.shell.restore_umount2
        if (self.mknodat_state == 1):
            self.shell.restore_mknodat
        if (self.run_state == 1):
            self.shell.rmmod()
        sys.exit()

    def page_change(self, page: str):
        self.page = page

    def try_psw(self, psw):
        cmd = "echo {} | sudo -S echo hello".format(psw)
        return_code = os.system(cmd)
        if return_code == 0:
            return True
        else:
            return False

    def menu_init(self):
        self.input_label = textBox(120, 150, 130, 30, font=18, color=self.screen_color, font_color='black')
        self.psw_input = textBox(300, 150, 200, 30, font=20, color="#dddddd", font_color='black', x_format='left',psw=True)
        self.psw_sub_button = Button(550, 150, 100, 30, "SUBMIT", font=20, button_color="#cccccc", text_color='black')
        self.tip_box = textBox(200, 200, 400, 40, font=16, color=self.screen_color, font_color='red')
        self.team_button = Button(300, 270, 200, 40, "TEAM", font=20, button_color='paleturquoise', text_color='black')
        self.help_button = Button(300, 330, 200, 40, "HELP", font=20, button_color='paleturquoise', text_color='black')
        self.next_button = Button(300, 390, 200, 40, "NEXT", font=20, button_color='paleturquoise', text_color='black')
        self.exit_button = Button(300, 450, 200, 40, "EXIT", font=20, button_color='paleturquoise', text_color='black')

    def menu_action(self):
        self.last_page.append("menu")
        self.input_label.input("Input password:")
        self.tip_box.show = False
        self.psw_sub_button.show = False
        self.psw_sub_button.force_check = True
        while (True):
            check_event([self.psw_sub_button, self.team_button, self.help_button, self.exit_button,
                         self.next_button], input=[self.psw_input])
            if self.psw_sub_button.active:
                self.psw_sub_button.active = False
                self.check = self.try_psw(self.psw_input.text)
                self.psw = self.psw_input.text
                self.psw_input.clear()
                if (self.check):
                    self.psw_input.font_color = 'red'
                    self.psw_input.input("Correct!")
                    self.shell.get_psw(self.psw)
                    self.tip_box.show = True
                    self.tip_box.input("YOU CAN GO NEXT")
                    self.psw_input.write = False
                else:
                    self.input_label.font_color = 'red'
                    self.input_label.input("Try again")
            elif self.next_button.active:
                self.next_button.active = False
                if self.check:
                    self.page = "main"
                    break
                else:
                    self.tip_box.change_color("white")
                    self.tip_box.input("YOU MUST INPUT THE CORRECT PASSWORD")
                    self.tip_box.show = True
            elif self.help_button.active:
                self.help_button.active = False
                self.page = "help"
                break
            elif self.team_button.active:
                self.team_button.active = False
                self.page = "team"
                break
            elif self.exit_button.active:
                self.exit()

            self.update_screen([self.title, self.input_label, self.exit_button, self.psw_input,
                                self.team_button, self.help_button, self.psw_sub_button, self.next_button,
                                self.tip_box])

    def main_init(self):
        self.shell_box = textBox(250, 480, 300, 30, font=16, color=self.screen_color, font_color='black')
        self.path_input = textBox(150, 150, 500, 40, font=20, color="#dddddd", font_color='black', x_format='left')
        self.path_tip = textBox(150, 150, 500, 40, font=20, color="#dddddd", font_color='red')
        self.path_submit_button = Button(700, 150, 50, 30, "SUBMIT", font=10, button_color='paleturquoise', text_color='black')
        
        self.run_button = Button(150, 230, 140, 40, "RUN", font=20, button_color='paleturquoise', text_color='black')
        self.log_button = Button(330, 230, 140, 40, "LOG", font=20, button_color='paleturquoise', text_color='black')
        self.clean_button = Button(510, 230, 140, 40, "CLEAN", font=20, button_color='paleturquoise',
                                   text_color='black')
        self.openat_button = Button(150, 290, 140, 40, "OPENAT", font=20, button_color='paleturquoise',
                                    text_color='black')
        self.unlink_button = Button(330, 290, 140, 40, "UNLINK", font=20, button_color='paleturquoise',
                                            text_color='black')
        self.execve_button = Button(510, 290, 140, 40, "EXECVE", font=20, button_color='paleturquoise',
                                            text_color='black')
        self.shutdown_button = Button(150, 350, 140, 40, "SHUTDOWN", font=20, button_color='paleturquoise',
                                    text_color='black')
        self.reboot_button = Button(330, 350, 140, 40, "REBOOT", font=20, button_color='paleturquoise',
                                    text_color='black')
        self.finit_module_button = Button(510, 350, 140, 40, "FINIT", font=20, button_color='paleturquoise',
                                    text_color='black')
        self.mount_button = Button(150, 410, 140, 40, "MOUNT", font=20, button_color='paleturquoise',
                                    text_color='black')
        self.umount2_button = Button(330, 410, 140, 40, "UMOUNT2", font=20, button_color='paleturquoise',
                                    text_color='black')
        self.mknodat_button = Button(510, 410, 140, 40, "MKNODAT", font=20, button_color='paleturquoise',
                                    text_color='black')

        
        self.exit_button = Button(600, 530, 100, 30, "EXIT", font=16, button_color='#aaaaaa', text_color='black')

    def main_action(self):
        self.path_tip.input("Please input the path(Optional)")
        self.last_page.append("main")
        self.path_submit_button.show = False
        self.path_submit_button.force_check = True
        while (True):
            check_event([self.path_submit_button,self.run_button, self.openat_button, self.unlink_button, 
                         self.execve_button,self.shutdown_button,self.reboot_button,self.finit_module_button,
                         self.mount_button,self.umount2_button,self.mknodat_button,self.clean_button,self.log_button, 
                         self.exit_button, self.home_button], input=[self.path_input])

            buttons = [self.run_button, self.openat_button, self.unlink_button, 
                       self.execve_button, self.shutdown_button, self.reboot_button, self.finit_module_button,
                       self.mount_button, self.umount2_button, self.mknodat_button]
            states = [self.run_state, self.openat_state, self.unlink_state, 
                      self.execve_state, self.shutdown_state, self.reboot_state,self.finit_module_state,
                      self.mount_state, self.umount2_state, self.mknodat_state]
            
            for i in range(len(buttons)):
                if states[i]:
                    buttons[i].change_color("orange")
                else:
                    buttons[i].change_color("paleturquoise")

            if self.run_button.active:
                self.run_button.active = False
                if self.run_state == 0:
                    self.shell.insmod()
                    self.run_state = 1
                    self.shell_box.change_font_color("black")
                    self.shell_box.input("INSMOD SUCCESS")
                else:
                    if self.openat_state:
                        self.openat_state = 0
                        self.shell.restore_openat()
                    self.shell.rmmod()
                    self.run_state = 0
                    self.shell_box.change_font_color("black")
                    self.shell_box.input("RMMOD SUCCESS")

            elif self.clean_button.active:
                self.clean_button.active = False
                self.shell.clean()
                self.shell_box.change_font_color("black")
                self.shell_box.input("CLEAN SUCCESS")

            elif self.openat_button.active:
                self.openat_button.active = False
                if self.openat_state == 0:
                    if self.run_state == 0:
                        self.shell_box.change_font_color("red")
                        self.shell_box.input("CNNOT HOOK OPENAT")
                    else:
                        self.shell.hook_openat()
                        self.openat_state = 1
                        self.shell_box.change_font_color("black")
                        self.shell_box.input("HOOK OPENAT SUCCESS")
                        self.openat_button.change_color("orange")
                else:
                    self.shell.restore_openat()
                    self.openat_state = 0
                    self.shell_box.change_font_color("black")
                    self.shell_box.input("RESTORE OPENAT SUCCESS")

            elif self.unlink_button.active:
                self.unlink_button.active = False
                if self.unlink_state == 0:
                    if self.run_state == 0:
                        self.shell_box.change_font_color("red")
                        self.shell_box.input("CNNOT HOOK UNLINK")
                    else:
                        self.shell.hook_unlink()
                        self.unlink_state = 1
                        self.shell_box.change_font_color("black")
                        self.shell_box.input("HOOK UNLINK SUCCESS")
                        self.unlink_button.change_color("orange")
                else:
                    self.shell.restore_unlink()
                    self.unlink_state = 0
                    self.shell_box.change_font_color("black")
                    self.shell_box.input("RESTORE UNLINK SUCCESS")

            elif self.execve_button.active:
                self.execve_button.active = False
                if self.execve_state == 0:
                    if self.run_state == 0:
                        self.shell_box.change_font_color("red")
                        self.shell_box.input("CNNOT HOOK EXECVE")
                    else:
                        self.shell.hook_execve()
                        self.execve_state = 1
                        self.shell_box.change_font_color("black")
                        self.shell_box.input("HOOK EXECVE SUCCESS")
                        self.execve_button.change_color("orange")
                else:
                    self.shell.restore_execve()
                    self.execve_state = 0
                    self.shell_box.change_font_color("black")
                    self.shell_box.input("RESTORE EXECVE SUCCESS")
                    
            elif self.shutdown_button.active:
                self.shutdown_button.active = False
                if self.shutdown_state == 0:
                    if self.run_state == 0:
                        self.shell_box.change_font_color("red")
                        self.shell_box.input("CNNOT HOOK SHUTDOWN")
                    else:
                        self.shell.hook_shutdown()
                        self.shutdown_state = 1
                        self.shell_box.change_font_color("black")
                        self.shell_box.input("HOOK SHUTDOWN SUCCESS")
                        self.shutdown_button.change_color("orange")
                else:
                    self.shell.restore_shutdown()
                    self.shutdown_state = 0
                    self.shell_box.change_font_color("black")
                    self.shell_box.input("RESTORE SHUTDOWN SUCCESS")

            elif self.reboot_button.active:
                self.reboot_button.active = False
                if self.reboot_state == 0:
                    if self.run_state == 0:
                        self.shell_box.change_font_color("red")
                        self.shell_box.input("CNNOT HOOK REBOOT")
                    else:
                        self.shell.hook_reboot()
                        self.reboot_state = 1
                        self.shell_box.change_font_color("black")
                        self.shell_box.input("HOOK REBOOT SUCCESS")
                        self.reboot_button.change_color("orange")
                else:
                    self.shell.restore_reboot()
                    self.reboot_state = 0
                    self.shell_box.change_font_color("black")
                    self.shell_box.input("RESTORE REBOOT SUCCESS")

            elif self.finit_module_button.active:
                self.finit_module_button.active = False
                if self.finit_module_state == 0:
                    if self.run_state == 0:
                        self.shell_box.change_font_color("red")
                        self.shell_box.input("CNNOT HOOK FINIT_MODULE")
                    else:
                        self.shell.hook_finit_module()
                        self.finit_module_state = 1
                        self.shell_box.change_font_color("black")
                        self.shell_box.input("HOOK FINIT_MODULE SUCCESS")
                        self.finit_module_button.change_color("orange")
                else:
                    self.shell.restore_finit_module()
                    self.finit_module_state = 0
                    self.shell_box.change_font_color("black")
                    self.shell_box.input("RESTORE FINIT_MODULE SUCCESS")

            elif self.mount_button.active:
                self.mount_button.active = False
                if self.mount_state == 0:
                    if self.run_state == 0:
                        self.shell_box.change_font_color("red")
                        self.shell_box.input("CNNOT HOOK MOUNT")
                    else:
                        self.shell.hook_mount()
                        self.mount_state = 1
                        self.shell_box.change_font_color("black")
                        self.shell_box.input("HOOK MOUNT SUCCESS")
                        self.mount_button.change_color("orange")
                else:
                    self.shell.restore_mount()
                    self.mount_state = 0
                    self.shell_box.change_font_color("black")
                    self.shell_box.input("RESTORE MOUNT SUCCESS")

            elif self.umount2_button.active:
                self.umount2_button.active = False
                if self.umount2_state == 0:
                    if self.run_state == 0:
                        self.shell_box.change_font_color("red")
                        self.shell_box.input("CNNOT HOOK UMOUNT2")
                    else:
                        self.shell.hook_umount2()
                        self.umount2_state = 1
                        self.shell_box.change_font_color("black")
                        self.shell_box.input("HOOK UMOUNT2 SUCCESS")
                        self.umount2_button.change_color("orange")
                else:
                    self.shell.restore_umount2()
                    self.umount2_state = 0
                    self.shell_box.change_font_color("black")
                    self.shell_box.input("RESTORE UMOUNT2 SUCCESS")

            elif self.mknodat_button.active:
                self.mknodat_button.active = False
                if self.mknodat_state == 0:
                    if self.run_state == 0:
                        self.shell_box.change_font_color("red")
                        self.shell_box.input("CNNOT HOOK MKNODAT")
                    else:
                        self.shell.hook_mknodat()
                        self.mknodat_state = 1
                        self.shell_box.change_font_color("black")
                        self.shell_box.input("HOOK MKNODAT SUCCESS")
                        self.mknodat_button.change_color("orange")
                else:
                    self.shell.restore_mknodat()
                    self.mknodat_state = 0
                    self.shell_box.change_font_color("black")
                    self.shell_box.input("RESTORE MKNODAT SUCCESS")

                
            elif self.path_submit_button.active:
                self.path_submit_button.active = False
                if self.run_state == 0:
                    self.shell_box.change_font_color("red")
                    self.shell_box.input("CNNOT CONTROL PATH, CLICK RUN FIRST")
                else:
                    path = self.path_input.text
                    self.shell_box.change_font_color("black")
                    self.shell_box.input("Detected path: " + path)
                    self.shell.path_control(path)
                
                
            elif self.log_button.active:
                self.log_button.active = False
                if self.run_state and self.openat_state:
                    self.page = "log"
                    break
                else:
                    self.tip_box.change_color("white")
                    self.tip_box.input("YOU MUST CLICK RUN AND OPENAT FIRST")
                    self.tip_box.show = True

            elif self.home_button.active:
                self.home_button.active = False
                self.page = "menu"
                break

            elif self.exit_button.active:
                self.exit()

            if self.path_input.text:
                self.path_tip.show = False
                
            self.update_screen([self.title, self.exit_button, self.home_button, 
                                self.shell_box, self.path_input, self.path_tip, self.path_submit_button,
                                self.run_button, self.log_button, self.clean_button,  
                                self.openat_button, self.unlink_button, 
                                self.execve_button, self.shutdown_button, self.reboot_button, self.finit_module_button, 
                                self.mount_button, self.umount2_button, self.mknodat_button])

    def team_init(self):
        message = "System level resource access audit based on system call overload\n\n"
        message += "Software Version:   0.1.0\n\n"
        message += "Team Members:    heatingma  qigu  cfg554  halsayxi  sora"
        self.team_box = textBox(150, 140, 500, 200, font=18, color=self.screen_color, font_color='black',
                                x_format='left', y_format='up')
        self.team_box.input(message)

    def team_action(self):
        while (True):
            check_event([self.home_button])
            if self.home_button.active:
                self.home_button.active = False
                self.page = "menu"
                break

            self.update_screen([self.title, self.home_button, self.team_box])

    def help_init(self):
        message = "Step 1: Enter the user password, submit and click NEXT to use the software\n\n"
        message += "Step 2: click the RUN button, and the button changes from blue to orange, \n\
          which is equivalent to insmod operation\n\n"
        message += "Step 3: click the corresponding button to perform operations \n \
          such as Function overloading and log clearing\n\n"
        message += "Note 1: The Function overloading can only be performed after RUN\n\n"
        message += "Note 2: Click the LOG button to manage logs"
        self.help_box = textBox(100, 140, 600, 300, font=18, color=self.screen_color, font_color='black',
                                x_format='left', y_format='up')
        self.help_box.input(message)

    def help_action(self):
        while (True):
            check_event([self.home_button])
            if self.home_button.active:
                self.home_button.active = False
                self.page = "menu"
                break

            self.update_screen([self.title, self.home_button, self.help_box])

    def log_init(self):
        self.sort_input = textBox(150, 210, 100, 40, font=20, color="#dddddd", font_color='black', x_format='left')
        self.sort_button = Button(250, 210, 100, 40, "SORT", font=20, button_color='paleturquoise', text_color='black')
        self.search_input = textBox(450, 210, 100, 40, font=20, color="#dddddd", font_color='black', x_format='left')
        self.search_button = Button(550, 210, 100, 40, "SEARCH", font=20, button_color='paleturquoise', text_color='black')
        self.merge_button = Button(150, 270, 200, 40, "MERGE", font=20, button_color='paleturquoise', text_color='black')
        self.reload_button = Button(450, 270, 200, 40, "RELOAD", font=20, button_color='paleturquoise', text_color='black')
        self.pause_continue_button = Button(150, 330, 200, 40, "PAUSE", font=20, button_color='paleturquoise', text_color='black')
        self.clear_button = Button(450, 330, 200, 40, "CLEAR", font=20, button_color='paleturquoise', text_color='black')
        self.show_button = Button(300, 390, 200, 40, "SHOW", font=20, button_color='paleturquoise', text_color='black')
        self.pause_flag = False


    def log_action(self):
        self.shell.log_control();
        self.last_page.append("log")
        elements = [self.sort_button, self.search_button, self.merge_button, self.reload_button,
                   self.pause_continue_button, self.clear_button, self.show_button, self.home_button]
        while (True):
            check_event(elements, input=[self.sort_input, self.search_input])
            if self.home_button.active:
                self.home_button.active = False
                self.page = "menu"
                break

            elif self.sort_button.active:
                self.sort_button.active = False
                self.shell.log_sort(self.sort_input.text)

            elif self.search_button.active:
                self.search_button.active = False
                self.shell.log_search(self.search_input.text)

            elif self.merge_button.active:
                self.merge_button.active = False
                self.shell.log_merge()

            elif self.reload_button.active:
                self.reload_button.active = False
                self.shell.log_reload()

            elif self.pause_continue_button.active:
                self.pause_continue_button.active = False
                if self.pause_flag:
                    self.pause_continue_button.change_msg("PAUSE")
                    self.shell.log_continue()
                    self.pause_flag = False
                else:
                    self.pause_continue_button.change_msg("CONTINUE")
                    self.shell.log_pause()
                    self.pause_flag = True

            elif self.clear_button.active:
                self.clear_button.active = False
                self.shell.log_clear()

            elif self.show_button.active:
                self.show_button.active = False
                self.shell.log_show()

            self.update_screen(elements + [self.title, self.sort_input, self.search_input])

    def update_screen(self, list: list):
        self.screen.fill(self.screen_color)
        self.title.input("SOFTWARE")
        for i in range(len(list)):
            if list[i].show:
                list[i].draw(self.screen)
        pg.display.flip()

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

    def log_control(self):
        command = ['./log_control']
        self.run_command(command, 'log_control')

    def log_sort(self,input):
        self.procs['log_control'].stdin.write("sort " + input + "\n")
        self.procs['log_control'].stdin.flush()
  
    def log_search(self,input):
        self.procs['log_control'].stdin.write("search " + input + "\n")
        self.procs['log_control'].stdin.flush()
  
    def log_merge(self):
        self.procs['log_control'].stdin.write('merge\n')
        self.procs['log_control'].stdin.flush() 

    def log_reload(self):
        self.procs['log_control'].stdin.write('reload\n')
        self.procs['log_control'].stdin.flush() 

    def log_continue(self):
        self.recv_msg_from_kernel()
 
    def log_pause(self):
        os.kill(self.pids['recv_msg'], signal.SIGUSR1)

    def log_clear(self):
        self.procs['log_control'].stdin.write('clear\n')
        self.procs['log_control'].stdin.flush() 

    def log_show(self):
        self.procs['log_control'].stdin.write('show\n')
        self.procs['log_control'].stdin.flush() 
  
#############################################################
####                    BASIC CLASS                      ####
#############################################################

class Button:
    def __init__(self, x, y, w, h, msg, font_name="Georgia", font=24, button_color=(0, 255, 0), text_color=(255, 255, 255),
                 border_radius=15, border_width=2, border_color=(255, 255, 255, 128)):
        self.active = False
        self.check = False
        self.button_color = pg.Color(button_color)
        self.text_color = pg.Color(text_color)
        self.border_radius = border_radius
        self.border_width = border_width
        self.border_color = pg.Color(border_color)
        self.font = pg.font.SysFont(font_name, font)
        self.rect = pg.Rect(x, y, w, h)
        self.msg = msg
        self.show = True
        self.force_check = False
        self.prep_msg(msg)
        
    def prep_msg(self, msg):
        self.msg_image = self.font.render(msg, True, self.text_color, self.button_color)
        self.msg_image_rect = self.msg_image.get_rect()
        self.msg_image_rect.center = self.rect.center

    def change_color(self, color):
        self.button_color = color
        self.prep_msg(self.msg)
        
    def change_msg(self, msg):
        self.msg = msg
        self.prep_msg(self.msg)

    def draw(self, screen):
        if self.show:
            border_rect = self.rect.copy()
            border_rect.inflate_ip(self.border_width * 2, self.border_width * 2)
            pg.draw.rect(screen, self.border_color, border_rect, border_radius=self.border_radius)
            pg.draw.rect(screen, self.button_color, self.rect, border_radius=self.border_radius)
            screen.blit(self.msg_image, self.msg_image_rect)

    def check_active(self, x, y):
        if self.check == False:
            if self.rect.collidepoint(x, y):
                self.active = True
            
class textBox:
    def __init__(self, x, y, w, h, font=20,color='white',font_color='black'
                 ,font_name="Times New Roman",ttf=False,x_format='center',y_format='center'
                 ,x_space=5,y_space=5,border_radius=10,bold=True,italic=False,
                 border=False,border_width=2,border_color="black",psw=False):
        self.rect = pg.Rect(x, y, w, h)
        self.click =  False
        self.color = pg.Color(color)
        self.font_color = pg.Color(font_color)
        self.text = ""
        self.psw = psw
        self.write = True
        self.ttf = ttf
        self.bold = bold
        self.italic = italic
        self.font_name = font_name
        self.border = border
        self.border_width = border_width
        self.border_color = border_color
        if self.ttf:
            self.FONT = pg.font.Font(self.font_name,font)
            self.FONT.set_bold(self.bold)
            self.FONT.set_italic(self.italic)
        else:
            self.FONT = pg.font.SysFont(name=self.font_name,size=font,bold=self.bold,italic=self.italic)
        self.x_format = x_format
        self.y_format = y_format
        self.x_space = x_space
        self.y_space = y_space
        self.border_radius = border_radius
        self.show = True
        
    def check_active(self,x,y):
        if(self.click == False):
            if self.rect.collidepoint(x,y):
                self.click = True  
                  
    def change_font_size(self,font_size):            
        if self.ttf:
            self.FONT = pg.font.Font(self.font_name,font_size)
            self.FONT.set_bold(self.bold)
            self.FONT.set_italic(self.italic)
        else:
            self.FONT = pg.font.SysFont(name=self.font_name,size=font_size,bold=self.bold,italic=self.italic)
            
    def input(self,input):
        if self.write:
            self.text = input
    
    def clear(self):
        self.text = ""
        
    def change_font_color(self,color):
        self.font_color = pg.Color(color)        
    
    def change_color(self,color):
        self.color = pg.Color(color)
    
    def handle_event(self,event:pg.event.Event):
        if self.write:
            if (event.key == pg.K_BACKSPACE):
                self.text = self.text[0:-1]
            elif(event.key == pg.K_ESCAPE):
                sys.exit()
            elif(event.key == 13):
                pass
            elif(event.key >= 0 and event.key <= 128):
                letter = chr(event.key)
                self.text += letter             
           
    def draw(self, screen):
        if self.border:
            border_rect = self.rect.copy()
            border_rect.inflate_ip(self.border_width * 2, self.border_width * 2)
            pg.draw.rect(screen, self.border_color, border_rect, border_radius=self.border_radius)
            pg.draw.rect(screen, self.color, self.rect, border_radius=self.border_radius)
        else:
            pg.draw.rect(screen, self.color, self.rect, border_radius=self.border_radius)
        # 绘制文本
        if self.psw:
            masked_text = '•' * len(self.text)
        else:
            masked_text = self.text
        self.texts = []
        current_text = ''
        for i in range(len(masked_text)):
            if(masked_text[i] != '\n'):
                current_text += masked_text[i]
            else:
                self.texts.append(current_text)
                current_text = ''
        self.texts.append(current_text)
        length = len(self.texts)
        for i in range(length):
            current_text = self.texts[i]
            text_surface = self.FONT.render(current_text,True,self.font_color)
            text_surface_rect = text_surface.get_rect()
            if(self.x_format == 'center'):
                text_surface_rect.centerx = self.rect.centerx
            elif(self.x_format == 'left'):
                text_surface_rect.x = self.rect.x + self.x_space
            if(self.y_format == 'center'):
                space_height = (self.rect.h - length * text_surface_rect.h)/(length+1)
                text_surface_rect.y = self.rect.y + space_height*(i+1) + text_surface_rect.h*i
            elif(self.y_format == 'up'):
                space_height = self.y_space
                text_surface_rect.y = self.rect.y + space_height*(i+1) + text_surface_rect.h*i
            screen.blit(text_surface,text_surface_rect)
              
class ListBoxes:
    def __init__(self,x,y,w,h,title=None,title_font=24,font=20,color='white',font_color='black',
                 font_name="Times New Roman",ttf=False,x_format='center',y_format='center'):
        self.x = x
        self.y = y
        self.w = w
        self.h = h
        self.begin = 0        
        self.len = 0
        self.font = font
        self.color = color
        self.font_color = font_color
        self.font_name = font_name
        self.ttf = ttf
        self.x_format = x_format
        self.y_format = y_format
        self.show = True
        self.rects = []
        self.rects.append(textBox(x,y,w,h,title_font,color,font_color,font_name,ttf,'center','center'))
        if(title):
            self.rects[0].input(title)
        else:
            self.rects[0].input("List")
    
    def clean_box(self):
        for _ in range(self.len):
            self.rects.pop()
        self.len = 0
    
    def add_box(self,input=None):
        self.len += 1
        yy = (self.h+2) * ((self.len-1)%5 + 1)  + self.y  
        box = textBox(self.x,yy,self.w,self.h,self.font,self.color,
                      self.font_color,self.font_name,self.ttf,self.x_format,self.y_format)
        if input:
            box.input(input)
        self.rects.append(box)            
                
    def delete_box(self,id=None):
        self.len -= 1
        if id is None:
            self.rects.pop()
        else:
            del self.rects[id]
            
    def input(self,id,input):
        self.rects[id].input(input)

    def handle_event(self,event):
        if self.len >= 1:
            for i in range(self.len):
                self.rects[i+1].handle_event(event)
                                    
    def draw(self, screen):
        self.rects[0].draw(screen)
        for i in range(min(self.len-self.begin,5)):
            self.rects[i+self.begin+1].draw(screen)

class LISTS:
    def __init__(self,x,y,w:list,h,title:list,title_font=24,font=20,color='white',font_color='black',
                 font_name="Times New Roman",ttf=False,x_format='center',y_format='center'):
        self.lists = list()
        self.row = 0
        self.show = True
        self.begin = 0
        self.len = len(w)
        self.color = color
        self.active = False 
        self.delete_num = 0
        self.delete_box = []
        for i in range(self.len):
            self.lists.append(ListBoxes(x, y, w[i], h, title[i], title_font, font, color, font_color, font_name, ttf, x_format, y_format))
            x = x+w[i]
        self.selected_box = self.lists[0].rects[0]

    def get_selected_box(self):
        return self.selected_box
    
    def restore_color(self):
        for i in range(self.len):
            self.lists[i].rects[0].change_color(self.color)
    
    def clean_all(self):
        self.row = 0
        for i in range(self.len):
            self.lists[i].clean_box()
    
    def add_row(self,input:list):
        self.row += 1
        for i in range(self.len):
            self.lists[i].add_box(input[i])           
                
    def delete_list(self,id=None):
        del self.lists[id].rects[id]
        
    def delete_row(self,id=None):
        self.row -= 1
        for i in range(self.len):
            self.lists[i].delete_box(id)
            
    def input(self,id,input):
        self.rects[id].input(input)

    def handle_event(self,event):
        for i in range(self.len):
            self.lists[i].handle_event(event)
    
    def check_selected_box(self,x,y):
        for i in range(self.len):
            if(self.lists[i].rects[0].rect.collidepoint(x,y)):
                if self.selected_box != self.lists[i].rects[0]:
                    self.selected_box = self.lists[i].rects[0]
                    self.active = True
                    self.restore_color()
    
    def check_delete_box(self,x,y):
        if self.row == 0:
            return
        length = min((self.row-self.begin),5)
        for i in range(length):
            cur_box = self.lists[0].rects[i+1+self.begin]
            if(cur_box.rect.collidepoint(x,y)):
                if self.delete_num == 0:
                    self.delete_num += 1
                    cur_box.change_color("orange")
                    self.delete_box.append(cur_box.text)
                else:
                    flag = True
                    for j in range(self.delete_num):
                        if self.delete_box[j] == cur_box.text:
                            cur_box.change_color(self.color)
                            self.delete_box.remove(cur_box.text)
                            self.delete_num -= 1
                            flag  = False
                            break
                    if flag:
                        cur_box.change_color("orange")
                        self.delete_box.append(cur_box.text)
                        self.delete_num += 1
    
    def change_begin(self,begin):
        self.begin = begin
        for i in range(self.len):
            self.lists[i].begin = begin                
                                        
    def draw(self, screen):
        for i in range(self.len):
            self.lists[i].draw(screen)       
    
class Dropdown:
    def __init__(self, x, y, w, h, label, options=["None"], font=20, font_name="Hack",ttf=False,
                 label_color="#999999", font_color="black", bg_color="white", border_color="black",
                 click_color="orange", border_radius=10, max_options=4):
        self.rect = pg.Rect(x, y, w, h)
        self.options = options
        self.label = label
        self.label_color = label_color
        if ttf:
            self.font = pg.font.Font(font_name, font)
        else:
            self.font = pg.font.SysFont(name=font_name, size=font)
        self.font_color = font_color
        self.bg_color = bg_color
        self.border_color = border_color
        self.click_color = click_color
        self.border_radius = border_radius
        self.active = False
        self.selected = None
        self.show = True
        self.max_options = max_options
        self.first_option_index = 0

    def change_options(self, options):
        self.option = options
        self.first_option_index = 0

    def set_selected(self,selected):
        self.selected = selected
    
    def change_options(self,options):
        self.options = options
        
    def draw(self, surface):
        if self.show:
            pg.draw.rect(surface, self.bg_color, self.rect, border_radius=self.border_radius)
            pg.draw.rect(surface, self.border_color, self.rect, 1, border_radius=self.border_radius)

            if self.selected is None:
                text = self.font.render(self.label, True, self.label_color)
            else:
                text = self.font.render(self.selected, True, self.font_color)

            text_rect = text.get_rect()
            text_rect.center = self.rect.center
            surface.blit(text, text_rect)

            if self.active:
                max_visible_options = min(self.max_options, len(self.options))
                options_rect = pg.Rect(self.rect.left, self.rect.bottom, self.rect.w, self.rect.h * max_visible_options)
                pg.draw.rect(surface, self.bg_color, options_rect, border_radius=self.border_radius)
                pg.draw.rect(surface, self.border_color, options_rect, 1, border_radius=self.border_radius)

                for i in range(self.first_option_index, self.first_option_index + max_visible_options):
                    if i >= len(self.options):
                        break
                    option = self.options[i]
                    option_rect = pg.Rect(self.rect.left, self.rect.bottom + (i - self.first_option_index) * self.rect.h,
                                          self.rect.w, self.rect.h)
                    if option_rect.collidepoint(pg.mouse.get_pos()):
                        pg.draw.rect(surface, self.click_color, option_rect, border_radius=self.border_radius)
                    else:
                        pg.draw.rect(surface, self.bg_color, option_rect, border_radius=self.border_radius)

                    option_text = self.font.render(option, True, self.font_color)
                    option_text_rect = option_text.get_rect()
                    option_text_rect.center = option_rect.center
                    surface.blit(option_text, option_text_rect)

    def handle_event(self, event):
        if self.rect.collidepoint(event.pos):
            self.active = not self.active
        elif self.active:
            if event.type == pg.MOUSEBUTTONDOWN and event.button == 4:
                # Scroll up
                self.first_option_index = max(0, self.first_option_index - 1)
            elif event.type == pg.MOUSEBUTTONDOWN and event.button == 5:
                # Scroll down
                max_visible_options = min(self.max_options, len(self.options))
                last_visible_option_index = self.first_option_index + max_visible_options - 1
                if last_visible_option_index < len(self.options) - 1:
                    self.first_option_index += 1
            else:
                for i in range(self.first_option_index, len(self.options)):
                    option_rect = pg.Rect(self.rect.left, self.rect.bottom + (i - self.first_option_index) * self.rect.h,
                                          self.rect.w, self.rect.h)
                    if option_rect.collidepoint(event.pos):
                        self.selected = self.options[i]
                        self.active = False
                        break
    
class TimeDropdown:
    def __init__(self,x,y,bg_color="white",font=16,font_name='Hack',ttf=False):
        self.x = x
        self.y = y
        self.bg_color = bg_color
        self.show = True
        self.font = font
        self.font_name = font_name
        self.ttf = ttf
        self.create()
        
    def create(self):
        self.yy_box = textBox(self.x+50,self.y,30,30,font=self.font,color=self.bg_color,
                              font_color='black',font_name=self.font_name,ttf=self.ttf)
        self.yy_box.input("年")
        self.yy_drop = Dropdown(self.x,self.y,50,30,label="year",options=[str(year) for year in range(2023, 2034)],
                                bg_color=self.bg_color,border_color='gray') 
        self.mm_box = textBox(self.x+130,self.y,30,30,font=self.font,color=self.bg_color,
                              font_color='black',font_name=self.font_name,ttf=self.ttf)
        self.mm_box.input("月")
        self.mm_drop = Dropdown(self.x+80,self.y,50,30,label="month",options=[str(month) for month in range(1,13)],
                                bg_color=self.bg_color,border_color='gray')  
        self.dd_box = textBox(self.x+210,self.y,30,30,font=self.font,color=self.bg_color,
                              font_color='black',font_name=self.font_name,ttf=self.ttf)
        self.dd_box.input("日")
        self.dd_drop = Dropdown(self.x+160,self.y,50,30,label="day",options=[str(day) for day in range(1, 31)],
                                bg_color=self.bg_color,border_color='gray')
        self.hh_box = textBox(self.x+290,self.y,30,30,font=self.font,color=self.bg_color,
                              font_color='black',font_name=self.font_name,ttf=self.ttf)
        self.hh_box.input("时")
        self.hh_drop = Dropdown(self.x+240,self.y,50,30,label="hour",options=[str(hour) for hour in range(0, 24)],
                                bg_color=self.bg_color,border_color='gray') 
        self.ss_box = textBox(self.x+380,self.y,30,30,font=self.font,color=self.bg_color,
                              font_color='black',font_name=self.font_name,ttf=self.ttf)
        self.ss_box.input("分")
        self.ss_drop = Dropdown(self.x+320,self.y,60,30,label="second",options=[str(second) for second in range(1, 61)],
                                bg_color=self.bg_color,border_color='gray')
        self.obj_list = [self.yy_box,self.yy_drop,self.mm_box,self.mm_drop,self.dd_box,self.dd_drop,\
                         self.hh_box,self.hh_drop,self.ss_box,self.ss_drop]
        self.handle_list = [self.yy_drop,self.mm_drop,self.dd_drop,self.hh_drop,self.ss_drop]
        
    def draw(self,surface):
        for i in range(len(self.obj_list)):
            self.obj_list[i].draw(surface)
    
    def handle_event(self,event:pg.event.Event):
        for i in range(len(self.handle_list)):
            self.handle_list[i].handle_event(event)
    
    def set_time(self,yy=None,mm=None,dd=None,hh=None,ss=None):
        if yy is not None:
            self.yy_drop.set_selected(yy)
        if mm is not None:
            self.mm_drop.set_selected(mm)
        if dd is not None:
            self.dd_drop.set_selected(dd)
        if hh is not None:
            self.hh_drop.set_selected(hh)        
        if ss is not None:
            self.ss_drop.set_selected(ss)   
            
    def get_time(self):
        yy = self.yy_drop.selected
        mm = self.mm_drop.selected  
        dd = self.dd_drop.selected
        hh = self.hh_drop.selected
        ss = self.ss_drop.selected
        if (yy is None or mm is None or dd is None or hh is None or ss is None):
            return None
        else:
            return format_date_time(yy,mm,dd,hh,ss)

class Timer:
    def __init__(self, x, y, w, h, font_name='Times New Roman', font_size=20, font_color="black", bg_color="white"):
        self.x = x
        self.y = y
        self.width = w
        self.height = h
        self.font = pg.font.SysFont(font_name, font_size)
        self.font_color = font_color
        self.bg_color = bg_color
        self.show = True
        self.active = False
        now = datetime.now()
        self.time_str = now.strftime('%Y-%m-%d %H:%M:%S')
        self.last_active_time = now.strftime('%Y-%m-%d %H:%M:%S')
        
    def draw(self, screen):
        now = datetime.now()
        self.time_str = now.strftime('%Y-%m-%d %H:%M:%S')
        time_text = self.font.render(self.time_str, True, self.font_color, self.bg_color)
        time_rect = time_text.get_rect()
        time_rect.center = (self.x + self.width // 2, self.y + self.height // 2)
        pg.draw.rect(screen, self.bg_color, (self.x, self.y, self.width, self.height))
        screen.blit(time_text, time_rect)
    
    def get_time(self):
        return self.time_str

    def shell_action(self,shell:cmds,user_name):
        return shell.check_remind(user_name)

#############################################################
####                     SOFTWARE                        ####
#############################################################
def main():
    pg.init()
    pg.display.set_caption("software")
    work = LOG()
    while True:
        if work.exit_button.active:
            sys.exit()
        if (work.page == 'menu'):
            work.menu_init()
            work.menu_action()
        elif (work.page == 'main'):
            work.main_init()
            work.main_action()
        elif (work.page == 'help'):
            work.help_init()
            work.help_action()
        elif (work.page == 'log'):
            work.log_init()
            work.log_action()
        elif (work.page == 'team'):
            work.team_init()
            work.team_action()


if __name__ == "__main__":
    main()

