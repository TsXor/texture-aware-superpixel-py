import sys
from pathlib import Path
sys.path.append(str(Path(__file__).parent.parent))

import numpy as np
from PIL import Image
from TSSP import Gradient

img = np.asarray(Image.open(sys.argv[1]).convert('RGB'))

Amptitude, Xita = Gradient.HybridGradient(img, 3) 

Amptitude_img = np.clip(Amptitude*255, 0, 255).astype(np.uint8) 
Xita_img = np.clip(Xita*255, 0, 255) 

Image.fromarray(Xita_img).show()
Image.fromarray(Amptitude_img).show()