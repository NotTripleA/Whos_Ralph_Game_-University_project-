# Disenos simples (estilo Fix-It Felix Jr.) para los personajes de Hero's Duty, hechos a partir de
# los personajes "a escala" (Sprites/02_Vanellope_y_personajes/Personajes_escala), igual que en la boda:
#   - Calhoun: la Calhoun a escala, con cuadros de caminar (piernas animadas) y de disparo (rifle)
#   - Marines: la misma figura con casco y armadura verde (dos variantes de piel)
#   - Ralph con armadura: el Ralph a escala con armadura verde (con y sin casco)
# Salida: Recursos/SpritesMejorados/04_Heros_Duty/... (reemplazan a los disenos muy detallados)
import os

AQUI = os.path.dirname(os.path.abspath(__file__))
ORIGEN = os.path.join(AQUI, "..", "Recursos", "Sprites")
DESTINO = os.path.join(AQUI, "..", "Recursos", "SpritesMejorados", "04_Heros_Duty")
ESCALA = os.path.join(ORIGEN, "02_Vanellope_y_personajes", "Personajes_escala")
SIMBOLOS = ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
            "!#$%&*+-/:;<=>?@[]^_{|}~")


def hexrgb(h):
    return (int(h[0:2], 16), int(h[2:4], 16), int(h[4:6], 16))


def leer(ruta):
    lineas = open(ruta, encoding="utf-8").read().splitlines()
    w, h = map(int, lineas[1].split()[1:3])
    n = int(lineas[2].split()[1])
    pal = {l[0]: hexrgb(l[2:8]) for l in lineas[3:3 + n]}
    img = []
    for f in lineas[4 + n:4 + n + h]:
        img.append([pal[c] if c != "." else None for c in f[:w].ljust(w, ".")])
    return img


def guardar(img, nombre, rel, destino=None):
    h, w = len(img), len(img[0])
    paleta, filas = {}, []
    for fila in img:
        s = []
        for c in fila:
            if c is None:
                s.append(".")
                continue
            if c not in paleta:
                paleta[c] = SIMBOLOS[len(paleta)]
            s.append(paleta[c])
        filas.append("".join(s))
    ruta = os.path.join(destino or DESTINO, rel + ".txt")
    os.makedirs(os.path.dirname(ruta), exist_ok=True)
    with open(ruta, "w", encoding="utf-8", newline="\n") as f:
        f.write(f"SPRITE {nombre}\nSIZE {w} {h}\nCOLORS {len(paleta)}\n")
        for (r, g, b), s in paleta.items():
            f.write(f"{s} {r:02X}{g:02X}{b:02X}\n")
        f.write("PIXELS\n" + "\n".join(filas) + "\nEND\n")


def copia(img):
    return [list(f) for f in img]


def recolor(img, mapa, filas=None):
    out = copia(img)
    for y, fila in enumerate(out):
        if filas and not (filas[0] <= y < filas[1]):
            continue
        for x, c in enumerate(fila):
            if c in mapa:
                fila[x] = mapa[c]
    return out


def ancho_extra(img, izq, der):
    return [[None] * izq + list(f) + [None] * der for f in img]


def pierna(img, desde, lado, sube):
    # levanta una pierna (lado -1 = izquierda, 1 = derecha) "sube" pixeles: efecto de caminar
    h, w = len(img), len(img[0])
    cols = [x for y in range(desde, h) for x in range(w) if img[y][x] is not None]
    if not cols:
        return copia(img)
    centro = (min(cols) + max(cols)) / 2.0
    out = copia(img)
    for y in range(desde, h):
        for x in range(w):
            if (x < centro) == (lado < 0):
                out[y][x] = None
    for y in range(desde, h):
        for x in range(w):
            if (x < centro) == (lado < 0) and img[y][x] is not None:
                ny = y - sube
                if ny >= desde - sube:
                    out[ny][x] = img[y][x]
    return out


