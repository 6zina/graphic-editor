import re

# Открываем исходный файл для чтения
with open('input_new.txt', 'r') as file:
    # Читаем содержимое файла
    data = file.readlines()

# Создаем список для хранения координат
coordinates = []

# Обрабатываем каждую строку данных
for line in data:
    # Ищем строки, содержащие координаты в формате "PA x,y;" и извлекаем координаты
    match = re.search(r"PA\s*(-?\d+),(-?\d+);", line)
    if match:
        x, y = match.groups()
        coord = f"{x},{y}"
        # Добавляем координаты в список
        coordinates.append(coord)

# Открываем файл для записи результатов в указанной директории
output_file_path = 'C:/projects_qt/build-rotate_figure-Desktop_x86_windows_msvc2017_pe_64bit-Debug/output_new.txt'
with open(output_file_path, 'w') as file:
    # Записываем координаты в новый файл без пустых строк
    file.write("\n".join(coordinates))