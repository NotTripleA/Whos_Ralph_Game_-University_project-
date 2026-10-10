# Whos_Ralph_Game_-University_project-
"Who's Ralph" is a game developed in C++ without the use of graphics libraries—a restriction imposed by university policy for this project. It aims to tell a story from three perspectives, featuring four levels, three endings, plot twists, decision-making, and reliance on the player's own skill.
The following information will be presented in Spanish, the project's native language:
# COMO JUGAR
# ----------
- Doble clic en WhosRalph.exe (o en bin\Release\WhosRalph.exe si se compilo con Visual Studio).
- El juego se abre en la consola clasica de Windows con fuente cuadrada 8x8 y ventana maximizada:
  asi cada pixel de los sprites es exactamente un caracter █ con su color real.
  (Windows Terminal no deja cambiar la fuente, por eso el juego se relanza solo en conhost).
- Controles: WASD o flechas para moverse, X o ENTER para aceptar/accion, ESC para pausa/volver.
- En el modo historia ESC abre la PAUSA: CONTINUAR, GUARDAR y SALIR, con la reputacion,
  el medidor de empatia y los niveles completados. La partida tambien se guarda sola al
  empezar cada cinematica o minijuego (Guardado\partida.txt) y "Jugar historia" permite continuar.
- F3 muestra/oculta los cuadros por segundo.
- Para probar o grabar el video: F9 completa la etapa actual del Fix-It; en Hero's Duty vacia el
  nido (otra vez F9: sube en el ascensor) o vence al Cy-Bug gigante; en Sugar Rush suma una vuelta;
  contra Turbo lo deja al 10% (y completa la secuencia final). Se puede abrir el juego directo en
  una escena:
  WhosRalph.exe --escena PROLOGO | NIVEL1_FIXIT | CINE_REUNION | CINE_BADANON | CINE_REGRESO |
  NIVEL1_FIN | CINE_CENTRAL | NIVEL2_HEROS | CINE_FINAL2 | NIVEL2_FIN | CINE_SUGAR_LLEGADA |
  CINE_SUGAR_CARRO | CINE_SUGAR_INICIO | NIVEL3_CARRERA | CINE_TURBO | NIVEL_SECRETO | CINE_FINAL |
  MENU | MINI_FIX_IT | MINI_HEROS_DUTY | MINI_SUGAR_RUSH
- Se guardan: punto de control, reputacion, empatia, niveles completados, decisiones, escuadron
  salvado, puntuacion, decisiones positivas/negativas (Guardado\partida.txt) y los finales
  descubiertos (Guardado\finales.txt, no se borran al empezar una partida nueva).

# HISTORIA (modelo: Inicio -> Menu -> Nivel 1 -> Animacion -> Nivel 2 -> Nivel 3 -> Eleccion de final)
# --------
  Prologo (narrador de sistema)
  Nivel 1: Fix-It Felix Jr. (controlas a Felix)  -> intro, 3 etapas de 2 pisos (Ralph arriba de los pisos),
           final en el techo y Ralph cayendo por el frente del edificio hasta el lodo
           (en la historia Ralph rompe pocas ventanas; en el minijuego es dificil)
  Cinematica: Ralph va a la reunion de villanos (voces del video)
  Cinematica: Bad-Anon -> eleccion ACEPTACION (+1 rep, +1 emp) o REBELION (+0 rep, -1 emp)
  Cinematica: Ralph regresa -> eleccion de Felix (+1/+1 o +0/-1)  "... recordara esto"
  Resumen del Nivel 1
  Cinematica: la Central del Juego (Surge Protector, aviso de Sonic) y el portal de Hero's Duty
  Nivel 2: Hero's Duty (controlas a la Sargento Calhoun), vista desde arriba como Pokemon Rojo/Fuego:
           se camina casilla por casilla con el escuadron detras y se dispara hacia donde se mira.
           3 pisos con un nido de 10 Cy-Bugs cada uno -> ascensor (se ve la torre desde afuera)
           -> cima con la medalla, Ralph asustado y el Cy-Bug gigante
           eleccion: salvar al escuadron (+0 rep, -1 emp) -> final "Hacer lo correcto"
                     salvar a Ralph (+1 rep, +1 emp)      -> final "Un nuevo amigo"
  Resumen del Nivel 2
  Cinematica: Ralph llega a Sugar Rush (en nave si salvo al escuadron, a escondidas si salvo a Ralph),
              el Cy-Bug se esconde, conoce a Vanellope y se cuentan sus historias (segun lo elegido).
              Empatia >= 2: Ralph le regala la medalla (+1 rep); si no, Vanellope se la roba de noche.
  Cinematica: el carro de Vanellope y la visita del rey -> eleccion: romper el carro (-1 rep, +0 emp)
              o abandonarla (-1 rep, -1 emp)  "VANELLOPE RECORDARA ESTO..."
  Cinematica: [SISTEMA] presenta Sugar Rush y a Vanellope (glitch) y Ralph trae un carro nuevo
  Nivel 3: la carrera de Sugar Rush (controlas a Vanellope): 3 vueltas contra 5 corredoras,
           obstaculos (chocolate, donas, bloqueos, zonas glitch), GLITCH con ESPACIO, HUD con
           velocidad, integridad, reputacion, empatia, niveles, progreso, glitch, posicion y mapa.
           Evento: obstaculo gigante -> Ralph lo demuele (+1 rep) o el glitch (puede fallar)
  NIVEL SECRETO - EL REINICIO DEL CODIGO: el rey se revela como Turbo y corrompe el arcade.
           Batalla por turnos (estilo Final Fantasy) con esquives estilo Undertale.
           Aliados segun la reputacion: Vanellope (>=1), Felix (>=2), Calhoun (>=3, con su escuadron
           si fue salvado) y los villanos de Bad-Anon (>=4). Con reputacion <= 0 Ralph pelea solo.
           Al 10% de vida de Turbo: lanzarse solo al nucleo (secuencia de teclas) o llamar al equipo.
  Finales segun la empatia: 2 a 3 "Arcade Ralph" (epilogo igual al video "Escena Final", solo con la voz de Ralph,
           sin musica ni sonidos del juego:
           maquina del arcade, Bad-Anon, el lodo y el pastel, los niveles superiores, el arcade popular,
           la casa nueva, la boda de Felix y Calhoun, el tejado y la vista de Sugar Rush),
           -1 a 1 "Esto no es lo que esperaba, pero...", -4 a -2 "Siempre sere el villano".
           Al vencer a Turbo sale YOU WIN. El final recuerda si Ralph fue solo al nucleo o con su equipo,
           muestra la medalla vista desde las tres miradas (Felix, Calhoun y Vanellope) y hace dos preguntas
           reflexivas: con quien te identificaste y si cambio tu lectura de Ralph (y por que).
           Recuadro con el final desbloqueado (x/3) y las estadisticas, pregunta reflexiva final
           (Guardado\respuestas.txt) y "¿Deseas comenzar una nueva partida?".

