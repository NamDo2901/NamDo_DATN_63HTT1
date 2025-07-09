import os

def reset_files():
    folders_to_clean = ['bleanding_img','origin_images','cvat/images', 'data', 'results', 'rs_image']

    current_dir = os.path.abspath(os.getcwd())

    for folder in folders_to_clean:
        full_path = os.path.abspath(folder)

        if full_path == current_dir:
            print(f"Skip this folder: {full_path}")
            continue

        if not os.path.exists(full_path):
            print(f"[!] Not exist : {full_path}")
            continue

        for root, dirs, files in os.walk(full_path):
            for file in files:
                file_path = os.path.join(root, file)
                try:
                    os.remove(file_path)
                    print(f"Deleted: {file_path}")
                except Exception as e:
                    print(f"Cant delete {file_path}: {e}")

if __name__ == "__main__":
    reset_files()
