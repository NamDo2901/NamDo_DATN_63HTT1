import cv2
import os
import numpy as np

input_txt_dir = "results/10"
output_img_dir = os.path.join("rs_image", "pts3", "0")

high = 201
width = 201
size = 3

os.makedirs(output_img_dir, exist_ok=True)

for result_file in os.listdir(input_txt_dir):
    if not result_file.startswith("ts3pfcm_text_") or not result_file.endswith(".txt"):
        continue

    print(f"Processing: {result_file}")

    list_element = result_file.split("_")
    if len(list_element) < 4:
        continue

    name_case = list_element[2] + "_" + list_element[3][:-4]
    result_txt_path = os.path.join(input_txt_dir, result_file)

    list_tmp = []
    with open(result_txt_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    flag_T = False
    for line in lines:
        if "W:" in line:
            break
        if "T:" in line:
            flag_T = True
            continue
        if flag_T:
            tmp = line.split()
            if len(tmp) >= 2:
                try:
                    list_tmp.append(0 if float(tmp[0]) >= float(tmp[1]) else 1)
                except ValueError:
                    continue

    img = np.zeros((high, width, 1), dtype=np.uint8)
    tmp_width = int(width / size)
    tmp_high = int(high / size)

    tmp_count = 0
    for i in range(tmp_high):
        for j in range(tmp_width):
            if tmp_count >= len(list_tmp):
                break
            pixel_value = 0 if list_tmp[tmp_count] == 0 else 255
            img[i * size:(i + 1) * size, j * size:(j + 1) * size] = pixel_value
            tmp_count += 1

    save_path = os.path.join(output_img_dir, f"image_{name_case}.png")
    cv2.imwrite(save_path, img)
    print(f"Saved: {save_path}")
