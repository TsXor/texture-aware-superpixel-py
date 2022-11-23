import sys
from pathlib import Path
sys.path.append(str(Path(__file__).parent))

from .cyext.TSSuperPixel import TSSuperPixel
from .cyext import Gradient

print(dir(Gradient))

def _shorten_module(mod, attr):
    setattr(mod, attr, getattr(getattr(mod, attr), attr))

for n in ['HybridGradient', 'calcIG', 'calcAngle', 'recalcAngle']:
    _shorten_module(Gradient, n)

del _shorten_module