# Redibuja los sprites de Hero's Duty y Sugar Rush con el estilo limpio de Fix-It Felix Jr.:
#   1) mas brillo/contraste/saturacion en los sprites oscuros (Calhoun, marines...)
#   2) pocos colores planos por sprite (k-means)
#   3) sin pixeles sueltos (filtro de mayoria)
#   4) contorno oscuro del color de cada zona (como los personajes del arcade)
# Entrada: Recursos/Sprites/<ruta>.txt   Salida: Recursos/SpritesMejorados/<ruta>.txt (mismo formato)
# Uso: python mejorar_sprites.py            (todos)
#      python mejorar_sprites.py --vista X  (guarda una comparacion PNG de algunos sprites en X)
import os
import sys
import colorsys
import numpy as np

AQUI = os.path.dirname(os.path.abspath(__file__))
ORIGEN = os.path.join(AQUI, "..", "Recursos", "Sprites")
DESTINO = os.path.join(AQUI, "..", "Recursos", "SpritesMejorados")
CARPETAS = ["03_Sugar_Rush", "04_Heros_Duty"]
SUELTOS = []
# Calhoun, los marines y Ralph con armadura tienen disenos simples propios (disenar_heros.py)
OMITIR = (os.path.join("04_Heros_Duty", "Calhoun"), os.path.join("04_Heros_Duty", "Marines"), os.path.join("04_Heros_Duty", "Ralph_armadura"), os.path.join("03_Sugar_Rush", "Turbo"))
# escenarios y efectos: solo color (sin contorno ni reduccion fuerte)
SIN_CONTORNO = ("Construccion", "Efectos", "Escenario", "UI", "HUD", "Props_mundo", "Armas_y_powerups")
SIMBOLOS = ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
            "!#$%&*+-/:;<=>?@[]^_{|}~")


def leer(ruta):
    lineas = open(ruta, encoding="utf-8").read().splitlines()
    nombre = lineas[0].split(" ", 1)[1]
    w, h = map(int, lineas[1].split()[1:3])
    n = int(lineas[2].split()[1])
    pal = {}
    for l in lineas[3:3 + n]:
        pal[l[0]] = (int(l[2:4], 16), int(l[4:6], 16), int(l[6:8], 16))
    rgb = np.zeros((h, w, 3), np.float32)
    mask = np.zeros((h, w), bool)
    for y, f in enumerate(lineas[4 + n:4 + n + h]):
        for x, c in enumerate(f[:w]):
            if c != ".":
                rgb[y, x] = pal.get(c, (255, 0, 255))
                mask[y, x] = True
    return nombre, rgb, mask


def guardar(nombre, rgb, mask, ruta):
    h, w = mask.shape
    paleta, filas = {}, []
    for y in range(h):
        fila = []
        for x in range(w):
            if not mask[y, x]:
                fila.append(".")
                continue
            c = tuple(int(v) for v in rgb[y, x])
            if c not in paleta:
                paleta[c] = SIMBOLOS[min(len(paleta), len(SIMBOLOS) - 1)]
            fila.append(paleta[c])
        filas.append("".join(fila))
    os.makedirs(os.path.dirname(ruta), exist_ok=True)
    with open(ruta, "w", encoding="utf-8", newline="\n") as f:
        f.write(f"SPRITE {nombre}\nSIZE {w} {h}\nCOLORS {len(paleta)}\n")
        for (r, g, b), s in paleta.items():
            f.write(f"{s} {r:02X}{g:02X}{b:02X}\n")
        f.write("PIXELS\n" + "\n".join(filas) + "\nEND\n")


def ajustar_color(rgb, mask):
    px = rgb[mask] / 255.0
    if len(px) == 0:
        return rgb
    lum = (0.299 * px[:, 0] + 0.587 * px[:, 1] + 0.114 * px[:, 2]).mean()
    gamma = 0.62 if lum < 0.22 else (0.78 if lum < 0.35 else 0.92)   # aclara los sprites oscuros
    sat = 1.35 if lum < 0.35 else 1.18
    out = rgb.copy()
    for y, x in zip(*np.nonzero(mask)):
        r, g, b = (rgb[y, x] / 255.0) ** gamma
        hh, ll, ss = colorsys.rgb_to_hls(r, g, b)
        ss = min(1.0, ss * sat)
        ll = min(1.0, (ll - 0.5) * 1.15 + 0.5)                       # un poco mas de contraste
        ll = max(0.0, ll)
        r, g, b = colorsys.hls_to_rgb(hh, ll, ss)
        out[y, x] = (r * 255, g * 255, b * 255)
    return out


