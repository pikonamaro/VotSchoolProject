import os
import subprocess
import shutil
import sys

file_list = ["boot","kernel"]

file_targets = {
    "boot": "bootup",
    "kernel": "system"
}

build_script = "./build.sh"
input_dir = "os-sorce"
output_dir = "os-compiled"

def run_builds():
    for folder in [output_dir, "junk"]:
        if os.path.exists(folder):
            shutil.rmtree(folder)
        os.makedirs(folder)

    for name in file_list:
        subpath = file_targets[name]
        src_path = os.path.join(input_dir, subpath, f"{name}.cpp")
        if not os.path.exists(src_path):
            print(f"Source file not found: {src_path}")
            continue

        print(f"\nBuilding {name} from {src_path}")

        shutil.copy(src_path, f"{name}.cpp")

        try:
            subprocess.run([build_script, name], check=True, capture_output=True, text=True)
        except subprocess.CalledProcessError as e:
            print(f"Build failed for {name}:\n{e.stderr}")
            continue
        finally:
            os.remove(f"{name}.cpp")  


        bin_file = f"{name}.bin"
        if os.path.exists(bin_file):
            target_path = os.path.join(output_dir, subpath)
            os.makedirs(target_path, exist_ok=True)
            shutil.move(bin_file, os.path.join(target_path, bin_file))
            print(f"{bin_file} placed in {target_path}")
        else:
            print(f"No .bin file found for {name}")

def get_usb_mounts():
    mounts = []
    with open("/proc/mounts", "r") as f:
        for line in f:
            parts = line.split()
            device, mountpoint = parts[0], parts[1]
            if "/dev/sd" in device and "/media/" in mountpoint:
                mounts.append(mountpoint)
    return mounts

def copy_output_to_usb():
    usb_mounts = get_usb_mounts()
    if not usb_mounts:
        print("No USB storage devices found.")
        sys.exit(1)

    usb_target = usb_mounts[0]
    print(f"\nCopying to USB: {usb_target}")

    try:
        for root, dirs, files in os.walk(output_dir):
            rel_path = os.path.relpath(root, output_dir)
            target_dir = os.path.join(usb_target, rel_path)
            os.makedirs(target_dir, exist_ok=True)
            for file in files:
                src_file = os.path.join(root, file)
                dst_file = os.path.join(target_dir, file)
                shutil.copy2(src_file, dst_file)
        print("All files copied successfully to USB.")
    except Exception as e:
        print(f"Error copying to USB: {e}")
        sys.exit(1)

if __name__ == "__main__":
    run_builds()
    copy_output_to_usb()
