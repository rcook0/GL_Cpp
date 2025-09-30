from pathlib import Path
from PIL import Image

folder = Path("./frames")
out_ext = "png"   # or "jpg"

for ppm in folder.glob("*.ppm"):
    img = Image.open(ppm)
    out_file = ppm.with_suffix("." + out_ext)
    img.save(out_file)
    print(f"Converted {ppm} -> {out_file}")
