#include"utils.h"
#include <errno.h> 

t_log* logger;

int iniciar_servidor(void)
{
	int socket_servidor, err;
	struct addrinfo hints, *server_info, *p;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	err = getaddrinfo(NULL, PUERTO, &hints, &server_info);
	if (err != 0) {
		fprintf(stderr, "[[ERROR]] {{getaddrinfo}}: {{%s}}\n", gai_strerror(err));
		abort();
	}	
	
	
	socket_servidor = socket(server_info->ai_family, server_info->ai_socktype ,server_info->ai_protocol);
	if (socket_servidor == -1 ){
		fprintf(stderr, "[[ERROR]] {{socket}}: {{%s}}\n", strerror(errno));
		abort();
	}
	
	
	err = bind(socket_servidor, server_info->ai_addr, server_info->ai_addrlen);
	if (err == -1){
		fprintf(stderr, "[[ERROR]] {{bind}}: {{%s}}\n", strerror(errno));
		abort();
	}
	
	err = listen(socket_servidor, SOMAXCONN);
	if (err == -1){
		fprintf(stderr, "[[ERROR]] {{listen}}: {{%s}}\n", strerror(errno));
		abort();
	}
	
	freeaddrinfo(server_info);
	log_trace(logger, "Listo para escuchar a mi cliente");

	return socket_servidor;
}

int esperar_cliente(int socket_servidor)
{
	int socket_cliente;
	socket_cliente = accept(socket_servidor, NULL, NULL);
	if (socket_cliente == -1){
		fprintf(stderr, "[[ERROR]] {{accept}}: {{%s}}\n", strerror(errno));
		abort();
	}
	log_info(logger, "Se conecto un cliente!");
	return socket_cliente;
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}
