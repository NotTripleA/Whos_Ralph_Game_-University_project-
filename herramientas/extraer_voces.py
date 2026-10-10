# Recorta los dialogos de voz de los videos guia (carpeta Indicaciones) y los guarda en
# "Dialogos de voz" como .wav. Los tiempos se ubicaron transcribiendo los videos con faster-whisper.
# Requiere: pip install imageio-ffmpeg
import os
import subprocess
import imageio_ffmpeg

AQUI = os.path.dirname(os.path.abspath(__file__))
TB1 = os.path.normpath(os.path.join(AQUI, "..", ".."))
VIDEOS = os.path.join(TB1, "Indicaciones")
SALIDA = os.path.join(AQUI, "..", "Dialogos de voz")
FF = imageio_ffmpeg.get_ffmpeg_exe()

# (archivo de salida, video, [(inicio, fin), ...])  -> varios tramos se unen con una pausa corta
CLIPS = [
    # Nivel 1: arcade Fix-It Felix Jr.
    ("ralph_tronco.wav", "Fix-It.mp4", [(18.30, 20.70)]),            # ¡Hey! ¡Movieron mi tronco!
    ("ralph_demolerlo.wav", "Fix-It.mp4", [(38.90, 41.30)]),         # ¡Voy a demolerlo!
    ("nicelanders_reparalo.wav", "Fix-It.mp4", [(51.30, 53.70)]),    # ¡Repáralo, Félix!
    ("felix_puedo_repararlo.wav", "Fix-It.mp4", [(55.40, 56.80)]),   # ¡Puedo repararlo!
    ("ralph_grito.wav", "Fix-It.mp4", [(169.00, 172.20)]),           # grito al caer del techo
    ("nicelanders_risa.wav", "Fix-It.mp4", [(176.50, 178.70)]),      # risas
    # Cinematica: Ralph va a la reunion (video Junta de Villanos)
    ("felix_a_descansar.wav", "Junta de Villanos.mp4", [(2.05, 3.20), (23.40, 24.30)]),
    ("ralph_monologo_1.wav", "Junta de Villanos.mp4", [(16.80, 20.45)]),
    ("ralph_monologo_2.wav", "Junta de Villanos.mp4", [(21.75, 23.40)]),
    ("ralph_monologo_3.wav", "Junta de Villanos.mp4", [(24.45, 30.00)]),
    ("ralph_monologo_4.wav", "Junta de Villanos.mp4", [(32.75, 37.90)]),
    # Junta de villanos: el lema de Bad-Anon
    ("lema_badanon.wav", "Junta de Villanos.mp4", [(231.90, 241.00)]),
    # Nivel 2: Ralph pasa por la Central del Juego (control de la Surge Protector)
    ("surge_nombre.wav", "Central del juego.mp4", [(56.00, 56.70)]),            # "Nombre."
    ("ralph_nombre.wav", "Central del juego.mp4", [(56.75, 57.65)]),            # "Ralph el Demoledor."
    ("surge_frutas.wav", "Central del juego.mp4", [(60.55, 61.85)]),            # "¿Transporta frutas con usted?"
    ("ralph_frutas.wav", "Central del juego.mp4", [(61.95, 63.65)]),            # "No, no, ni una cereza."
    ("surge_dirige.wav", "Central del juego.mp4", [(63.75, 64.90)]),            # "¿Y a dónde se dirige?"
    ("ralph_dirige.wav", "Central del juego.mp4", [(64.90, 66.20)]),            # "A Repara Félix Jr."
    ("surge_declarar.wav", "Central del juego.mp4", [(66.30, 67.45)]),          # "¿Algo que declarar?"
    ("ralph_odio.wav", "Central del juego.mp4", [(67.50, 68.15)]),              # "Que te odio."
    ("surge_proceda.wav", "Central del juego.mp4", [(68.20, 69.95)]),           # "Eso dicen todos. Proceda."
    ("sonic_aviso.wav", "Central del juego.mp4", [(75.50, 89.60)]),             # aviso de Sonic: "Game Over"
    # Nivel 2: Hero's Duty (video Hero's Duty reference)
    ("hd_narrador.wav", "Hero's Duty reference.mp4", [(0.00, 8.40)]),           # "En un planeta sin nombre..."
    ("calhoun_briefing.wav", "Hero's Duty reference.mp4", [(17.00, 30.00)]),    # "Atencion, escuchen bien..."
    ("escuadron_viva.wav", "Hero's Duty reference.mp4", [(30.00, 31.40)]),      # "Viva mama, viva!"
    ("calhoun_cuidado.wav", "Hero's Duty reference.mp4", [(77.30, 80.50)]),     # "Cuidado, novato..."
    ("calhoun_laboratorio.wav", "Hero's Duty reference.mp4", [(100.55, 107.30)]),  # "Bien, ninas... al laboratorio"
    # Nivel 3: Turbo (video Turbo es relevado) y el epilogo del final "Arcade Ralph" (video Escena Final)
    ("turbo_turbotastico.wav", "Turbo es relevado.mp4", [(93.70, 95.40)]),        # "Turbotastico!"
    ("ralph_epilogo.wav", "Ralph el Demoledor. Escena Final (Español latino).mp4", [(3.00, 122.50)]),   # narracion final de Ralph
    ("ralph_salvame.wav", "Hero's Duty reference.mp4", [(115.10, 120.40)]),     # "Desde cuando los videojuegos... Salvame!"
    ("calhoun_mision.wav", "Hero's Duty reference.mp4", [(44.00, 50.30)]),      # "Somos la ultima esperanza..."
    ("calhoun_huevos.wav", "Hero's Duty reference.mp4", [(88.50, 91.40)]),      # "Destruyan los huevos..."
]


def tramo(video, ini, fin, destino):
    dur = fin - ini
    filtro = f"afade=t=in:d=0.03,afade=t=out:st={max(0.0, dur - 0.05):.2f}:d=0.05"
    subprocess.run([FF, "-y", "-loglevel", "error", "-ss", f"{ini:.2f}", "-t", f"{dur:.2f}", "-i", video,
                    "-vn", "-af", filtro, "-ac", "2", "-ar", "44100", "-c:a", "pcm_s16le", destino], check=True)


def main():
    os.makedirs(SALIDA, exist_ok=True)
    import sys
    forzar = "--forzar" in sys.argv
    for nombre, video, tramos in CLIPS:
        ruta_video = os.path.join(VIDEOS, video)
        destino = os.path.join(SALIDA, nombre)
        if os.path.exists(destino) and not forzar:
            continue   # ya existe (usar --forzar para volver a recortar)
        if len(tramos) == 1:
            tramo(ruta_video, tramos[0][0], tramos[0][1], destino)
        else:
            partes = []
            for i, (a, b) in enumerate(tramos):
                p = destino + f".parte{i}.wav"
                tramo(ruta_video, a, b, p)
                partes.append(p)
            entradas = []
            for p in partes:
                entradas += ["-i", p]
            n = len(partes)
            silencio = "".join(f"[{i}:a]apad=pad_dur=0.25[a{i}];" for i in range(n))
            unir = "".join(f"[a{i}]" for i in range(n)) + f"concat=n={n}:v=0:a=1[s]"
            subprocess.run([FF, "-y", "-loglevel", "error"] + entradas + ["-filter_complex", silencio + unir, "-map", "[s]",
                                                                         "-c:a", "pcm_s16le", destino], check=True)
            for p in partes:
                os.remove(p)
        print("ok", nombre)


if __name__ == "__main__":
    main()
