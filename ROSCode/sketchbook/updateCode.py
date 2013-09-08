#!/bin/python
import subprocess
#import git 

#g = git.cmd.Git('.')
#g.pull()

process = subprocess.Popen(["git", "pull"], stdout=subprocess.PIPE)
output = process.communicate()[0]