#!/bin/bash

# путь к исполняемому файлу в bin 
SOURCE_EXE="./bin/ChessGame"

# папка с полным билдом
RELEASE_DIR="./bin"

# путь для размещения библиотек
LIB_DIR="$RELEASE_DIR/lib"

# проверка на наличие исполняемого файла
if [ ! -f "$SOURCE_EXE" ]; then
    echo "Ошибка: $SOURCE_EXE не найден! Сначала соберите проект через cmake --build."
    exit 1
fi

# создание папки lib
mkdir -p "$LIB_DIR"

echo "Анализ зависимостей для $SOURCE_EXE..."

# определение зависимостей бинарника через ldd и выбор только нужных
# исключение базовых библиотек Linux
libs=$(ldd "$SOURCE_EXE" | grep "=> /" | grep -vE "linux-vdso|ld-linux|libc.so|libdl.so|libpthread.so|libm.so|libGL.so|libdrm.so" | awk '{print $3}')

for lib in $libs; do
    # копирование найденных .so файлов из системных папок в bin/lib
    # -L - для копии тела библиотеки, а не просто ссылки
    cp -v -L "$lib" "$LIB_DIR/"
done

echo "------------------------------------------------"
echo "Сборка библиотек завершена!"
echo "Теперь папка $RELEASE_DIR содержит всё необходимое для запуска на другом ПК."