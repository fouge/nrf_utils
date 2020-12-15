#!/usr/bin/env python -u

from serial import Serial
import re
import datetime
import time
import sys
import platform
import subprocess

if sys.version_info[0] < 3:
    raise Exception("Python 3 or a more recent version is required.")

class colors:
    END = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4'
    SLOW = '\033[5'
    FAST = '\033[6'
    BLACK = '\033[30m'
    RED = '\033[31m'
    GREEN = '\033[32m'
    YELLOW = '\033[33m'
    BLUE = '\033[34m'
    MAGENTA = '\033[35m'
    CYAN = '\033[36m'
    WHITE = '\033[37m'
    BLINK = '\033[5m'
    VERBOSE = GREEN
    INFO = WHITE
    ERROR = RED
    FATAL = RED
    WARNING = YELLOW

if len(sys.argv) < 2:
    print("Usage: {} <port> <path-to-elf-file>".format(sys.argv[0]))
    quit()

port = sys.argv[1]
path_to_elf = ""
if len(sys.argv) >= 3:
    path_to_elf = sys.argv[2]

ser = Serial(port, 1000000)

log_parse_re = re.compile(r'\[(\d+):(\d+):(\d):(.+?):(\d+)\] (.*)')
timestamp_re = re.compile(r'\d{10}')

def insert_delta(matchobj):
    if len(matchobj.group(0)) == 10:
        ts = float(matchobj.group(0))
        delta = ts - time.time()

        if (delta > 0):
            delta = ("( %.3fs)" % (delta))
        else:
            delta = ("(%.3fs)" % (delta))

        return colors.BLUE + "%010i " % (ts) + colors.BOLD + "%s" % delta + colors.END

def parse_message(matchobj):
    local_time = "%s" % (now.isoformat(' ')[:23]) + colors.END
    remote_time = colors.BLUE + timestamp_re.sub(insert_delta, matchobj.group(1)) + colors.END

    level = matchobj.group(3)
    file = colors.CYAN + matchobj.group(4) + colors.END
    line = colors.MAGENTA + matchobj.group(5) + colors.END


    load = matchobj.group(2)
    if (int(load) > 5):
        load = colors.RED + load + colors.END
    elif (int(load) > 2):
        load = colors.YELLOW + load + colors.END
    else:
        load = colors.WHITE + load + colors.END

    if level == '0':
        color = colors.GREEN
    elif level == '1':
        color = colors.END
    elif level == '2':
        color = colors.YELLOW
    elif level == '3':
        color = colors.RED
    elif level  == '4':
        color = colors.RED + colors.BOLD
    else:
        color = colors.END
    message = color + matchobj.group(6) + colors.END
    message = timestamp_re.sub(insert_delta, message)

    return local_time + " [" + remote_time + ":" + load + ":" + file + ":" + line + "] " + message

while 1:
    line = ser.readline()
    if not line:
        break

    # When crash is detected
    # Crash dump is added into a temp file
    # GDB is used to back trace the crash
    if b"###CRASH###" in line.strip() and len(path_to_elf) > 0:
        print("Crash detected, retrieving crash info...")
        dump_filename = "last_crash_dump.txt"
        dump_file = open(dump_filename, 'wb+')
        crashdebug_exe = "../CrashDebug/lin64/CrashDebug"
        if platform.system() == "Darwin":
            crashdebug_exe = "../CrashDebug/osx64/CrashDebug"

        cmd = "arm-none-eabi-gdb --batch --quiet " + path_to_elf + "  -ex \"set target-charset ASCII\" -ex \"target remote | " + crashdebug_exe + " --elf " + path_to_elf + " --dump " + dump_filename + "\" -ex \"set print pretty on\" -ex \"bt full\" -ex \"quit\""
        print(cmd)

        line = ser.readline()
        dumping = True
        while b"###END###" not in line.strip():
            dump_file.write(line)
            line = ser.readline()

        print("Crash info retrieved.\n")

        dump_file.close()

        process = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
        output, error = process.communicate()

        print(output.decode("utf-8"))
        print("---------\n")
        line = b""

    now = datetime.datetime.now()

    to_print = ""
    try:
        to_print = line.decode('utf-8').rstrip()
    except Exception as err:
        print("ERROR: {}".format(err))

    print(log_parse_re.sub(parse_message, line.decode('utf-8').rstrip()))
