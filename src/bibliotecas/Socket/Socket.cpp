//*****************************************************************
// File:   Socket.cpp
// Author: PSCD-Unizar
// Date:   noviembre 2015
// Coms:   Implementación de una librería genérica para
//         comunicación síncrona utilizando sockets
//         La especificación se encuentra en "Socket.hpp"
//         Como realiza "#include <Socket.hpp>", es necesario que
//         se indique al compilador que debe buscar ficheros
//         para incluir también en el directorio donde se encuentre
//         "Socket.hpp", mediante
//              -IpathDondeSeEncuentaSocket.hpp
//*****************************************************************

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <Socket.hpp>

#include <thread>
#include <chrono>

using namespace std;

const int MESSAGE_SIZE = 4001;  // mensajes de no más 4000 caracteres

//-------------------------------------------------------------
Servidor::Servidor(int port, int max_connections) {
  // Creación del socket con el que se llevará a cabo
  // la comunicación con el servidor.
  socket = new Socket(port);

  // Bind
  socket_fd = socket->Bind();
  if (socket_fd == -1) {
    string mensError(strerror(errno));
    cerr << "Error en el bind: " + mensError + "\n";
    exit(1);
  }

  // Listen
  int error_code = socket->Listen(max_connections);
  if (error_code == -1) {
    string mensError(strerror(errno));
    cerr << "Error en el listen: " + mensError + "\n";
    exit(1);
  }
}
//-------------------------------------------------------------
Canal& Servidor::getCliente() {
  // Accept
  int client_fd = socket->Accept();

  if (client_fd == -1) {
    string mensError(strerror(errno));
    cerr << "Mensaje de error: " + mensError + "\n";
    exit(1);
  }

  return *(new Canal(socket, client_fd));
}
//-------------------------------------------------------------
Servidor::~Servidor() {
  // Cerramos el socket del servidor
  int error_code = socket->Close(socket_fd);
  if (error_code == -1) {
    string mensError(strerror(errno));
    cerr << "Error cerrando el socket del servidor: " + mensError + "\n";
  }
}
//-------------------------------------------------------------

