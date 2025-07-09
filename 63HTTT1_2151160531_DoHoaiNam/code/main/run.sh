gcc mts3pfcm.c -o athmt -lm
gcc cs3fcm.c -o athcs -lm
gcc ts3pfcm.c -o athts -lm

athmt text_1.txt


run rotate.py  (xoay anh ve huong thang)
run split_img.py   (chia nho anh ve kich thuoc 201 201)

Chon anh de dua vao gan nhan

run export_data.py  (tao file train)

hiện tại chỉ lấy được ảnh đã gán nhãn