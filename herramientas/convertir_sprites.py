# Convierte los PNG de Sprites/FrameXFrame a sprites ASCII (.txt) que el juego carga.
# Cada pixel opaco se dibuja en la consola como '█' con su color RGB exacto (sin cambios).
# Formato de salida:
#   SPRITE <nombre>
#   SIZE <ancho> <alto>
#   COLORS <n>
#   <simbolo> <RRGGBB>     (n lineas)
#   PIXELS
#   <alto lineas de ancho simbolos; '.' = transparente>
#   END
import os
import sys
from PIL import Image

SIMBOLOS = ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
            "!#$%&*+-/:;<=>?@[]^_{|}~")

AQUI = os.path.dirname(os.path.abspath(__file__))
TB1 = os.path.normpath(os.path.join(AQUI, "..", ".."))
ORIGEN = os.path.join(TB1, "Sprites", "FrameXFrame", "Sprites_FrameXFrame")
DESTINO = os.path.join(TB1, "código", "Recursos", "Sprites")


def convertir(ruta_png, ruta_txt):
    im = Image.open(ruta_png).convert("RGBA")
    paleta = {}
    filas = []
    for y in range(im.height):
        fila = []
        for x in range(im.width):
            r, g, b, a = im.getpixel((x, y))
            if a == 0:
                fila.append(".")
                continue
            if (r, g, b) not in paleta:
                if len(paleta) >= len(SIMBOLOS):
                    raise ValueError(f"demasiados colores en {ruta_png}")
                paleta[(r, g, b)] = SIMBOLOS[len(paleta)]
            fila.append(paleta[(r, g, b)])
        filas.append("".join(fila))
    nombre = os.path.splitext(os.path.basename(ruta_png))[0]
    os.makedirs(os.path.dirname(ruta_txt), exist_ok=True)
    with open(ruta_txt, "w", encoding="utf-8", newline="\n") as f:
        f.write(f"SPRITE {nombre}\nSIZE {im.width} {im.height}\nCOLORS {len(paleta)}\n")
        for (r, g, b), s in paleta.items():
            f.write(f"{s} {r:02X}{g:02X}{b:02X}\n")
        f.write("PIXELS\n" + "\n".join(filas) + "\nEND\n")


def main():
    destino = sys.argv[1] if len(sys.argv) > 1 else DESTINO
    total = 0
    for raiz, dirs, archivos in os.walk(ORIGEN):
        dirs[:] = [d for d in dirs if d != "_REVISAR"]
        for a in archivos:
            if not a.lower().endswith(".png") or a.startswith("_VISTA_PREVIA"):
                continue
            rel = os.path.relpath(os.path.join(raiz, a), ORIGEN)
            convertir(os.path.join(raiz, a), os.path.join(destino, os.path.splitext(rel)[0] + ".txt"))
            total += 1
    print(f"{total} sprites convertidos en {destino}")


if __name__ == "__main__":
    main()