//-------------------------------------------------------------
Canal::Canal(Socket* s, int i) {
  socket = s;
  socket_fd = i;
  own = false;
}
//-------------------------------------------------------------
Canal::Canal(string address, int port, int maxAttempts, int millisWait) {
  socket = new Socket(address, port);
  socket_fd = -1;
  own = true;

  // Conectamos con el servidor. Probamos varias conexiones
  int count = 0;
  do {
    // Conexión con el servidor

    if (count != 0) {
      cerr << "Reintento " << count + 1 << "/" << maxAttempts << " ";
    }

    socket_fd = socket->Connect();
    count++;

    // Si error --> esperamos para reconectar
    if (socket_fd == -1) {
      this_thread::sleep_for(chrono::milliseconds(millisWait));
    }
  } while (socket_fd == -1 && count < maxAttempts);

  // Chequeamos si se ha realizado la conexión
  if (socket_fd == -1) {
    cerr << "No se ha podido conectar con el servidor "+address + ":" + to_string(port) + " : " << strerror(errno)
         << endl;
    exit(1);
  }
}
//-------------------------------------------------------------
Canal::~Canal() {

  // Cerramos el socket
  int error_code = socket->Close(socket_fd);
  if (error_code == -1) {
    cerr << "Error cerrando el socket: " << strerror(errno) << endl;
  }

  if (!own) {
    // No es nuestro socket (es de un servidor) así que no hay que borrar
    return;
  }

  delete (socket);
}
//-------------------------------------------------------------
void Canal::operator<<(const string message) {
  // enviar mensaje
  int send_bytes = socket->Send(socket_fd, message);

  if (send_bytes <= 0) {
    if (send_bytes == -1)
      cerr << "Error al enviar datos: " << strerror(errno) << endl;
    throw "SEND_END";  // exit(1);
  }
}
//-------------------------------------------------------------
void Canal::operator>>(string& message) {
  // Recibimos el mensaje
  int read_bytes = socket->Recv(socket_fd, message, MESSAGE_SIZE);

  if (read_bytes <= 0) {
    if (read_bytes == -1)
      cerr << "Mensaje de error: " << strerror(errno) << endl;
    throw "RECV_END";  // exit(1);
  }
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////


//-------------------------------------------------------------
const int ACK_BUFFER_SIZE = 4;  // para hacer la comunición síncrona
//-------------------------------------------------------------
// Constructor de la clase indicando la dirección
// y el puerto de escucha del servidor. Para clientes.
Socket::Socket(string address, int port) {

  // Guardamos datos del servidor
  SERVER_ADDRESS = address;
  SERVER_PORT = port;
}
//-------------------------------------------------------------
// Constructor de la clase indicando solo el puerto de
// escucha del servidor. Para servidores.
Socket::Socket(int port) {

  // Guardamos datos del servidor
  SERVER_ADDRESS = "localhost";
  SERVER_PORT = port;
}
//-------------------------------------------------------------
int Socket::Accept() {

  struct sockaddr_in client;  // Información dirección scliente
  socklen_t sin_size = sizeof(struct sockaddr_in);
  int fd = accept(socket_fd, (struct sockaddr *)&client, &sin_size);

  if (fd == -1) {
    cerr << "Error en accept\n";
  }
  return fd;
}
//-------------------------------------------------------------.
int Socket::Bind() {

  // Creación del socket y almacenamiento del descriptor del socket (Servidor)
  // AF_INET     --> IPv4
  // SOCK_STREAM --> Comunicación TCP
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  // Información de la dirección del servidor
  struct sockaddr_in server;

  server.sin_family = AF_INET;  // IPv4
  server.sin_port = htons(SERVER_PORT);
  server.sin_addr.s_addr =
      INADDR_ANY;  // INADDR_ANY coloca nuestra dirección IP automáticamente
  bzero(&(server.sin_zero), 8);  // 0 en el resto de la estructura

  // Llamada a bind
  int exito =
      ::bind(socket_fd, (struct sockaddr *)&server, sizeof(struct sockaddr_in));

  if (exito == -1) {
    return -1;
  } else {
    return socket_fd;
  }
}
//-------------------------------------------------------------
int Socket::Close(int fd) {

  int exito = close(fd);

  if (exito != 0) {
    return -1;
  }

  return exito;
}
//-------------------------------------------------------------
int Socket::Connect() {

  // Creación del socket y almacenamiento del descriptor del socket (Cliente)
  // AF_INET     --> IPv4
  // SOCK_STREAM --> Comunicación TCP
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  struct hostent *he;         // Información del nodo remoto
  struct sockaddr_in server;  // Información dirección servidor

  // Obtenemos la dirección del servidor
  he = gethostbyname(SERVER_ADDRESS.c_str());
  if (he == NULL) {
    cerr << "Error obteniendo la dirección del servidor\n";
    return -1;
  }

  // Guardamos la información del servidor
  server.sin_family = AF_INET;                        // IPv4
  server.sin_port = htons(SERVER_PORT);               // Codificación por red
  server.sin_addr = *((struct in_addr *)he->h_addr);  // Info del servidor
  bzero(&(server.sin_zero), 8);                       // Rellenamos con ceros

  // Establecemos la conexion con el servidor
  int exito =
      connect(socket_fd, (struct sockaddr *)&server, sizeof(struct sockaddr));
  if (exito == -1) {
    cerr << "Error conectando con el servidor " << SERVER_ADDRESS << ":"
         << SERVER_PORT << endl;
    return -1;
  } else {
    return socket_fd;
  }
}
//-------------------------------------------------------------
int Socket::Listen(int max_conexions_espera) {

  // Llamamos a listen
  int exito = listen(socket_fd, max_conexions_espera);

  return exito;
}
//-------------------------------------------------------------
int Socket::Recv(int fd, char *buffer, int buffer_length) {

  // PRIMERO: RECIBIMOS INFORMACION
  // Limpiamos el buffer
  bzero(buffer, buffer_length);

  // Leemos todos los datos posibles que quepan en el buffer
  int num_bytes = recv(fd, buffer, buffer_length, 0);

  if (num_bytes == -1) {
    cerr << "Error al recibir datos del socket\n";
  }

  // SEGUNDO: ENVIAMOS ACK

  char ack[ACK_BUFFER_SIZE];

  bzero(ack, ACK_BUFFER_SIZE);
  strcpy(ack, "ACK");

  int sent_bytes = send(fd, ack, strlen(ack), 0);

  // Devolvemos número de bytes leídos
  return num_bytes;
}
//-------------------------------------------------------------
int Socket::Recv(int fd, string &buffer, int buffer_length) {
  buffer = "";  // vaciar
  // PRIMERO: RECIBIMOS INFORMACION
  // Limpiamos el buffer
  char bufferAux[buffer_length];
  bzero(bufferAux, buffer_length);

  // Leemos todos los datos posibles que quepan en el buffer
  int num_bytes = Recv(fd, bufferAux, buffer_length);

  if (num_bytes != -1) {  // ha ido bien
    buffer.append(bufferAux);
  }

  // Devolvemos número de bytes leídos
  return num_bytes;
}
//-------------------------------------------------------------
ssize_t Socket::Send(int fd, const char *message) {

  // PRIMERO ENVIAMOS INFORMACION
  ssize_t num_bytes = send(fd, message, strlen(message), 0);

  // SEGUNDO: RECIBIMOS ACK
  // Creamos buffer para ACK
  char ack_buffer[ACK_BUFFER_SIZE];
  bzero(ack_buffer, ACK_BUFFER_SIZE);

  // Leemos todos los datos posibles que quepan en el buffer
  // Será "ACK"
  int rcv_bytes = recv(fd, ack_buffer, ACK_BUFFER_SIZE - 1, 0);

  // Comprobamos que no haya error enviando el ACK
  if (0 != strcmp(ack_buffer, "ACK")) {
    num_bytes = -1;
  }

  return num_bytes;
}
//-------------------------------------------------------------
ssize_t Socket::Send(int fd, const string message) {

  ssize_t num_bytes = Send(fd, message.c_str());

  return num_bytes;
}
