#include "client.h"

int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int conexion;
	char* ip;
	char* puerto;
	char* clave;

	t_log* logger;
	t_config* config;

	/* ---------------- LOGGING ---------------- */

	logger = iniciar_logger();
	
	
	if (!logger){
		fprintf(stderr, "[[ERROR]] {{logger}: {{no se puedo inicializar}}\n");
		abort();
	}

	log_info(logger, "Hola! Soy un log");

	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();
	
	
	if (!config){\
		fprintf(stderr, "[[ERROR]] {{config}: {{no se puedo inicializar}}\n");
		abort();
	}
	
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");
	clave = config_get_string_value(config, "CLAVE");

	
	log_info(logger, "%s:%s:%s", ip, puerto, clave);
	
	
	/* ---------------- LEER DE CONSOLA ---------------- */

	leer_consola(logger);

	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/
	conexion = crear_conexion(ip, puerto);
	
	
	enviar_mensaje(clave, conexion);
	paquete(conexion);


	terminar_programa(conexion, logger, config);

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger;
	nuevo_logger = log_create("tp0.log", "tp0", true, LOG_LEVEL_INFO);
	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	t_config* nuevo_config;
	nuevo_config = config_create("cliente.config");
	return nuevo_config;
}
void leer_consola(t_log* logger)
{
	char* leido;
	leido = readline("> ");
	while(strcmp(leido, "\0") != 0){
		log_info(logger, "%s", leido);
		free(leido);
		leido = readline("> ");
	}

	free(leido);
}

void paquete(int conexion)
{
	char* leido;
	t_paquete *paquete;

	paquete = crear_paquete();


	leido = readline("> ");
	while(strcmp(leido, "\0") != 0){
		agregar_a_paquete(paquete, leido, strlen(leido) + 1);
		free(leido);
		leido = readline("> ");
	}
	
	if (paquete->buffer->size > 0) {
		enviar_paquete(paquete, conexion);
	}	

	eliminar_paquete(paquete);
	free(leido);
	
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	log_destroy(logger);
	config_destroy(config);
	liberar_conexion(conexion);
}
