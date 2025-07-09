import sys
import os
from operator import itemgetter
from itertools import groupby
import argparse
import json
import numpy as np
import random
from skimage.draw import polygon
import cv2

from shapely.geometry import Point
from shapely.geometry.polygon import Polygon


def get_args():
    parser = argparse.ArgumentParser()

    parser.add_argument("--dataset_dir", type=str, default="cvat/test/", help="Directory contains images")
    parser.add_argument("--label_file_path", type=str, default="cvat/annotations/instances_default.json", help="Path to parking spaces annotation file")

    args = parser.parse_args()

    return args


def read_label_file(label_file_path):
    with open(label_file_path, "r") as f:
        json_label = json.load(f)

    return json_label


def parse_json_label(args, json_label):
    assert isinstance(json_label, dict)
    images = json_label["images"]
    annotations = json_label["annotations"]

    assert isinstance(images, list) and isinstance(annotations, list)

    for image_id, items in groupby(images, key=itemgetter("id")):
        print("\nImage id: {}".format(image_id))

        for item in items:
            # print("Path of image id {0} is {1}".format(image_id, item["file_name"]))
            file_name = item["file_name"]
            file_path = os.path.join(args.dataset_dir, file_name)

        parking_spaces = list(filter(lambda x: x["image_id"] == image_id, annotations))
        # print("Parking spaces of image id {0} is {1}".format(image_id, len(parking_spaces)))

        img = cv2.imread(file_path)

        if img is None:
            img = np.zeros(shape=(201, 201, 3), dtype=np.uint8)

        space_id_list = []
        for i, parking_space in enumerate(parking_spaces):
            segmentation = parking_space["segmentation"]
            id = parking_space["id"]

            space_id_list.append(id)
            segmentation = np.array(segmentation, dtype=np.uint16).reshape(-1, 2)

            cc, rr = segmentation.T
            rr, cc = polygon(rr, cc)

            # img[rr, cc] = (0,255,215)

            segmentation = segmentation.tolist()

            # color = (np.random.randint(150, 255), np.random.randint(150, 255), np.random.randint(150, 255))
            color = (0,255,215)

            for j, point in enumerate(segmentation):
                x1, y1, = point
                if j < len(segmentation) - 1:
                    x2, y2 = segmentation[j + 1]
                else:
                    x2, y2 = segmentation[0]

                cv2.line(img, (x1, y1), (x2, y2), color=color, thickness=1)
            
        cv2.imwrite(f"image_{image_id}__.jpg", img)

if __name__ == '__main__':
    root_dir = os.path.abspath(".")
    sys.path.append(root_dir)

    args = get_args()

    json_label = read_label_file(label_file_path=args.label_file_path)

    parse_json_label(args=args, json_label=json_label)