def rifle(img, x0, y0, largo, fogonazo=0):
    out = copia(img)
    gris, oscuro, luz = (70, 74, 86), (28, 30, 38), (140, 146, 160)
    for i in range(largo):
        x = x0 + i
        if 0 <= x < len(out[0]):
            out[y0][x] = oscuro if i % 5 == 0 else gris
            out[y0 + 1][x] = oscuro
            if i < 6:
                out[y0 + 2][x] = gris   # culata
    out[y0 - 1][x0 + largo - 4] = luz          # mira
    if fogonazo:
        fx = x0 + largo
        amar, nar = (255, 240, 120), (255, 150, 40)
        pts = [(0, 0), (1, 0), (1, -1), (1, 1), (2, 0)] + ([(2, -1), (2, 1), (3, 0), (3, -2), (3, 2)] if fogonazo > 1 else [])
        for dx, dy in pts:
            if 0 <= fx + dx < len(out[0]) and 0 <= y0 + dy < len(out):
                out[y0 + dy][fx + dx] = amar if dx < 2 else nar
    return out


# ---------------------------------------------------------------- Calhoun
def calhoun():
    base = leer(os.path.join(ESCALA, "calhoun.txt"))
    caderas = 38
    ciclo = {1: (0, 0), 2: (-1, 2), 3: (0, 0), 4: (1, 2), 5: (-1, 2), 6: (0, 0), 7: (1, 2)}
    for n, (lado, sube) in ciclo.items():
        img = pierna(base, caderas, lado, sube) if sube else copia(base)
        guardar(img, f"calhoun_caminar_{n:02d}", f"Calhoun/caminar/calhoun_caminar_{n:02d}")
    # disparando: rifle a la derecha; se agrega margen a la izquierda para que al voltearla quede centrada
    ancho = ancho_extra(base, 12, 12)
    for n, fog in ((1, 0), (2, 1), (3, 2), (4, 0)):
        img = rifle(ancho, 26, 23, 20, fog)
        guardar(img, f"calhoun_disparar_{n:02d}", f"Calhoun/disparar/calhoun_disparar_{n:02d}")


# ---------------------------------------------------------------- Marines
def marines():
    base = leer(os.path.join(ESCALA, "calhoun.txt"))
    pelo = [hexrgb(h) for h in ("FFFFAA", "B66D00", "B66D55")]
    armadura = {hexrgb("000000"): (34, 44, 24), hexrgb("484855"): (84, 104, 52), hexrgb("9191AA"): (130, 150, 90),
                hexrgb("4848AA"): (34, 44, 24), hexrgb("DA2400"): (160, 140, 60), hexrgb("DA00AA"): (110, 60, 50)}
    pieles = [None, {hexrgb("FFB6AA"): (200, 140, 100), hexrgb("B66D55"): (140, 90, 60)}]
    cascos = [((96, 120, 60), (60, 78, 36)), ((110, 112, 70), (70, 72, 40))]
    for fila in (1, 2):
        piel = pieles[fila - 1]
        cuerpo = recolor(base, armadura, (15, 54))
        if piel:
            cuerpo = recolor(cuerpo, piel, (5, 16))
        # casco: todo el pelo se vuelve un casco verde redondo
        claro, oscuro = cascos[fila - 1]
        for y in range(0, 14):
            for x in range(len(cuerpo[0])):
                c = base[y][x]
                if c == pelo[0] or c == pelo[1] or (c == pelo[2] and y < 9):
                    cuerpo[y][x] = claro if y >= 6 else oscuro
        for x in range(4, 16):
            if cuerpo[5][x] is None:
                cuerpo[5][x] = oscuro
        for x in range(3, 17):   # visera
            cuerpo[9][x] = oscuro if cuerpo[9][x] is not None or 3 < x < 16 else None
        cuerpo = ancho_extra(cuerpo, 8, 8)
        cuerpo = rifle(cuerpo, 18, 23, 14, 0)
        ciclo = [(0, 0), (-1, 2), (0, 0), (1, 2)]
        for n in range(1, 17):
            lado, sube = ciclo[(n - 1) % 4]
            img = pierna(cuerpo, 38, lado, sube) if sube else copia(cuerpo)
            guardar(img, f"marine_fila_{fila}_{n:02d}", f"Marines/fila_{fila}/marine_fila_{fila}_{n:02d}")


