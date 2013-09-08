#!/usr/bin/python
import time
import subprocess

while True:
  print "This prints once a minute."
  process = subprocess.Popen(["git", "pull"], stdout=subprocess.PIPE)
  output = process.communicate()[0]
  if output != '' and 'Already up-to-date' not in output:
    print 'Code Changed, I should recompile and upload to the Atmega328 board'
  else: 
    print 'Nothing changed'
  time.sleep(60)  # Delay for 1 minute (60 seconds)
  

