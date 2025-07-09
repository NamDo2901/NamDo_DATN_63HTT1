import cv2
import os
from glob import glob

input_folder = "origin_images"
output_folder = "output_rotate"

os.makedirs(output_folder, exist_ok=True)

image_paths = glob(os.path.join(input_folder, "*.jpg"))

for source_image in image_paths:

    base_name = os.path.splitext(os.path.basename(source_image))[0]
    
    detination_image = os.path.join(output_folder, f"{base_name}_out.jpg")

    image = cv2.imread(source_image)
    if image is None:
        print(f"Không thể đọc ảnh: {source_image}")
        continue

    image = cv2.resize(image, None, fx=0.2, fy=0.2, interpolation=cv2.INTER_CUBIC)

    (h, w) = image.shape[:2]
    (cX, cY) = (w // 2, h // 2)
    M = cv2.getRotationMatrix2D((cX, cY), 13, 1.0)
    rotated = cv2.warpAffine(image, M, (w, h))

    try:
        rotated = rotated[550:1435, 135:1060, :]
    except:
        print(f"Cant crop : {base_name} ")
        continue

    cv2.imwrite(detination_image, rotated)
