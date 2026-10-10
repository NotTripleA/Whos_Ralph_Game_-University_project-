# Genera el sprite del titulo "WHO'S RALPH?" con letras de bloques al estilo del logo
# de Ralph el Demoledor (bloques que se "rompen", degradado amarillo-rojo, contorno y relieve).
# Salida: Recursos/Sprites/Juego/titulo_*.txt (mismo formato que convertir_sprites.py)
import os
import random
from PIL import Image

AQUI = os.path.dirname(os.path.abspath(__file__))
SALIDA = os.path.join(AQUI, "..", "Recursos", "Sprites", "Juego")

LETRAS = {
    "W": ["XX...XX", "XX...XX", "XX...XX", "XX.X.XX", "XXXXXXX", "XXX.XXX", "XX...XX"],
    "H": ["XX...XX", "XX...XX", "XX...XX", "XXXXXXX", "XX...XX", "XX...XX", "XX...XX"],
    "O": [".XXXXX.", "XX...XX", "XX...XX", "XX...XX", "XX...XX", "XX...XX", ".XXXXX."],
    "'": ["XX", "XX", ".X", "X.", "..", "..", ".."],
    "S": [".XXXXXX", "XX.....", "XX.....", ".XXXXX.", ".....XX", ".....XX", "XXXXXX."],
    "R": ["XXXXXX.", "XX...XX", "XX...XX", "XXXXXX.", "XX.XX..", "XX..XX.", "XX...XX"],
    "A": ["..XXX..", ".XX.XX.", "XX...XX", "XX...XX", "XXXXXXX", "XX...XX", "XX...XX"],
    "L": ["XX.....", "XX.....", "XX.....", "XX.....", "XX.....", "XX.....", "XXXXXXX"],
    "P": ["XXXXXX.", "XX...XX", "XX...XX", "XXXXXX.", "XX.....", "XX.....", "XX....."],
    "?": [".XXXXX.", "XX...XX", ".....XX", "...XXX.", "...XX..", ".......", "...XX.."],
    " ": ["...", "...", "...", "...", "...", "...", "..."],
}

# Degradado del logo: amarillo arriba -> naranja -> rojo -> rojo oscuro abajo
DEGRADADO = [(255, 214, 64), (255, 170, 40), (255, 120, 30), (236, 64, 28), (200, 30, 24), (150, 18, 20)]
CONTORNO = (34, 8, 4)
RELIEVE = (92, 22, 10)


def aclarar(c, f):
    return tuple(min(255, int(v + (255 - v) * f)) for v in c)


def oscurecer(c, f):
    return tuple(int(v * (1 - f)) for v in c)


def color_fila(fy):
    # Bandas de color (estilo pixel art, sin mezclas suaves)
    return DEGRADADO[min(len(DEGRADADO) - 1, int(fy * len(DEGRADADO)))]


def bloques_linea(texto, unidad, x0, y0, sep=1):
    """Devuelve lista de bloques (x, y, unidad, alto_total, y0) de una linea de texto."""
    bloques = []
    x = x0
    for ch in texto:
        g = LETRAS[ch]
        for gy, fila in enumerate(g):
            for gx, c in enumerate(fila):
                if c == "X":
                    bloques.append((x + gx * unidad, y0 + gy * unidad, unidad, 7 * unidad, y0))
        x += (len(g[0]) + sep) * unidad
    ancho = x - x0 - sep * unidad
    return bloques, ancho


