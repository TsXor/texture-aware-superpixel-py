import subprocess
from pathlib import Path

def winstr(astr):
    astr = str(astr)
    if len(astr.split(' ')) > 1:
        return '"%s"' % astr
    else:
        return astr

WINSYSDIR = Path(r'C:\Windows\system32')

try:
    gccwhere = subprocess.run('where gcc', shell=True, check=True, stdout=subprocess.PIPE)
except:
    print('"where gcc" fails, maybe mingw is not installed!'); exit(1)

gccplaces = gccwhere.stdout.decode()
gccplaces = gccplaces.split('\r\n')
gccplaces = [Path(p) for p in gccplaces if p]
dllplaces = [p for gccp in gccplaces for p in gccp.parent.glob('*.dll')]
for dllp in dllplaces:
    subprocess.run('mklink %s %s' % (winstr(WINSYSDIR/dllp.name), winstr(dllp)), shell=True)