#!/usr/bin/python
import time
import subprocess
import os
import sys
import socket

# Just be sure that we are running only one instance of this script

def getLock(process_name):
    global lock_socket
    lock_socket = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
    try:
        lock_socket.bind('\0' + process_name)
    except socket.error:
        print 'Only one instance of this script is allowed to run'
        sys.exit()
getLock('robotica')
    
# This script will run on the raspberry pi, check if the microcontroller code changed then compile and 
# and re-upload it once done, steps:
# 1- Check if the code changed (git pull)
# 2- Kill ros-serial
# 3- Use ino to compile
# 4- Upload the binary to the micro
# 5- Re-run rosserial

delayTime = 20 # delay between each checks in seconds

def killProcessByName(scriptName):
  process = subprocess.Popen(["ps", "-eo","pid,command"], stdout=subprocess.PIPE)
  output = process.communicate()[0]
  splitted = output.rsplit('\n')
  for line in splitted:
    if scriptName in line:
      pid = line.split(' ')[1]
      print pid
      os.system('kill -9 ' + pid)

while True:
  print "Check Github updates every: " + str(delayTime) + ' seconds'
  process = subprocess.Popen(["git", "pull"], stdout=subprocess.PIPE)
  output = process.communicate()[0]
  # Step 1: check if code changed
  if output != '' and 'Already up-to-date' not in output:
    print 'Code Changed, I should recompile and upload to the Atmega328 board'
    # Step 2: kill the script
    killProcessByName('serial_node.py')
    # Step 3: recompile code
    process = subprocess.Popen(["ino", "build","-m","atmega328"], stdout=subprocess.PIPE)
    output = process.communicate()[0]    
    print output
    # Step 4: upload the binary
    process = subprocess.Popen(["ino", "upload","-m","atmega328"], stdout=subprocess.PIPE)
    output = process.communicate()[0]       
    # Step 5: Run ros-serial again
    process = subprocess.Popen(["rosrun", "rosserial_python","serial_node.py","/dev/ttyUSB0"], stdout=subprocess.PIPE)
    output = process.communicate()[0]    
  time.sleep(delayTime)  # Delay for 1 minute (60 seconds)

#killall -9 roscore
#killall -9 rosrun
#killall -9 rostopic
#roscore & 
#rosrun rosserial_python serial_node.py /dev/ttyUSB0 & 
