import cv2
import os
from glob import glob

input_folder = 'origin_images_tiff'
output_folder = 'origin_images_png'

os.makedirs(output_folder, exist_ok=True)

tiff_files = glob(os.path.join(input_folder, '*.tif')) + glob(os.path.join(input_folder, '*.tiff'))

for tiff_path in tiff_files:

    img = cv2.imread(tiff_path)

    if img is None:
        print(f"Cant read: {tiff_path}")
        continue

    base_name = os.path.splitext(os.path.basename(tiff_path))[0]

    jpg_path = os.path.join(output_folder, base_name + '.jpg')

    cv2.imwrite(jpg_path, img)
