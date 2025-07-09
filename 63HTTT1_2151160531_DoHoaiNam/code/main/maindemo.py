import tkinter as tk
from tkinter import filedialog, messagebox
from tkinter import ttk
import subprocess
import os
from PIL import Image, ImageTk
import shutil
import threading
import reset

def run_external_process(command):
    try:
        subprocess.run(command, check=True, shell=True)
    except subprocess.CalledProcessError as e:
        messagebox.showerror("Error", f"Error during execution: {e}")
        return False
    return True

def select_image():
    image_path = filedialog.askopenfilename(
        title="Select an image", filetypes=[("Image Files", "*.jpg;*.jpeg;*.png")]
    )
    if not image_path:
        return

    try:
        reset.reset_files()
        original_filename = os.path.basename(image_path)

        os.makedirs("cvat/images", exist_ok=True)
        os.makedirs("origin_images", exist_ok=True)

        cvat_image_path = os.path.join("cvat/images", original_filename)
        shutil.copy(image_path, cvat_image_path)

        img = Image.open(image_path)
        resized_img = img.resize((201, 201))

        origin_image_path = os.path.join("origin_images", "image_1_0.jpg")
        resized_img.save(origin_image_path)

        img_tk = ImageTk.PhotoImage(resized_img)
        origin_image_label.config(image=img_tk)
        origin_image_label.image = img_tk

        origin_frame.pack(side="left", padx=10)

        algo_label.pack(pady=(20, 0))
        algo_combo.pack()
        apply_button.pack(pady=10)

        if not run_external_process("python export_data.py"):
            return

    except Exception as e:
        messagebox.showerror("Error", f"An error occurred: {e}")

def parse_result_csv(csv_path):
    """
    Parse the CSV file and return a dictionary with keys for each metric.
    """
    if not os.path.exists(csv_path):
        return None

    results = {}
    try:
        with open(csv_path, "r") as f:
            lines = f.readlines()
        # Try to find the summary lines at the end of the file
        for line in lines[::-1]:
            line = line.strip()
            if not line:
                continue
            if line.startswith("DB:"):
                results['DB'] = float(line.split(":")[1].strip())
            elif line.startswith("ASW:"):
                results['ASW'] = float(line.split(":")[1].strip())
            elif line.startswith("PBM:"):
                results['PBM'] = float(line.split(":")[1].strip())
            elif line.startswith("IFV:"):
                results['IFV'] = float(line.split(":")[1].strip())
            elif line.startswith("Ca:"):
                results['Ca'] = float(line.split(":")[1].strip())
            elif line.startswith("Ca_label:"):
                results['Ca_label'] = float(line.split(":")[1].strip())
            elif line.startswith("Time:"):
                results['Time'] = float(line.split(":")[1].strip())
            # Stop if all metrics found
            if len(results) == 7:
                break
    except Exception as e:
        results = None
    return results

def format_results(results, algo_name):
    if not results:
        return f"Không tìm thấy kết quả cho {algo_name}."
    out = [f"{algo_name} Kết quả:"]
    out.append(f"DB:    {results.get('DB', 0):.10f}")
    out.append(f"ASW:    {results.get('ASW', 0):.10f}")
    out.append(f"PBM:    {results.get('PBM', 0):.10f}")
    out.append(f"IFV:    {results.get('IFV', 0):.10f}")
    out.append(f"Ca:    {results.get('Ca', 0):.10f}")
    out.append(f"Ca_label:    {results.get('Ca_label', 0):.10f}")
    out.append(f"Time:    {results.get('Time', 0):.10f}")
    return "\n".join(out)

