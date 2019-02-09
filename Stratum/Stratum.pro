TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += \
    StData \
    StVmData \
    StInterfaces \
    StSpace \
    StCompiler \
    StSerialization \
    StVirtualMachine \
    StCore \
    StGui \

StSpace.depends = StData
StVirtualMachine.depends = StInterfaces StData StSpace StSerialization StVmData
StCompiler.depends = StData StVmData StVirtualMachine
StCore.depends = StData StVmData StSpace StInterfaces StVirtualMachine StCompiler
StGui.depends = StData StVmData StSpace StInterfaces StVirtualMachine StCompiler StCore
