#ifndef _SOCKET_SERVIDOR_H
#define _SOCKET_SERVIDOR_H

int abreSocket(char *ip, int puerto, int nroConexiones);
int aceptaConexionCliente(int descriptor);

#endif
