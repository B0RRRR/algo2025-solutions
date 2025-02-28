# !/usr/bin/env python3
import os
import argparse


def merge_files(extension: str, max_size: int):
    output_file = f"merged.{extension}"

    open(output_file, "w").close()

    for file in os.listdir():
        if file.endswith(f".{extension}") and os.path.isfile(file):
            file_size = os.path.getsize(file)

            if file_size < max_size:
                with open(file, "r", encoding="utf-8") as f_read, open(output_file, "a", encoding="utf-8") as f_write:
                    f_write.write(f_read.read())
                    f_write.write(f"\n--- Конец файла {file} ---\n")
                print(f"Файл {file} добавлен в {output_file}")

    print(f"Объединение завершено. Итоговый файл: {output_file}")



parser = argparse.ArgumentParser(description="Merge files with a given extension into one if their size is below the specified limit.")
parser.add_argument("extension", help="  <extension>  file extension to search for (e.g., txt, log)")
parser.add_argument("max_size", type=int, help="<max_size>  maximum file size in bytes to be merged")
args = pcdarser.parse_args()

merge_files(args.extension, args.max_size)