# ---------------------------------------------------------------- Ralph con armadura
def ralph():
    base = leer(os.path.join(ESCALA, "ralph.txt"))
    armadura = {hexrgb("FF4855"): (126, 160, 92), hexrgb("DA4800"): (94, 124, 64), hexrgb("DA2400"): (74, 104, 52),
                hexrgb("910000"): (58, 58, 70), hexrgb("6D0000"): (40, 40, 50), hexrgb("DA6D55"): (150, 150, 160)}
    k = {hexrgb("DA4855"): (110, 140, 80)}     # este color es la boca en la cara y tela mas abajo
    cuerpo = recolor(recolor(base, armadura, (7, 54)), k, (14, 54))
    abierta = recolor(cuerpo, {hexrgb("DA4855"): (90, 20, 20)}, (9, 14))
    # victoria sin casco (01 normal, 02 hablando)
    for n in range(1, 5):
        img = cuerpo if n % 2 else abierta
        guardar(img, f"ralph_victoria_{n:02d}", f"Ralph_armadura/victoria_sin_casco/ralph_victoria_{n:02d}")
    # con casco: el pelo se tapa con un casco verde
    casco = copia(cuerpo)
    pelo = hexrgb("B62400")
    for y in range(0, 9):
        for x in range(len(casco[0])):
            if base[y][x] == pelo:
                casco[y][x] = (96, 124, 66) if y > 2 else (70, 92, 48)
    for x in range(13, 30):
        if casco[6][x] is not None:
            casco[6][x] = (50, 64, 36)    # borde del casco sobre la frente
    ciclo = [(0, 0), (-1, 2), (0, 0), (1, 2), (0, 0)]
    for n in range(1, 6):
        lado, sube = ciclo[n - 1]
        img = pierna(casco, 42, lado, sube) if sube else copia(casco)
        guardar(img, f"ralph_armadura_f1_{n:02d}", f"Ralph_armadura/fila_1/ralph_armadura_f1_{n:02d}")
    # asustado: agachado un pixel (temblando)
    agachado = [[None] * len(casco[0])] + copia(casco)[:-1]
    for n in range(1, 5):
        guardar(casco if n % 2 else agachado, f"ralph_armadura_f2_{n:02d}", f"Ralph_armadura/fila_2/ralph_armadura_f2_{n:02d}")


# ---------------------------------------------------------------- Turbo (Sugar Rush)
# Hecho a partir del Felix a escala: casco blanco con franja roja, traje rojo con franja blanca,
# guantes y botas blancas. La cara queda a la vista, como los demas personajes de la boda.
DESTINO_SR = os.path.join(AQUI, "..", "Recursos", "SpritesMejorados", "03_Sugar_Rush", "Turbo")


