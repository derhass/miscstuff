#!/usr/bin/python3

# evil hack
# needs convert from image magick
# format support is only RGBA 8Bit at the moment
# usage: apitrace dump --blobs file.trace | apitrace-texture_extract.py

# required libraries

import os
import enum
import fileinput
import re
import subprocess

##############################################################################
# internally used Exception types                                            #
##############################################################################

class ScriptError(Exception):
    def __init__(self, message):
        self.message = message

class ScriptConfigError(ScriptError):
    pass

##############################################################################
# Logging functions                                                          #
##############################################################################

class LogLevel(enum.IntEnum):
    ERROR = 0
    WARN = 1
    INFO = 2
    DEBUG = 3

def Log(message, level=LogLevel.INFO):
    if (level <= Config.settings['logLevel']):
        print(str(level) + ': ' + message)

def Error(message):
    Log(message, LogLevel.ERROR)

def Warn(message):
    Log(message, LogLevel.WARN)

def Info(message):
    Log(message, LogLevel.INFO)

def Debug(message):
    Log(message, LogLevel.DEBUG)


##############################################################################
# class for configuration settings                                           #
##############################################################################

class Settings:
    def __init__(self):
        self.settings = {}
        self.settings['logLevel'] = LogLevel.INFO
        self.settings['minLevel'] = 0
        self.settings['maxLevel'] = 0

##############################################################################
# apitrace dump processing                                                   #
##############################################################################

def processImage(callId, call, args, blobs):
    argsDict = {}
    for arg in args:
        if len(arg) > 1:
            argsDict[arg[0]]=arg[1]
    width = 1
    height = 1
    level = 0
    depth = 8
    format = 'rgba'

    if 'width' in argsDict:
        width = int(argsDict['width'])
    if 'height' in argsDict:
        height = int(argsDict['height'])
    if 'level' in argsDict:
        level = int(argsDict['level'])
    Info('Found ' + str(callId) + ' ' + call + ' ' + str(width) + 'x'+ str(height)+ ' Level ' + str(level))
    if level < Config.settings['minLevel'] or level > Config.settings['maxLevel']:
        return
    subprocess.run(['convert','-size',str(width)+'x'+str(height), '-depth', str(depth), format+':blob_call'+str(callId)+'.bin','blob_call'+str(callId)+'.png'])



def processCall(callId, call, args):
    if re.search('Tex.*Image.*2D', call):
        argCnt = 0
        blobs = []
        for arg in args:
            if len(arg) > 1:
                match = re.search('^blob\((.*)\)$', arg[1])
                if match:
                    blobs.append([argCnt, match.groups()[0]])
            argCnt = argCnt + 1
        if len(blobs) > 0:
            processImage(callId, call, args, blobs)


def processLine(line):
    match = m=re.search('^(\d+)\s+([^(]+)\((.*)\)',line)
    if match:
        callId = int(match.groups()[0])
        call = match.groups()[1]
        argline = match.groups()[2]
        argfields = re.split('\s*,\s', argline)
        args = []
        for argfield in argfields:
            args.append(re.split('\s*=\s*', argfield))
        processCall(callId, call, args)

##############################################################################
# entry point                                                                #
##############################################################################

Config = Settings()

for line in fileinput.input():
    processLine(line)

