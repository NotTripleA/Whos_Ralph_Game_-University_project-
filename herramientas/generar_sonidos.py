# Genera efectos de sonido estilo arcade antiguo (onda cuadrada 8-bit) para el menu.
# Salida: Sonidos/*.wav
import math
import os
import random
import struct
import wave

AQUI = os.path.dirname(os.path.abspath(__file__))
SALIDA = os.path.join(AQUI, "..", "Sonidos")
FREC = 22050


def nota(hz, ms, vol=0.35, duty=0.25, caida=True):
    n = int(FREC * ms / 1000)
    datos = []
    for i in range(n):
        fase = (i * hz / FREC) % 1.0
        v = vol if fase < duty else -vol
        if caida:
            v *= 1.0 - (i / n) * 0.6
        datos.append(v)
    return datos


def silencio(ms):
    return [0.0] * int(FREC * ms / 1000)


def guardar(nombre, muestras):
    os.makedirs(SALIDA, exist_ok=True)
    with wave.open(os.path.join(SALIDA, nombre), "wb") as w:
        w.setnchannels(1)
        w.setsampwidth(1)
        w.setframerate(FREC)
        w.writeframes(bytes(int(128 + max(-1.0, min(1.0, s)) * 127) for s in muestras))
    print(nombre, len(muestras), "muestras")


if __name__ == "__main__":
    # Cambio de opcion: "blip" corto de dos tonos
    guardar("menu_mover.wav", nota(988, 28) + nota(1319, 38))
    # Aceptar: arpegio ascendente tipo moneda
    guardar("menu_aceptar.wav", nota(1047, 45) + nota(1319, 45) + nota(1568, 45) + nota(2093, 110))
    # Atras / cancelar: dos notas descendentes
    guardar("menu_atras.wav", nota(784, 60) + nota(523, 90))
    # Pulsar Enter en la pantalla de inicio: "start" arcade
    guardar("inicio.wav", nota(523, 60) + nota(659, 60) + nota(784, 60) + nota(1047, 60) + silencio(20)
            + nota(784, 50) + nota(1047, 220, duty=0.5))
    # Error / opcion no disponible
    guardar("menu_error.wav", nota(196, 80, duty=0.5) + silencio(20) + nota(185, 140, duty=0.5))

    # ---- Modo historia ----
    # letra de los dialogos (estilo Undertale, muy corto y suave)
    guardar("texto.wav", nota(740, 18, vol=0.18, duty=0.5))
    # "<personaje> recordara esto..."
    guardar("recordara.wav", nota(659, 70, vol=0.25) + nota(988, 70, vol=0.25) + nota(1319, 180, vol=0.25, duty=0.5))
    # puerta que se cierra
    guardar("puerta.wav", nota(110, 40, duty=0.5) + nota(82, 120, duty=0.5))

    # ---- Fix-It Felix Jr. ----
    guardar("martillo.wav", nota(1568, 25, vol=0.3) + nota(1175, 30, vol=0.25))           # tink del martillo
    guardar("ventana_reparada.wav", nota(1047, 40) + nota(1319, 40) + nota(1568, 40) + nota(2093, 90))
    guardar("golpe_ralph.wav", nota(98, 50, duty=0.5) + nota(73, 90, duty=0.5))           # golpe pesado
    guardar("salto.wav", [s for k in range(8) for s in nota(400 + k * 90, 12, vol=0.25)])
    guardar("herido.wav", [s for k in range(10) for s in nota(900 - k * 70, 22, vol=0.3, duty=0.5)])
    guardar("etapa.wav", nota(523, 80) + nota(659, 80) + nota(784, 80) + nota(1047, 80) + nota(784, 80) + nota(1047, 260, duty=0.5))
    guardar("pastel.wav", nota(784, 50) + nota(988, 50) + nota(1175, 50) + nota(1568, 50) + nota(1976, 120))
    guardar("gameover.wav", nota(392, 160, duty=0.5) + nota(349, 160, duty=0.5) + nota(330, 160, duty=0.5) + nota(262, 420, duty=0.5))
    guardar("destello.wav", nota(2093, 30, vol=0.2) + nota(2637, 30, vol=0.2) + nota(3136, 60, vol=0.2))

    # ---- Hero's Duty ----
    # laser: barrido descendente rapido
    guardar("laser.wav", [s for k in range(9) for s in nota(2400 - k * 210, 9, vol=0.22, duty=0.5)])
    # explosion: ruido que se apaga
    n = int(FREC * 0.32)
    guardar("explosion.wav", [random.uniform(-0.5, 0.5) * (1.0 - i / n) ** 2 * (1 if (i // 40) % 2 else 0.6) for i in range(n)])
    # disparo de rifle: estallido de ruido muy corto + golpe grave que se apaga rapido
    n = int(FREC * 0.26)
    disparo = []
    for i in range(n):
        t = i / FREC
        ruido = random.uniform(-1, 1) * math.exp(-t * 38)
        golpe = math.sin(2 * math.pi * (140 - 260 * t) * t) * math.exp(-t * 16) * 0.8
        cola = random.uniform(-1, 1) * math.exp(-t * 9) * 0.18
        disparo.append(max(-1.0, min(1.0, (ruido + golpe + cola) * 0.9)))
    guardar("disparo.wav", disparo)
    # recarga del rifle: dos "clacks" metalicos
    guardar("recarga.wav", nota(330, 30, vol=0.3) + silencio(60) + nota(1760, 18, vol=0.25) + nota(880, 30, vol=0.25))
