import tkinter as tk
from tkinter import filedialog
import os

def browseFiles():
    filename = filedialog.askopenfilename(initialdir = "/", title = "Select a File", filetypes = (("Hex files", "*.hex*"), ("all files", "*.*")))
    print(f"Selected file: {filename}")
    root.destroy()
    # generate output file path
    output_file_path = os.path.splitext(filename)[0] + "_appendcrc.hex"
    print(f"Output file path: {output_file_path}")
    # read input hex file
    with open(filename, 'r') as f:
        input_lines = f.readlines()
    # calculate CRC and append to each record
    for i, line in enumerate(input_lines):
        # ignore non-data lines
       # if line[7:9] != "00":
       #     continue
        # calculate CRC
        bytes = bytearray.fromhex(line[1:-3])
        crc = 0xFFFF
        for b in bytes:
            crc ^= b
            for _ in range(8):
                if crc & 0x0001:
                    crc >>= 1
                    crc ^= 0xA001
                else:
                    crc >>= 1
        crc_lo = crc & 0xFF
        crc_hi = (crc >> 8) & 0xFF
        # append CRC after checksum byte
        checksum_index = len(line) - 1
        line = line[:checksum_index] + format(crc_hi, '02X') + format(crc_lo, '02X') + line[checksum_index:]
        # update input lines
        input_lines[i] = line
    # write output hex file
    eof = ':'
    with open(output_file_path, 'w') as f:
        f.writelines(input_lines)
        f.write(eof)
    print(f"Output file written to {output_file_path}")

# create tkinter window
root = tk.Tk()
root.withdraw()
# prompt user to select input hex file
browseFiles()
