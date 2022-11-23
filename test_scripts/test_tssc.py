import sys
from pathlib import Path
sys.path.append(str(Path(__file__).parent.parent))

import time
from TSSP import TSSuperPixel
import numpy as np
from PIL import Image
img = np.asarray(Image.open(sys.argv[1]).convert('RGB'))

time_start = time.time()
nlabels, labels = TSSuperPixel(img, 1000, 5, 1)
labels = np.asarray(labels)
time_end = time.time()
time_used = time_end - time_start; time_used

palette = np.random.randint(0, high=0xFFFFFF, size=nlabels)
colored = np.empty_like(labels, dtype=np.int32)
for i in range(nlabels):
    colored[labels == i] = palette[i]

h, w = labels.shape
labelimg = np.empty((h, w, 3), dtype=np.uint8)
labelimg[:, :, 0] = (colored & 0xFF0000) >> 4
labelimg[:, :, 1] = (colored & 0x00FF00) >> 2
labelimg[:, :, 2] = (colored & 0x0000FF) >> 0

Image.fromarray(labelimg).show()
Image.fromarray((img*0.7+labelimg*0.3).astype(np.uint8)).show()