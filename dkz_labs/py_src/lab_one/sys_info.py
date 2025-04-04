import os
import platform
import psutil
import multiprocessing # number of cores)
from datetime import datetime
import signal

def accumulate_green(head,body):
    class bcolors:
        OKGREEN = '\033[92m'
        ENDC = '\033[0m'
    return bcolors.OKGREEN + head + bcolors.ENDC +" "+ f"{body}\n"

def clear_console():
    if os.name == 'nt':  # 'nt' is for Windows
        os.system('cls')
    else:  # 'posix' for macOS/Linux
        os.system('clear')

# freq is time in seconds in which update happens ;)
def print_sys_info(freq):
    # collect data
    cpu_freq = psutil.cpu_freq()
    total_ram = psutil.virtual_memory().total / (1024 ** 3)
    available_ram = psutil.virtual_memory().available / (1024 ** 3)
    
    cpu_usage = psutil.cpu_percent(interval=freq) 
    current_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    #print data
    disks = ""
    for partition in psutil.disk_partitions():
        disks += f"{partition.device} "
    msg = ""
    msg += accumulate_green("OS:",platform.system())
    msg += accumulate_green("Processor Name:" ,platform.processor())
    msg += accumulate_green("Number of cores:",multiprocessing.cpu_count())
    msg += accumulate_green("Max Clock Speed:", f"{cpu_freq.max:.2f} MHz")
    msg += accumulate_green(f"Total RAM:", f"{total_ram:.2f} GB")
    msg += accumulate_green(f"Available RAM:", f"{available_ram:.2f} GB")
    msg += accumulate_green("Disks:" ,f"{disks}")
    msg += accumulate_green("CPU Usage:" ,f"{cpu_usage:.2f}%")
    msg += accumulate_green("Current Date and Time:" ,current_time)
    clear_console()
    print(msg)

should_run = True
def graceful_shutdown(signum, frame):
    global should_run
    should_run = False
signal.signal(signal.SIGINT, graceful_shutdown)


while should_run == True:
    print_sys_info(1)