def turbo():
    base = leer(os.path.join(ESCALA, "felix.txt"))
    h, w = len(base), len(base[0])
    blanco, gris, rojo, rojo_osc = (250, 244, 240), (200, 194, 192), (200, 40, 35), (140, 26, 24)
    piel, piel_osc, pupila = (226, 158, 122), (168, 98, 72), (20, 15, 15)
    A, B, C, D, E, F, G, H, I, J, K = (hexrgb(x) for x in ("0024AA", "FF9100", "914800", "0091FF", "6D4855", "B66D55",
                                                           "FFB6AA", "FFFFFF", "DA4855", "6DB6FF", "B66D00"))
    img = copia(base)
    for y in range(h):
        for x in range(w):
            c = base[y][x]
            if c is None:
                continue
            brazo = (37 <= y <= 43) and (x < 5 or x > 11)
            if y <= 25:                                   # casco (gorra y pelo)
                img[y][x] = {A: blanco, B: rojo, C: rojo_osc, D: rojo, E: blanco, F: piel_osc}.get(c, c)
            elif y <= 33:                                 # cara: lados del casco, ojos, nariz y boca
                if c == E and x >= 10:
                    img[y][x] = gris if y > 30 else blanco
                elif c == E:
                    img[y][x] = (120, 50, 40)
                else:
                    img[y][x] = {G: piel, F: piel_osc, A: pupila, I: (220, 150, 130)}.get(c, c)
            elif brazo:                                   # mangas rojas y guantes blancos
                img[y][x] = rojo if y <= 40 else blanco
            elif y <= 43:                                 # traje, cinturon
                img[y][x] = {D: rojo, J: blanco, G: piel, F: piel_osc, C: rojo_osc, K: rojo}.get(c, c)
            elif y <= 50:                                 # pantalon
                img[y][x] = {A: rojo}.get(c, c)
            else:                                         # medias y botas
                img[y][x] = {D: blanco, K: gris}.get(c, c)
    for y in range(36, 44):                               # franja blanca del traje
        if img[y][8] is not None:
            img[y][8] = blanco
    for x in range(5, 12):                                # antiparras sobre el casco
        if img[23][x] is not None:
            img[23][x] = (90, 90, 100) if x in (5, 11) else (150, 190, 210)
    # contorno gris del casco blanco (para que no se pegue con la cara)
    for y in range(18, 34):
        for x in range(w):
            if img[y][x] in (blanco, gris):
                vecinos = [img[yy][xx] if 0 <= yy < h and 0 <= xx < w else None for yy, xx in ((y - 1, x), (y + 1, x), (y, x - 1), (y, x + 1))]
                if any(v is None or v in (piel, piel_osc) for v in vecinos):
                    img[y][x] = (140, 136, 140)
    pose = img
    habla = copia(pose)
    for x in range(w):
        if base[32][x] == E:
            habla[32][x] = (60, 20, 20)
            habla[33][x] = (200, 90, 80) if habla[33][x] is not None else None

    def brazos_arriba(src, abre):
        out = copia(src)
        for y in range(37, 44):
            for x in range(w):
                if x < 5 or x > 11:
                    out[y][x] = None
        for y in range(18 + abre, 38):
            for dx in (0, 1):
                out[y][2 - abre + dx] = rojo if y > 22 + abre else blanco
                out[y][13 + abre + dx] = rojo if y > 22 + abre else blanco
        return out

    brazos1, brazos2 = brazos_arriba(pose, 0), brazos_arriba(pose, 1)
    agachado = [[None] * w for _ in range(6)] + [list(f) for f in pose[:44]] + [list(f) for f in pose[50:]]
    agachado = agachado[:h]
    sal = lambda img, nombre, rel: guardar(img, nombre, rel, DESTINO_SR)
    sal(pose, "turbo_pose_01", "identidad_pose/turbo_pose_01")
    sal(habla, "turbo_pose_02", "identidad_pose/turbo_pose_02")
    sal(brazos1, "turbo_brazos_01", "identidad_brazos_arriba/turbo_brazos_01")
    sal(brazos2, "turbo_brazos_02", "identidad_brazos_arriba/turbo_brazos_02")
    for n, (lado, sube) in enumerate([(0, 0), (-1, 2), (0, 0), (1, 2)], 1):
        sal(pierna(pose, 44, lado, sube) if sube else copia(pose), f"turbo_caminar_{n:02d}", f"arcade_caminar/turbo_caminar_{n:02d}")
    for n, img2 in enumerate([brazos1, brazos2, pose, brazos2], 1):
        sal(img2, f"turbo_celebrar_{n:02d}", f"arcade_celebrar/turbo_celebrar_{n:02d}")
    sal(agachado, "turbo_agachado_01", "arcade_agachado/turbo_agachado_01")


if __name__ == "__main__":
    calhoun()
    marines()
    ralph()
    turbo()
    print("disenos simples de Hero's Duty listos en", os.path.normpath(DESTINO))
