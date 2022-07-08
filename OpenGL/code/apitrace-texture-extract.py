#!/usr/bin/python3

# evil hack
# needs convert from image magick
# format support is only RGBA 8Bit at the moment
# usage: apitrace dump --blobs [--grep='.*Image.*'] file.trace | apitrace-texture_extract.py

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

def processImage(imgInfo):
    Info('Processing ' + str(imgInfo))
    try:
        subprocess.run(['convert','-size',str(imgInfo['width'])+'x'+str(imgInfo['height']), '-depth', str(imgInfo['colorDepth']), imgInfo['format']+':blob_call'+str(imgInfo['callId'])+'.bin','blob_call'+str(imgInfo['callId'])+'.png'], check=True)
    except subprocess.CalledProcessError as E:
        Warn('Failed to process ' + str(imgInfo) + ': ' + str(E))

def selectImage(callId, call, args, blobs):
    argsDict = {}
    for arg in args:
        if len(arg) > 1:
            argsDict[arg[0]]=arg[1]
    imgInfo = {}
    imgInfo['width'] = 1
    imgInfo['height'] = 1
    imgInfo['depth'] = 1
    imgInfo['level'] = 0
    imgInfo['colorDepth'] = 8
    imgInfo['format'] = 'rgba'
    imgInfo['callId'] = callId

    if 'width' in argsDict:
        imgInfo['width'] = int(argsDict['width'])
    if 'height' in argsDict:
        imgInfo['height'] = int(argsDict['height'])
    if 'level' in argsDict:
        imgInfo['level'] = int(argsDict['level'])
    if 'format' in argsDict:
        f = argsDict['format']
        if f in FormatMap:
            imgInfo['format'] = FormatMap[f]
    if 'type' in argsDict:
        t = argsDict['type']
        if t in TypeMap:
            imgInfo['colorDepth'] = TypeMap[t]

    Debug('Found ' + call + ' ' + str(imgInfo))
    if imgInfo['level'] < Config.settings['minLevel'] or imgInfo['level'] > Config.settings['maxLevel']:
        return None
    Info('Selected ' + str(imgInfo))
    return imgInfo

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
            return selectImage(callId, call, args, blobs)
    return None


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
        return processCall(callId, call, args)

##############################################################################
# entry point                                                                #
##############################################################################

Config = Settings()
imgInfos = []

FormatMap = {
    'GL_RGBA': 'rgba',
    'GL_RGB': 'rgb',
    'GL_BGR': 'bgr',
    'GL_BGRA': 'bgra',
    'GL_RED': 'gray',
    'GL_BLUE': 'gray',
    'GL_GREEN': 'gray',
    'GL_ALPHA': 'gray',
    'GL_LUMINANCE': 'gray'
}

TypeMap = {
    'GL_UNSIGNED_BYTE': 8,
    'GL_UNSIGNED_SHORT': 16,
    'GL_UNSIGNED_INT': 32,
    # TODO: signed and float types...
}

for line in fileinput.input():
    imgInfo = processLine(line)
    if imgInfo:
        imgInfos.append(imgInfo)
Info('Selected ' +str(len(imgInfos)) + ' images')
for imgInfo in imgInfos:
    processImage(imgInfo)

