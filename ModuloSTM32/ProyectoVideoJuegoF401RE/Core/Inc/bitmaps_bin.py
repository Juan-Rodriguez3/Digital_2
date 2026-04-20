# Convierte arrays de bitmaps C a archivos binarios para SD
import re
import struct
import os

# Archivo de entrada
INPUT_FILE = "bitmaps.h"
# Carpeta de salida
OUTPUT_FOLDER = "bins_para_SD"

os.makedirs(OUTPUT_FOLDER, exist_ok=True)

# Leer archivo
with open(INPUT_FILE, "r") as f:
    content = f.read()

# Buscar todos los arrays uint16_t
pattern = r'const\s+uint16_t\s+(\w+)\s*\[.*?\]\s*(?:PROGMEM\s*)?\=\s*\{([^}]+)\}'
matches = re.findall(pattern, content, re.DOTALL)

for name, data in matches:
    # Extraer valores hex
    values = re.findall(r'0x[0-9a-fA-F]+', data)
    print(f"Convirtiendo {name}: {len(values)} pixels")

    # Escribir archivo binario
    output_path = os.path.join(OUTPUT_FOLDER, f"{name}.bin")
    with open(output_path, "wb") as f:
        for v in values:
            # Guardar como big-endian igual que la pantalla espera
            f.write(struct.pack(">H", int(v, 16)))

    print(f"  -> {output_path} ({os.path.getsize(output_path)} bytes)")

print("\nListo. Copia los .bin a la raiz de la SD.")