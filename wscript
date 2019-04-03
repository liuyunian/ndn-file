# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-
import sys
reload(sys)
sys.setdefaultencoding('utf-8')

def options(opt):
    opt.load(['compiler_c', 'compiler_cxx'])

def configure(conf):
    conf.load(['compiler_c', 'compiler_cxx'])
    conf.check_cfg(package='libndn-cxx', args=['--cflags', '--libs'], uselib_store='NDN_CXX', mandatory=True)

def build(bld):
    bld.program(
        features = 'cxx',
        target='client', # 生成的可执行文件名
        source=bld.path.ant_glob(['src/client/*.cpp']),
        includes = ". ./src/client",
        use='NDN_CXX', # 使用的库名
    )

    bld.program(
        features='cxx',
        target='server',
        source=bld.path.ant_glob(['src/server/*.cpp']),
        includes = ". ./src/server", 
        use='NDN_CXX',
    )