def construir(lineas, semilla, nombre, ladrillo=4, escombros=10):
    rnd = random.Random(semilla)
    # Cada linea: (texto, unidad). Se centran horizontalmente.
    medidas = []
    for texto, u in lineas:
        _, w = bloques_linea(texto, u, 0, 0)
        medidas.append(w)
    ancho = max(medidas) + 10
    todos = []
    y = 2
    for (texto, u), w in zip(lineas, medidas):
        b, _ = bloques_linea(texto, u, (ancho - w) // 2, y)
        todos += b
        y += 7 * u + 3
    alto = y + 8

    # Relleno de cada letra: degradado vertical del logo
    cara = {}
    origen = {}
    for bx, by, u, h, oy in todos:
        for py in range(u):
            for px in range(u):
                fy = min(1.0, max(0.0, (by + py - oy) / max(1, h - 1)))
                cara[(bx + px, by + py)] = color_fila(fy)
                origen[(bx + px, by + py)] = (oy, h)
    # Brillo en el borde superior / izquierdo y sombra en el inferior / derecho (bloque 3D)
    final = {}
    for (x, y), c in cara.items():
        if (x, y - 1) not in cara:
            c = aclarar(c, 0.55)
        elif (x - 1, y) not in cara:
            c = aclarar(c, 0.22)
        elif (x, y + 1) not in cara:
            c = oscurecer(c, 0.35)
        elif (x + 1, y) not in cara:
            c = oscurecer(c, 0.18)
        else:
            # Juntas de ladrillo sutiles (Ralph destruye ladrillos)
            oy, h = origen[(x, y)]
            fila = (y - oy) // ladrillo
            if (y - oy) % ladrillo == ladrillo - 1 or (x + (fila % 2) * (ladrillo // 2 + 1)) % (ladrillo * 2 - 1) == 0:
                c = oscurecer(c, 0.14)
        final[(x, y)] = c
    cara = final
    # Escombros: pedacitos que saltan bajo algunas letras (solo decoracion, no tapan letras)
    bordes = [(x, y) for (x, y) in cara if (x, y + 1) not in cara and (x, y + 2) not in cara]
    rnd.shuffle(bordes)
    for (x, y) in bordes[:escombros]:
        dx, dy = rnd.randint(-3, 3), rnd.randint(3, 6)
        t = rnd.choice([1, 2])
        col = color_fila(rnd.uniform(0.4, 1.0))
        for a in range(t):
            for b in range(t):
                p = (x + dx + a, y + dy + b)
                if all((p[0] + i, p[1] + j) not in final for i in (-1, 0, 1) for j in (-1, 0, 1)):
                    cara[p] = col

    img = Image.new("RGBA", (ancho, alto), (0, 0, 0, 0))
    # Relieve (sombra 3D hacia abajo-derecha)
    for d in (2, 1):
        for (x, y) in cara:
            if 0 <= x + d < ancho and 0 <= y + d < alto:
                img.putpixel((x + d, y + d), RELIEVE + (255,))
    # Contorno
    for (x, y) in cara:
        for dx in (-1, 0, 1):
            for dy in (-1, 0, 1):
                p = (x + dx, y + dy)
                if p not in cara and 0 <= p[0] < ancho and 0 <= p[1] < alto:
                    img.putpixel(p, CONTORNO + (255,))
    for (x, y), c in cara.items():
        img.putpixel((x, y), c + (255,))
    bbox = img.getbbox()
    img = img.crop(bbox)
    guardar(img, nombre)
    return img


def guardar(img, nombre):
    simbolos = ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
                "!#$%&*+-/:;<=>?@[]^_{|}~")
    paleta, filas = {}, []
    for y in range(img.height):
        f = ""
        for x in range(img.width):
            r, g, b, a = img.getpixel((x, y))
            if a == 0:
                f += "."
                continue
            if (r, g, b) not in paleta:
                paleta[(r, g, b)] = simbolos[len(paleta)]
            f += paleta[(r, g, b)]
        filas.append(f)
    os.makedirs(SALIDA, exist_ok=True)
    with open(os.path.join(SALIDA, nombre + ".txt"), "w", encoding="utf-8", newline="\n") as fh:
        fh.write(f"SPRITE {nombre}\nSIZE {img.width} {img.height}\nCOLORS {len(paleta)}\n")
        for (r, g, b), s in paleta.items():
            fh.write(f"{s} {r:02X}{g:02X}{b:02X}\n")
        fh.write("PIXELS\n" + "\n".join(filas) + "\nEND\n")
    print(nombre, img.size, len(paleta), "colores")


if __name__ == "__main__":
    a = construir([("WHO'S", 2), ("RALPH?", 3)], 7, "titulo_grande", 5, 14)
    b = construir([("WHO'S RALPH?", 2)], 11, "titulo", 4, 12)
    import sys
    if len(sys.argv) > 1:
        for img, n in ((a, "titulo_grande"), (b, "titulo")):
            fondo = Image.new("RGBA", img.size, (12, 14, 40, 255))
            fondo.alpha_composite(img)
            fondo.resize((img.width * 4, img.height * 4), Image.NEAREST).save(os.path.join(sys.argv[1], n + ".png"))
