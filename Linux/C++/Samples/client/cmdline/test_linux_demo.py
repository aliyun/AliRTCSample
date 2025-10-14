#!/usr/bin/python
# coding:utf-8

# 1. subprocess start onertc_demo, run basic test
# 2. random exit, back to step 1, loop several times
# 3. check any core generated

import random
import subprocess
import time
import os
import logging
import sys

THIS_DIR = os.path.abspath(os.path.dirname(__file__))

FORMAT = '%(asctime)s - %(levelname)s [%(threadName)s-%(thread)d] [%(filename)s:%(lineno)d] - %(message)s'
logging.basicConfig(level=logging.INFO, format=FORMAT)

# def subprocess_run(cmd):
#     print("run cmd: %s" % cmd)
#     pid = 0
#     with subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True, text=True) as process:
#         # while True:
#         #     output = process.stdout.readline()
#         #     # If the process has terminated, the output will be empty
#         #     if output == '' and process.poll() is not None:
#         #         break
#         #     if output:
#         #         print(output.strip())
#         # After the loop, you may need to wait for the process to finish completely
#         process.wait()
#         pid = process.pid
#         print("pid: %d" % pid)
#         if process.returncode != 0:
#             print("The command failed with return code %d" % process.returncode)
#     return pid

def find_core_file(pid):
    core_dump_dir = "/var/dumps/"
    # List all files in the directory and search for a matching core dump file
    for filename in os.listdir(core_dump_dir):
        if filename.startswith("core-") and str(pid) in filename:
            logging.info("Coredump found: %s" % (core_dump_dir + filename))
            return True
    return False

if __name__ == "__main__":
    onertc_demo_path = "./out/Release/onertc_demo"
    log_dir = "onertc_log"
    app_config_filepath = "app_config.json"
    
    # -1 means indefinitely
    total_count = -1
    if len(sys.argv) > 1:
        total_count = int(sys.argv[1])

    min_time = 10
    max_time = 120

    logging.info("Run test total_count:%d" % (total_count))
    counter = 0
    while True:
        cmd = onertc_demo_path + " --log-dir=" + log_dir + " --app-config=" + app_config_filepath
        process = subprocess.Popen(cmd, shell=True)
        pid = process.pid
        time_to_kill = random.randint(min_time, max_time)
        logging.info("----- test[%d] start, pid:%d, timeout:%d" % (counter, pid, time_to_kill))
        
        # for python2
        start_time = time.time()
        is_timeout = False
        while True:
            if process.poll() is not None:
                # subprocess already terminated
                break
            if time.time() - start_time > time_to_kill:
                is_timeout = True
                process.terminate()
                break
            time.sleep(0.1)
        process.wait()

        # for python3
        # try:
        #     # Wait for the subprocess to complete, with a timeout
        #     process.wait(timeout=time_to_kill)
        # except subprocess.TimeoutExpired:
        #     logging.info("terminate the process with timeout: %d" % time_to_kill)
        #     process.terminate()
        
        flag_core = "Y" if find_core_file(pid) else "N"
        flag_timeout = "Y" if is_timeout else "N"
        logging.info("----- test[%d] end, pid:%d, timeout:%s, core:%s" % (counter, pid, flag_timeout, flag_core))

        counter += 1
        if total_count != -1 and counter >= total_count:
            break