def process_algorithm():
    algo = algo_var.get()

    def task():
        try:
            result_csv_path = ""
            if algo == "CS3FCM":
                if not run_external_process("athcs text_1"):
                    return
                if not run_external_process("python export_img_cs3fcm.py"):
                    return
                result_path = "./rs_image/cs3/0/image_1_0.png"
                blend_path = "./bleanding_img/cs3/0/image_1_0.png"
                result_title.config(text="Result by applying CS3FCM")
                result_csv_path = "results/CS3FCM_result_text_1_0.csv"

            elif algo == "TS3PFCM":
                if not run_external_process("athts text_1"):
                    return
                if not run_external_process("python export_img_ts3pfcm.py"):
                    return
                result_path = "./rs_image/pts3/0/image_1_0.png"
                blend_path = "./bleanding_img/pts3/0/image_1_0.png"
                result_title.config(text="Result by applying TS3PFCM")
                result_csv_path = "results/TS3PFCM_result_text_1_0.csv"

            elif algo == "MTS3PFCM":
                if not run_external_process("athmt text_1"):
                    return
                if not run_external_process("python export_img_mts3pfcm.py"):
                    return
                result_path = "./rs_image/mpts3/0/image_1_0.png"
                blend_path = "./bleanding_img/mpts3/0/image_1_0.png"
                result_title.config(text="Result by applying MTS3PFCM")
                result_csv_path = "results/MTS3PFCM_result_text_1_0.csv"

            else:
                messagebox.showerror("Error", "Unknown algorithm selected.")
                return

            if not run_external_process("python blending.py"):
                return

            if os.path.exists(result_path):
                result_img = Image.open(result_path).resize((201, 201))
                result_img_tk = ImageTk.PhotoImage(result_img)
                result_image_label.config(image=result_img_tk)
                result_image_label.image = result_img_tk
                result_frame.pack(side="left", padx=10)
            else:
                messagebox.showerror("Error", f"Result image not found: {result_path}")
                return

            if os.path.exists(blend_path):
                blend_img = Image.open(blend_path).resize((201, 201))
                blend_img_tk = ImageTk.PhotoImage(blend_img)
                blend_image_label.config(image=blend_img_tk)
                blend_image_label.image = blend_img_tk
                blend_frame.pack(side="left", padx=10)
            else:
                messagebox.showerror("Error", f"Blending image not found: {blend_path}")

            # Load and show result metrics
            results = parse_result_csv(result_csv_path)
            formatted = format_results(results, algo)
            result_metrics_text.config(state="normal")
            result_metrics_text.delete("1.0", tk.END)
            result_metrics_text.insert(tk.END, formatted)
            result_metrics_text.config(state="disabled")
            metrics_frame.pack(side="bottom", anchor="se", padx=20, pady=10)

        except Exception as e:
            messagebox.showerror("Error", f"An error occurred: {e}")

    threading.Thread(target=task).start()

root = tk.Tk()
root.geometry("1100x650")
root.title("Image Processing with Blending")

main_frame = tk.Frame(root)
main_frame.pack(pady=20, fill="both", expand=True)

images_frame = tk.Frame(main_frame)
images_frame.pack(side="left", padx=20)

def create_image_frame(parent, title_text):
    frame = tk.Frame(parent, bd=2, relief="groove", padx=5, pady=5)
    title = tk.Label(frame, text=title_text, font=("Arial", 12))
    title.pack()
    img_label = tk.Label(frame)
    img_label.pack()
    return frame, img_label, title

origin_frame, origin_image_label, origin_title = create_image_frame(images_frame, "Origin Image")
result_frame, result_image_label, result_title = create_image_frame(images_frame, "Result")
blend_frame, blend_image_label, blend_title = create_image_frame(images_frame, "After Blending")

origin_frame.pack_forget()
result_frame.pack_forget()
blend_frame.pack_forget()

control_frame = tk.Frame(main_frame)
control_frame.pack(side="right", padx=30, anchor="n")

select_button = tk.Button(control_frame, text="Select Image", command=select_image, width=20, height=2)
select_button.pack(pady=10)

algo_label = tk.Label(control_frame, text="Choose Algorithm", font=("Arial", 12))
algo_var = tk.StringVar()
algo_combo = ttk.Combobox(control_frame, textvariable=algo_var, state="readonly", width=18, font=("Arial", 11))
algo_combo['values'] = ["CS3FCM", "TS3PFCM", "MTS3PFCM"]
algo_combo.current(0)

apply_button = tk.Button(control_frame, text="Apply", command=process_algorithm, width=20, height=2)

algo_label.pack_forget()
algo_combo.pack_forget()
apply_button.pack_forget()

metrics_frame = tk.Frame(root, bd=2, relief="ridge", padx=10, pady=10)
result_metrics_text = tk.Text(metrics_frame, height=10, font=("Consolas", 11), state="disabled")
result_metrics_text.pack(fill="both", expand=True)
metrics_frame.pack(side="bottom", fill="x", padx=10, pady=10)
root.mainloop()