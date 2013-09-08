#!/usr/bin/python
import time
import subprocess

# This script will run on the raspberry pi, check if the microcontroller code changed then compile and 
# and re-upload it once done, steps:
# 1- Check if the code changed (git pull)
# 2- Kill ros-serial
# 3- Use ino to compile and upload the code
# 4- Re-run rosserial

def killPythonScript(scriptName):
  process = subprocess.Popen(["ps", "-eo","pid,command"], stdout=subprocess.PIPE)
  output = process.communicate()[0]
  splitted = output.rsplit('\n')
  for line in splitted:
    if scriptName in line:
      pid = line.split(' ')[1]
      print pid
      os.system('kill -9 ' + pid)

while True:
  print "This prints once a minute."
  process = subprocess.Popen(["git", "pull"], stdout=subprocess.PIPE)
  output = process.communicate()[0]
  # Step 1: check if code changed
  if output != '' and 'Already up-to-date' not in output:
    print 'Code Changed, I should recompile and upload to the Atmega328 board'
    # Step 2: kill the script
    killPythonScript('serial_node.py')
    # Step 3: recompile code
    os.system("killall -9 rosserial_python");
  else: 
    print 'Nothing changed'
  time.sleep(20)  # Delay for 1 minute (60 seconds)
  

#killall -9 roscore
#killall -9 rosrun
#killall -9 rostopic
#roscore & 
#rosrun rosserial_python serial_node.py /dev/ttyUSB0 & 
