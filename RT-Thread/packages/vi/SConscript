Import('rtconfig')
from building import *

src     = Glob('*.c')
group = DefineGroup('vi', src, depend = ['PKG_USING_VI'])
Return('group')