def kmeans(colores, k, vueltas=12):
    rng = np.random.default_rng(7)
    k = min(k, len(np.unique(colores, axis=0)))
    # centros iniciales: los colores mas distintos (por luminancia) para no perder detalles chicos
    lum = colores @ np.array([0.299, 0.587, 0.114])
    orden = np.argsort(lum)
    centros = colores[orden[np.linspace(0, len(orden) - 1, k).astype(int)]].copy()
    for _ in range(vueltas):
        d = ((colores[:, None, :] - centros[None, :, :]) ** 2).sum(-1)
        etiqueta = d.argmin(1)
        for i in range(k):
            sel = colores[etiqueta == i]
            if len(sel):
                centros[i] = sel.mean(0)
            else:
                centros[i] = colores[rng.integers(len(colores))]
    d = ((colores[:, None, :] - centros[None, :, :]) ** 2).sum(-1)
    return centros, d.argmin(1)


def limpiar(rgb, mask, con_contorno):
    h, w = mask.shape
    n = int(mask.sum())
    if n == 0:
        return rgb
    k = 8 if n < 500 else (10 if n < 2000 else 12)
    if not con_contorno:
        k += 8
    pos = np.nonzero(mask)
    centros, etq = kmeans(rgb[pos], k)
    lab = -np.ones((h, w), int)
    lab[pos] = etq
    # filtro de mayoria: un pixel distinto a casi todos sus vecinos toma el color de la mayoria
    for _ in range(3):
        nuevo = lab.copy()
        for y, x in zip(*pos):
            vec = [lab[yy, xx] for yy in (y - 1, y, y + 1) for xx in (x - 1, x, x + 1)
                   if 0 <= yy < h and 0 <= xx < w and (yy, xx) != (y, x) and lab[yy, xx] >= 0]
            if len(vec) < 5:
                continue
            valores, cuentas = np.unique(vec, return_counts=True)
            mejor = valores[cuentas.argmax()]
            if (lab[y, x] not in vec and cuentas.max() >= 5) or (vec.count(lab[y, x]) <= 1 and cuentas.max() >= 6):
                nuevo[y, x] = mejor
        lab = nuevo
    out = np.zeros_like(rgb)
    out[pos] = centros[lab[pos]]
    if con_contorno and n >= 900:
        # contornos internos (estilo arcade): entre dos zonas muy distintas se oscurece la mas oscura
        lumc = centros @ np.array([0.299, 0.587, 0.114])
        oscurecer = np.zeros((h, w), bool)
        for y, x in zip(*pos):
            for dy, dx in ((0, 1), (1, 0)):
                yy, xx = y + dy, x + dx
                if yy >= h or xx >= w or lab[yy, xx] < 0:
                    continue
                a, b = lab[y, x], lab[yy, xx]
                if a == b or np.abs(centros[a] - centros[b]).sum() < 150:
                    continue
                if lumc[a] < lumc[b]:
                    oscurecer[y, x] = True
                else:
                    oscurecer[yy, xx] = True
        out[oscurecer] = out[oscurecer] * 0.55
    if con_contorno:
        # contorno: los pixeles del borde de la silueta se oscurecen con su propio color
        for y, x in zip(*pos):
            borde = any(not (0 <= y + dy < h and 0 <= x + dx < w) or not mask[y + dy, x + dx]
                        for dy, dx in ((-1, 0), (1, 0), (0, -1), (0, 1)))
            if borde:
                c = out[y, x]
                out[y, x] = c * 0.28 + np.array([8, 6, 12])
    return np.clip(out, 0, 255)


