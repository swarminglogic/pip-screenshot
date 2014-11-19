import os

env = Environment(ENV = {'PATH' : os.environ['PATH'],
                         'HOME' : os.environ['HOME'],
                         'TERM' : 'xterm'},
                  CXX='g++',
                  tools=['default'], toolpath=[''])

VariantDir('build', 'src')
Export('env')
SConscript('build/SConscript')
