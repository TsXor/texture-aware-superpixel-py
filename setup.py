from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext as build_ext_orig
from setuptools.command.install import install as install_orig
from Cython.Build import cythonize

from pathlib import Path
import subprocess
import numpy as np
import platform, os
system = platform.system()

MOD_DIR = Path(__file__).parent / 'TSSP'

module_name = MOD_DIR.stem
curdir = MOD_DIR
os.chdir(str(MOD_DIR))

# ----- Extensions START -----

STATIC_LIB_SUFFIX = 'lib' if 'Windows' in system else 'a'
SHELL_SCRIPT_SUFFIX = 'bat' if 'Windows' in system else 'sh'

class RawLib(Extension):
    def __init__(self, name, file_path, build_script_path):
        super().__init__(name, sources=[])
        self.file_path = file_path
        self.build_script_path = build_script_path

class build_ext(build_ext_orig):
    def run(self):
        israw = [isinstance(ext, RawLib) for ext in self.extensions]
        rawext = [ext for i, ext in enumerate(self.extensions) if israw[i]]
        self.extensions = [ext for i, ext in enumerate(self.extensions) if not(israw[i])]
        [self.build_raw(ext) for ext in rawext]
        super().run()
        

    def build_raw(self, ext):
        if ext.file_path.exists():
            print('Library %s already built, skipping...' % ext.file_path.name)
        else:
            print('Building library %s...' % ext.file_path.name)
            subprocess.run(str(ext.build_script_path), shell=True, check=True)
            print('Build library %s done.' % ext.file_path.name)

def get_cy_tree(path, excludes=[]):
    return [Extension(
        cyname, [str(p)],
        extra_compile_args=["-O2", "-march=native", '/openmp']
    ) for p in path.rglob('*.pyx') \
    if (cyname := '%s.%s'%(path.name, '.'.join([*(p.relative_to(path)).parts[:-1], p.stem]))) not in excludes]

extensions = [
    RawLib(
        'TSSP',
        curdir/'cyext'/'cTSSP'/('TSSP.'+STATIC_LIB_SUFFIX),
        curdir/'cyext'/'cTSSP'/('make.'+SHELL_SCRIPT_SUFFIX)
    )
] + cythonize([
    *get_cy_tree(curdir/'cyext', excludes=['cyext.cTSSP.cTSSP']),
    Extension(
        "cyext.cTSSP.cTSSP", [str(curdir/'cyext'/'cTSSP'/'cTSSP.pyx')],
        extra_compile_args=["-O2", "-march=native"],
        libraries=['TSSP'],
        library_dirs=[str(curdir/'cyext'/'cTSSP')]
    ),
])

# ----- Extensions END -----

setup(
    ext_modules=extensions,
    include_dirs=[np.get_include()],
    cmdclass={'build_ext': build_ext},
    packages=[str(MOD_DIR)]
)
