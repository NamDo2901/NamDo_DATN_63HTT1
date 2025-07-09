import cv2, os

save = "output_split"
path_input = "output_rotate"

dim = 201

for name_image in os.listdir(path_input):

    name = name_image[:-4]

    if not os.path.exists(os.path.join(save, name)):
        os.mkdir(os.path.join(save, name))

    path_save = os.path.join(save, name)

    img_path = os.path.join(path_input, name_image)
    image = cv2.imread(img_path)

    h, w, c =  image.shape

    step_w = int(w/dim)
    step_h = int(h/dim)

    for i in range(step_w):
        for j in range(step_h):
            rotated = image[i*dim:(i+1)*dim, j*dim:(j+1)*dim, :]

            cv2.imwrite(os.path.join(path_save, f"{name}_{i}_{j}.jpg"), rotated)


    for i in range(step_w):
        rotated = image[h-dim:h, i*dim:(i+1)*dim, :]

        cv2.imwrite(os.path.join(path_save, f"{name}_{step_h}_{i}.jpg"), rotated)


    for i in range(step_h):
        rotated = image[i*dim:(i+1)*dim, w-dim:w, :]

        cv2.imwrite(os.path.join(path_save, f"{name}_{i}_{step_w}.jpg"), rotated)

    rotated = image[h-dim:h, w-dim:w, :]
    cv2.imwrite(os.path.join(path_save, f"{name}_{step_h}_{step_w}.jpg"), rotated)


    

