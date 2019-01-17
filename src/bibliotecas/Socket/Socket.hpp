//*****************************************************************
// File:   Socket.hpp
// Author: PSCD-Unizar
// Date:   noviembre 2015
// Coms:   Especificación de una librería genérica para
//         comunicación síncrona utilizando sockets
//*****************************************************************

#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include <cstring>
#include <strings.h>
#include <iostream>
#include <unistd.h>

using namespace std;

class Canal;
class Socket;

/**
Representa un servidor, escuchando en un puerto dado un número determinado de
clientes, de forma concurrente.
*/
class Servidor {
 public:
  /**
  Inicializa un nuevo servidor en localhost:{port} para atender a exactamente
  {max_connections} clientes
  */
  Servidor(int port, int max_connections, int maxAttempts = 10, int millisWait = 1000);

  /**
  Finaliza el servidor
  */
  ~Servidor();

  /**
  Devuelve el canal para la comunicación con el siguiente cliente.
  Si no hay ninguno se queda a la espera hasta que haya.
  */
  Canal& getCliente();

 private:
  Socket* socket;
  int socket_fd;
};

/**
Representa un canal de comunicación bidireccional de mensajes string.
*/
class Canal {
  friend Servidor;

 public:
  /**
  Inicializa un nuevo canal de comunicación con un servidor en {address}:{port}
  Si no se consigue se espera {millisWait} millisegundos y se reintenta, un
  máximo de {maxAttempts} reintentos.
  */
  Canal(string address, int port, int maxAttempts = 10, int millisWait = 1000);

  /**
  Finaliza el canal
  */
  ~Canal();

  /**
  Envía un mensaje al canal
  Envío síncrono
  */
  void operator<<(const string message);

  /**
  Recibe un mensaje del canal
  (Recepción síncrona)
  */
  void operator>>(string& message);

 private:
  Canal(Socket* s, int i);
  Socket* socket;
  int socket_fd;
  bool own;
};

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

class Socket {
 private:
  // ATRIBUTOS
  // Para almacenar la dirección y número de puerto
  // donde escucha el proceso servidor
  string SERVER_ADDRESS;
  int SERVER_PORT;

  // Descriptor de fichero asociado al socket;
  int socket_fd;

 public:
  // CONSTRUCTORES
  // Constructor de la clase indicando la dirección
  // y el puerto de escucha del servidor. Para clientes.
  Socket(string address, int port);

  // Constructor de la clase indicando  el puerto de
  // escucha del servidor. Para servidores.
  Socket(int port);

  // MÉTODOS

  // Acepta la conexión de un nuevo cliente.
  // PRE:
  // POST: Abre una nueva conexión con un cliente.
  // Devuelve el descriptor de fichero del socket asociado
  // al nuevo cliente en caso de exito.
  // Devuelve -1 en caso de error.
  int Accept();

  // Binding de la nueva conexión.
  // PRE:
  // POST: Asocia la nueva conexión con el programa actual.
  // Devuelve el descriptor de fichero del socket asociado
  // a la conexión en caso de éxito.
  // Devuelve -1 en caso de error.
  int Bind();

  // Cierre de conexión.
  // PRE: 'fd' indica un descriptor de fichero asociado
  //      a un socket con una conexión abierta.
  // POST: Cierra la conexión asociada al descriptor de
  //       fichero indicado.
  // Devuelve 0 en caso de éxito.
  // Devuelve -1 en caso de error.
  int Close(int fd);

  // Conexión con el servidor.
  // PRE:
  // POST: La función conecta con el servidor utilizando
  //       el socket especificado.
  // En caso de éxito el método devuelve el descriptor
  // de fichero del socket
  // a utilizar para enviar y recibir mensages.
  // Devuelve -1 en caso de error.
  int Connect();

  // Llamada a listen.
  // PRE: 'max_conexiones_espera' > 0 (número de conexiones en
  //      que se mantienen. )
  // POST: El servidor comienza a escuchar en el
  //       puerto indicado.
  // Devuelve 0 en caso de éxito.
  // Devuelve -1 en caso de error.
  int Listen(int max_conexiones_espera);

  // Recepción de un nuevo mensaje a través del
  // socket abierto.
  // PRE: 'fd' indica un descriptor de fichero asociado
  //       a un socket con una conexión abierta.
  //       buffer: Puntero al buffer donde se almacenaran
  //       los datos leídos.
  //       buffer_length indica el tamaño del buffer.
  // POST: Recibe un nuevo mensaje que es almacenado
  //       en la variable "buffer".
  // Devuelve el numero de bytes leídos en caso de éxito.
  // Devuelve -1 en caso de error.
  int Recv(int fd, char* buffer, int buffer_length);
  int Recv(int fd, string& buffer, int buffer_length);

  // Envía información a através del socket abierto
  // PRE: 'fd' indica un descriptor de fichero asociado
  //       a un socket con una conexión abierta.
  //       "message" ha de ser NO VACÍO
  // POST: Envía el mensaje establecido a través del
  //       socket indicado
  // Devuelve el número de bytes enviados en caso de éxito.
  // Devuelve -1 en caso de error.
  ssize_t Send(int fd, const char* message);
  ssize_t Send(int fd, const string message);
};

#endif
