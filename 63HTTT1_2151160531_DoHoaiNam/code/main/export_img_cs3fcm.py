import cv2
import os
import numpy as np

path_ath_result = os.path.join("results", "1")
path_save = os.path.join("rs_image", "cs3")

high = 201
width = 201

for result_file in os.listdir(path_ath_result):
    if not result_file.startswith("CS3FCM_text_") or not result_file.endswith(".txt"):
        continue

    list_element = result_file.split("_")
    if len(list_element) < 4:
        continue

    name_case = list_element[2]
    percent_error = list_element[3][:-4]
    print(f"Processing: {result_file} | case: {name_case} | %error: {percent_error}")

    path_save_img = os.path.join(path_save, percent_error)
    os.makedirs(path_save_img, exist_ok=True)

    list_tmp = []

    result_txt = os.path.join(path_ath_result, result_file)
    with open(result_txt, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    length = len(lines)
    print(f"Total lines: {length}")

    for idx, line in enumerate(lines):
        if idx <= 7 or idx >= length - 6:
            continue
        tmp = line.split()
        if len(tmp) < 2:
            continue
        try:
            list_tmp.append(0 if float(tmp[0]) >= float(tmp[1]) else 1)
        except ValueError:
            continue

    img = np.zeros((high, width, 1), dtype=np.uint8)
    size = 3
    tmp_width = int(width / size)
    tmp_high = int(high / size)

    tmp_count = 0
    for i in range(tmp_high):
        for j in range(tmp_width):
            if tmp_count >= len(list_tmp):
                break

            if name_case in ["1", "3", "5"]:
                pixel_value = 0 if list_tmp[tmp_count] == 0 else 255
            else:
                pixel_value = 255 if list_tmp[tmp_count] == 0 else 0

            img[i * size:(i + 1) * size, j * size:(j + 1) * size] = pixel_value
            tmp_count += 1

    img_name = f"image_{name_case}_0.png"
    cv2.imwrite(os.path.join(path_save_img, img_name), img)
    print(f"Image saved at: {os.path.join(path_save_img, img_name)}")