# COMO COMPILAR
# -------------
- Visual Studio: abrir WhosRalph.sln y compilar (Debug o Release, x64).
- Sin abrir Visual Studio: ejecutar compilar.bat (genera WhosRalph.exe en esta carpeta).

# CARPETAS
# --------
  src\Motor\           consola, lienzo (con camara), sprites, animaciones, fuentes, teclado, audio
  src\Entidades\       personajes del menu (Entidad -> PersonajeAnimado -> ...) y desfile del menu
  src\Escenas\         pantallas del menu (Escena -> PreMenu, MenuPrincipal, Minijuegos, ...)
  src\Historia\        modo historia: progreso/guardado, pausa, dialogos, cinematicas
  src\Niveles\         niveles jugables (FixIt\, HerosDuty\, SugarRush\ carrera, Turbo\ batalla por turnos)
  Recursos\Sprites     los sprites de Sprites\FrameXFrame convertidos a ASCII (paleta + grilla)
  Música\              musica (ver Música\LEEME.txt)
  Dialogos de voz\     voces recortadas de los videos guia
  Sonidos\             efectos arcade generados
  Guardado\            partida y record (se crea al jugar)
  herramientas\        convertir_sprites.py, generar_titulo.py, generar_sonidos.py, extraer_voces.py

Si se cambian o agregan PNG en Sprites\FrameXFrame, volver a ejecutar:
    python herramientas\convertir_sprites.py
    python herramientas\mejorar_sprites.py   (sprites de Hero's Duty y Sugar Rush redibujados)

SPRITES MEJORADOS (Hero's Duty y Sugar Rush)
  Recursos\SpritesMejorados tiene esos sprites redibujados con el estilo limpio de Fix-It Felix Jr.:
  mas brillo y contraste en los oscuros, pocos colores planos, sin pixeles sueltos, contorno oscuro
  y contornos internos entre zonas. Calhoun, los marines, Ralph con armadura y Turbo usan disenos simples hechos
  a partir de los personajes a escala (herramientas\disenar_heros.py), y Vanellope es siempre la del menu.
  El juego los usa en lugar de los originales; si falta alguno, usa el original. Se siguen dibujando con █.

# POO APLICADA
# ------------
- Abstraccion: clases base abstractas Escena, Entidad, EventoDesfile y Paso (metodos virtuales puros).
- Encapsulamiento: atributos privados con metodos de acceso (Sprite, Lienzo, Progreso, EdificioFixIt...).
- Herencia: Entidad -> PersonajeAnimado -> FelixJugador / RalphFixIt / Actor / CalhounJugador / ...;
            PersonajeAnimado -> CyBugHD -> CyBugRastreroHD / CyBugExploradorHD / CyBugVoladorHD;
            ObstaculoSR -> BarreraChocolate / DonaSR / BloqueoSR / ZonaGlitchSR / GiganteSR;
            MiembroEquipo -> RalphCombate / VanellopeCombate / FelixCombate / CalhounCombate / VillanosCombate
            (cada miembro redefine su especial y su forma de esquivar);
            Escena -> EscenaHistoria -> EscenaCinematica -> EscenaBadAnon / EscenaCentral / ...;
            Paso -> PasoDialogo / PasoEleccion / PasoEspera / PasoAccion.
- Polimorfismo: el juego, el fondo del menu y las cinematicas llaman actualizar()/dibujar() o
                Paso::actualizar() sin saber que objeto concreto es; cada Cy-Bug redefine
                moverse() (busca al escuadron por los pasillos, toma desvios o vuela sobre las paredes).
