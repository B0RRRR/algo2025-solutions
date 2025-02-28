#!/bin/sh

show_help() {
    echo "Usage: $0 [OPTION] <extension> <max_size>"
    echo "Merge files with a given extension into one if their size is below the specified limit."
    echo
    echo "Mandatory arguments:"
    echo "  <extension>        file extension to search for (e.g., txt, log)"
    echo "  <max_size>         maximum file size in bytes to be merged"
    echo
    echo "Options:"
    echo "  -h, --help         display this help and exit"
}


if [ "$1" == "-h" ] || [ "$1" == "--help" ]; then
    show_help
    exit 0
fi

if [ "$#" -ne 2 ] || ! [[ "$2" =~ ^[0-9]+$ ]]; then
    echo "Usage: $0 [OPTION] <extension> <max_size>"
    echo "$0: error: the following arguments are required: extension, max_size"
    exit 1
fi


EXT="$1"
MAX_SIZE="$2"

OUTPUT_FILE="merged.$EXT"

# Очищаем файл перед объединением (если он уже существует)
> "$OUTPUT_FILE"

# Обрабатываем файлы с указанным расширением
for file in *."$EXT"; do
    [ -e "$file" ] || continue
    
    FILE_SIZE=$(stat -c %s "$file")

    if [ "$FILE_SIZE" -lt "$MAX_SIZE" ]; then
        cat "$file" >> "$OUTPUT_FILE"
        echo -e "\n--- Конец файла $file ---\n" >> "$OUTPUT_FILE"
        echo "Файл $file добавлен в $OUTPUT_FILE"
    fi
done

echo "Объединение завершено. Итоговый файл: $OUTPUT_FILE"