def scale2x(rgb, mask):
    # Scale2x (EPX) sobre pixel art: duplica el tamano suavizando las diagonales
    h, w = mask.shape
    clave = lambda y, x: (tuple(rgb[y, x]) if mask[y, x] else None)
    out = np.zeros((h * 2, w * 2, 3), np.float32)
    om = np.zeros((h * 2, w * 2), bool)
    for y in range(h):
        for x in range(w):
            p = (y, x)
            A = (y - 1, x) if y > 0 else p
            B = (y, x + 1) if x < w - 1 else p
            C = (y, x - 1) if x > 0 else p
            D = (y + 1, x) if y < h - 1 else p
            k = {q: clave(*q) for q in (p, A, B, C, D)}
            e = [p, p, p, p]
            if k[C] == k[A] and k[C] != k[D] and k[A] != k[B]: e[0] = A
            if k[A] == k[B] and k[A] != k[C] and k[B] != k[D]: e[1] = B
            if k[D] == k[C] and k[D] != k[B] and k[C] != k[A]: e[2] = C
            if k[B] == k[D] and k[B] != k[A] and k[D] != k[C]: e[3] = D
            for i, q in enumerate(e):
                oy, ox = 2 * y + i // 2, 2 * x + i % 2
                om[oy, ox] = mask[q]
                out[oy, ox] = rgb[q]
    return out, om


# Vanellope es muy chica para reconocerla: sus cuadros de caminar/parada se agrandan al doble
AGRANDAR = (os.path.join("02_Vanellope_y_personajes", "Vanellope"),)


def procesar(rel):
    nombre, rgb, mask = leer(os.path.join(ORIGEN, rel))
    if any(rel.startswith(p) for p in AGRANDAR):
        rgb, mask = scale2x(rgb, mask)
    con_contorno = not any(p in rel for p in SIN_CONTORNO)
    rgb = ajustar_color(rgb, mask) if con_contorno else rgb
    rgb = limpiar(rgb, mask, con_contorno)
    guardar(nombre, rgb, mask, os.path.join(DESTINO, rel))
    return rgb, mask


def lista():
    rels = []
    for carpeta in CARPETAS:
        for raiz, _, archivos in os.walk(os.path.join(ORIGEN, carpeta)):
            rels += [os.path.relpath(os.path.join(raiz, a), ORIGEN) for a in archivos if a.endswith(".txt")]
    rels = [r for r in rels if not r.startswith(OMITIR)]
    return rels + SUELTOS


def vista(salida):
    from PIL import Image
    muestras = ["04_Heros_Duty/Calhoun/caminar/calhoun_caminar_01", "04_Heros_Duty/Marines/fila_1/marine_fila_1_01",
                "04_Heros_Duty/Ralph_armadura/fila_1/ralph_armadura_f1_01", "04_Heros_Duty/Cy-Bugs/reina_grande_2/cybug_reina_grande_2_01",
                "02_Vanellope_y_personajes/Vanellope/fila_1/vanellope_f1_01", "02_Vanellope_y_personajes/Personajes_escala/vanellope",
                "03_Sugar_Rush/Corredores/Vanellope/kart_atras/vanellope_kart_atras_01", "03_Sugar_Rush/Corredores/Vanellope/kart_lado/vanellope_kart_lado_01",
                "03_Sugar_Rush/King_Candy/pose/king_candy_pose_01", "03_Sugar_Rush/Turbo/identidad_pose/turbo_pose_01"]
    piezas = []
    for m in muestras:
        rel = m.replace("/", os.sep) + ".txt"
        _, rgb0, mask = leer(os.path.join(ORIGEN, rel))
        rgb1, mask1 = procesar(rel)
        for rgb, mask in ((rgb0, mask), (rgb1, mask1)):
            im = Image.new("RGB", (mask.shape[1], mask.shape[0]), (255, 0, 255))
            for y, x in zip(*np.nonzero(mask)):
                im.putpixel((int(x), int(y)), tuple(int(v) for v in rgb[y, x]))
            piezas.append(im)
    W = sum(p.width + 3 for p in piezas)
    H = max(p.height for p in piezas)
    hoja = Image.new("RGB", (W, H), (40, 40, 40))
    x = 0
    for p in piezas:
        hoja.paste(p, (x, H - p.height))
        x += p.width + 3
    hoja.resize((W * 3, H * 3), Image.NEAREST).save(salida)


if __name__ == "__main__":
    if len(sys.argv) > 2 and sys.argv[1] == "--vista":
        vista(sys.argv[2])
    else:
        rels = lista()
        for r in rels:
            procesar(r)
        print(len(rels), "sprites mejorados en", os.path.normpath(DESTINO))
        import disenar_heros
        disenar_heros.calhoun(); disenar_heros.marines(); disenar_heros.ralph(); disenar_heros.turbo()
        print("disenos simples de Calhoun, marines y Ralph con armadura listos")
