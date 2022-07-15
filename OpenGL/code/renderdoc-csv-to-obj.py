#!/usr/bin/python3

# evil hack

import enum
import fileinput
import sys


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
        self.settings['logLevel'] = LogLevel.DEBUG
        self.settings['delimiter'] = ','
        self.settings['faceCnt'] = 3
        self.settings['index'] = ['IDX']
        self.settings['position'] = ['in_POSITION0.x', 'in_POSITION0.y', 'in_POSITION0.z']
        self.settings['normal'] = ['in_NORMAL0.x', 'in_NORMAL0.y', 'in_NORMAL0.z']
        self.settings['texcoord'] = ['in_TEXCOORD0.x', 'in_TEXCOORD0.y', 'in_TEXCOORD0.z']

##############################################################################
# helpers                                                                    #
##############################################################################

def splitLine(line,delimiter):
    parts = line.split(delimiter)
    if parts:
        for i in range(0,len(parts)):
            parts[i]=parts[i].strip()
    return parts

def findColumnIndexes(parts, columnNames):
    indices = []
    for name in columnNames:
        try:
            index = parts.index(name)
        except ValueError:
            index = -1
        indices.append(index)
    return indices

def extractColumnData(parts, columnIndices):
    data = []
    for index in columnIndices:
        if (index >= 0) and (index <len(parts)):
            data.append(parts[index])
        else:
            data.append('0')
    return data

def interpretColumnData(data):
    for i in range(0,len(data)):
        data[i]=float(data[i])
    return data

##############################################################################
# main processing                                                            #
##############################################################################

class ColumnMapping:
    def __init__(self):
        self.index = [-1]
        self.postition = [-1, -1, -1]
        self.normal = [-1, -1, -1]
        self.texcoord = [-1, -1]

class Vertex:
    def __init__(self):
        self.indexMap = ['0']
        self.index = -1
        self.indexRemapped = -1
        self.position = [0.0, 0.0, 0.0]
        self.normal = [0.0, 0.0, 1.0]
        self.texcoord = [0.0, 0.0]
        
    def __init__(self, parts, cm):
        self.importData(parts, cm)

    def importData(self, parts, cm):
        self.indexMap = extractColumnData(parts, cm.index)
        self.index = int(self.indexMap[0])
        self.indexRemapped = -1
        self.position = interpretColumnData(extractColumnData(parts, cm.position))
        self.normal = interpretColumnData(extractColumnData(parts, cm.normal))
        self.texcoord = interpretColumnData(extractColumnData(parts, cm.texcoord))

    def exportObjValue(self, f, value):
        f.write(' ' + f'{value:.6f}')

    def exportObjPosition(self, f):
        f.write('v')
        self.exportObjValue(f, self.position[0])
        self.exportObjValue(f, self.position[1])
        self.exportObjValue(f, self.position[2])
        f.write('\n')

    def exportObjNormal(self, f):
        f.write('vn')
        self.exportObjValue(f, self.normal[0])
        self.exportObjValue(f, self.normal[1])
        self.exportObjValue(f, self.normal[2])
        f.write('\n')

    def exportObjTexcoord(self, f):
        f.write('vt')
        self.exportObjValue(f, self.texcoord[0])
        self.exportObjValue(f, self.texcoord[1])
        f.write('\n')

class Processor:
    def __init__(self):
        self.delimiter = Config.settings['delimiter']
        self.reset()

    def reset(self):
        self.lineNumber = 1
        self.columnMap = ColumnMapping()
        self.faces = []
        self.facesRemapped = []
        self.vertices = []
        self.verticesMap = {}

    def processHeader(self, parts):
        self.columnMap.index = findColumnIndexes(parts, Config.settings['index'])
        self.columnMap.position = findColumnIndexes(parts, Config.settings['position'])
        self.columnMap.normal = findColumnIndexes(parts, Config.settings['normal'])
        self.columnMap.texcoord = findColumnIndexes(parts, Config.settings['texcoord'])

    def processLine(self, parts):
        v = Vertex(parts, self.columnMap)
        try:
            v.indexRemapped = self.verticesMap[v.index]
            Debug('Found existing vertex ' + str(v.indexRemapped) + ', was ' + str(self.vertices[v.indexRemapped].index))
        except KeyError:
            v.indexRemapped = len(self.vertices)
            self.verticesMap[v.index] = v.indexRemapped
            self.vertices.append(v)
            Debug('Found new vertex ' + str(v.indexRemapped) + ', was ' + str(v.index))
        self.faces.append(v.indexRemapped)

    def addLine(self, line):
        parts = splitLine(line, self.delimiter)
        if parts:
            if self.lineNumber == 1:
                self.processHeader(parts)
            else:
                self.processLine(parts)
            self.lineNumber = self.lineNumber + 1

    def writeObj(self, f):
        f.write('o object\n')
        f.write('\n# vertex positions\n')
        for vertex in self.vertices:
            vertex.exportObjPosition(f)
        f.write('\n# vertex normals\n')
        for vertex in self.vertices:
            vertex.exportObjNormal(f)
        f.write('\n# vertex texcoords\n')
        for vertex in self.vertices:
            vertex.exportObjTexcoord(f)

        faceCnt = Config.settings['faceCnt']
        print(Config.settings)
        f.write('\n# face data\n')
        prim = []
        for face in self.faces:
            prim.append(face)
            if len(prim) >= faceCnt:
                f.write('f')
                for p in prim:
                    ps = str(p+1)
                    f.write(' ' + ps + '/' + ps + '/' + ps)
                f.write('\n')
                prim = []

##############################################################################
# entry point                                                                #
##############################################################################

Config = Settings()
processor = Processor()

for line in fileinput.input():
    processor.addLine(line)

objFile = open('data.obj',"wt")
processor.writeObj(objFile)
objFile.close()
