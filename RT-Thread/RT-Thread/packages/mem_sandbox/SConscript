Import('rtconfig')
from building import *

cwd     = GetCurrentDir()
src     = Glob('*.c')
CPPPATH = [cwd]

group = DefineGroup('mem_sandbox', src, depend = ['PKG_USING_MEM_SANDBOX'], CPPPATH = CPPPATH)

Return('group')
