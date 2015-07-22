import sys, os
import clr

try:
    masterApiBasePath=r'C:\Program Files (x86)\Nordic Semiconductor\Master Emulator'
    dirsandfiles = os.listdir(masterApiBasePath)
    dirs = []
    for element in dirsandfiles:
        if os.path.isdir(os.path.join(masterApiBasePath, element)):
            dirs.append(element)
    if len(dirs) == 0: 
        raise Exception('Master Emulator directory not found.')
    dirs.sort()
    masterApiPath = os.path.join(masterApiBasePath, dirs[-1])
    print masterApiPath
    sys.path.append(masterApiPath)
    clr.AddReferenceToFile("MasterEmulator.dll")
except Exception, e:
    raise Exception("Cannot load MasterEmulator.dll")