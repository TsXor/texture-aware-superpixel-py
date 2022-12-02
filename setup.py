from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext as build_ext_orig
from setuptools.command.install import install as install_orig
from Cython.Build import cythonize

from pathlib import Path
import subprocess
import numpy as np
import platform, os
system = platform.system()

PROJECT_DIR = Path(__file__).parent
MOD_DIR = PROJECT_DIR / 'TSSP'

module_name = MOD_DIR.stem
curdir = MOD_DIR
os.chdir(str(MOD_DIR))

# ----- Extensions START -----
IS_WINDOWS = 'Windows' in system
SHELL_SCRIPT_SUFFIX = 'bat' if IS_WINDOWS else 'sh'

class RawLib(Extension):
    def __init__(self, name, file_path, build_script_path):
        super().__init__(name, sources=[])
        self.file_path = file_path
        self.build_script_path = build_script_path

class build_ext(build_ext_orig):
    def run(self):
        self.IS_MSVC = 'msvc' in str(self.compiler).lower()
        self.OMP_ARG = ['/openmp'] if self.IS_MSVC else ['-fopenmp']
        self.STATIC_LIB_SUFFIX = 'lib' if self.IS_MSVC else 'a'
        for ext in self.extensions: ext.extra_compile_args.extend(self.OMP_ARG); \
                                    ext.extra_link_args.extend(self.OMP_ARG);
        israw = [isinstance(ext, RawLib) for ext in self.extensions]
        rawext = [ext for i, ext in enumerate(self.extensions) if israw[i]]
        self.extensions = [ext for i, ext in enumerate(self.extensions) if not(israw[i])]
        [self.build_raw(ext) for ext in rawext]
        super().run()
        

    def build_raw(self, ext):
        LIB_PREFIX = '' if self.IS_MSVC else 'lib'
        fpath = ext.file_path.parent / (LIB_PREFIX + ext.file_path.name % self.STATIC_LIB_SUFFIX)
        if fpath.exists():
            print('Library %s already built, skipping...' % fpath.name)
        else:
            print('Building library %s...' % fpath.name)
            subprocess.run(str(ext.build_script_path), shell=True, check=True)
            print('Build library %s done.' % fpath.name)

def get_cy_tree(path, excludes=[]):
    return [Extension(
        cyname, [str(p)],
        extra_compile_args=["-O2", "-march=native"]
    ) for p in path.rglob('*.pyx') \
    if (cyname := '%s.%s'%(path.name, '.'.join([*(p.relative_to(path)).parts[:-1], p.stem]))) not in excludes]

extensions = [
    RawLib(
        'TSSP',
        PROJECT_DIR/'csrc'/('TSSP.%s'),
        PROJECT_DIR/'csrc'/('make.'+SHELL_SCRIPT_SUFFIX)
    ),
] + cythonize([
    *get_cy_tree(curdir/'cyext', excludes=['cyext.cTSSP.cTSSP']),
    Extension(
        "cyext.cTSSP.cTSSP", [str(curdir/'cyext'/'cTSSP'/'cTSSP.pyx')],
        extra_compile_args=["-O2", "-march=native"],
        libraries=['TSSP'],
        library_dirs=[str(PROJECT_DIR/'csrc')],
        include_dirs=[str(PROJECT_DIR/'csrc')]
    ),
])

# ----- Extensions END -----

setup(
    ext_modules=extensions,
    include_dirs=[np.get_include()],
    cmdclass={'build_ext': build_ext},
    packages=[str(MOD_DIR)]
)
