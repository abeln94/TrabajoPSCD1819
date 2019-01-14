Contenido:
  src:
    Contiene todo el código del programa organizado por carpetas.
  Makefile:
    Permite compilar el código. El resultado se coloca en 'buildX/' siendo X el resultado del comando 'uname' (permite compilar en varias máquinas). 
    Importante: En hendrix es necesario utilizar el comando 'gmake' en lugar de 'make'.
  run:
    Utilidad que permite ejecutar cada uno de los ejecutables sin necesidad de ir manualmente a la carpeta correspondiente (tiene en cuenta 'uname').
  lanzar.sh:
    Utilidad para iniciar el servidor y los subservidores en local en los puertos especificados.
  readme.txt:
    Este documento.