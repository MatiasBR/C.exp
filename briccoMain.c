
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INSERT_GASTO                 1
#define MOSTRAR_LISTA                2
#define BORRAR_GASTO                 3
#define MODIFY_GASTO_DETAILS         4
#define DETALLE_GASTOS_MES           5
#define LISTAR_NOMBRE_GASTOS         6
#define LISTAR_GASTO_FIJOS           7
#define LISTAR_GASTO_VAR             8
#define EXIT                         9

#define TIPO_FIJO                   1
#define TIPO_VARIABLE               2
#define TIPO_FIJO_TEXTO           "Fijo"
#define TIPO_VARIABLE_TEXTO       "Variable"

#define GASTO_BD                    "gastos.dat"

#define ISSUE_FOUND                 100
#define ELEMENTOS_GUARDADOS         200
#define NOT_ISSUE_FOUND             201

#define TRUE                        1
#define FALSE                       0

#define INITIAL_ID                  1

typedef struct {
    int dia;                    //Registro para las fechas
    int mes;
    int anio;
} TFecha;

typedef struct {                    //Estructura de los gastos
    int id;
    char descripcion[20];
    char nombre[20];
    int tipo;
    int monto;
    TFecha fechaCreacion;
    _Bool var;
} TGasto;

struct nodo {                   // Define tipo nodo, registro.
    TGasto info;            // Guarda la informacion, de TGASTO
    struct nodo *next;          // Puntero recursivo. Llamado next.
};

typedef struct {            //Estructura para el ID
    int nextID;
}THeadInfo;

typedef struct {
    struct nodo *next;          //Estructura para trabajar con listas
    THeadInfo info;
}THeadNodo;

typedef struct {                // Defino una estructura lista.
    THeadNodo *head;        // Puntero recursivo, llamado head.
} TGastoLista;


//ACCIONES Y FUNCIONES

// 1º Nivel
void getGastoDetails(TGasto *);
void modifyGastoDetails(TGasto *);
void detalleGasto (TGastoLista, int);
void mostrarGastosFijos (TGastoLista, TFecha, int);
void mostrarGastosVar(TGastoLista, TFecha, int);
void listaGastosNombre(TGastoLista, int);

// 2º Nivel
int cargarGastosDB(TGastoLista *, FILE *, const char *);
void inLista(TGastoLista *);
int siListaVacia(TGastoLista);
void enterTipoGasto(int *);
void enterFechaGasto(TFecha *);
void printGastosLista(TGastoLista);
void printGastos(TGasto);
struct nodo * CrearNodo(TGasto);
int getGastoID();
int getUserOption();
int guardarGastos(TGastoLista, FILE *, const char *);
void showHeader();

int main () {
    struct nodo *aux;

    FILE *gastoBD = NULL;

    TGastoLista gastos;

    TFecha fechaActual;

    printf("Ingrese la fecha actual:\n");
    enterFechaGasto(&fechaActual);

    int status = cargarGastosDB(&gastos, gastoBD, GASTO_BD);

    if (ISSUE_FOUND == status) {
        printf("\n No se han podido cargar. El programa no continuara. \n");
        return EXIT_FAILURE;
    }

    int opcionUsuario;

    do {

        opcionUsuario = getUserOption();

        if (opcionUsuario != EXIT) {
            switch(opcionUsuario ) {
                case INSERT_GASTO:
                {
                    TGasto gasto;

                    getGastoDetails(&gasto);

                    gasto.fechaCreacion = fechaActual;

                    if (siListaVacia(gastos) == 1) {
                        gastos.head->next = CrearNodo(gasto);
                        if (gastos.head->next != NULL)  {
                            gastos.head->next->info.id = gastos.head->info.nextID;
                        }
                    } else {
                        aux = gastos.head->next;
                        if (TIPO_FIJO == gasto.tipo) {
                            gasto.id = gastos.head->info.nextID;
                            aux = CrearNodo(gasto);
                            if (aux != NULL) {
                                aux->next = gastos.head->next;
                                gastos.head->next = aux;
                            }
                        }
                        if (TIPO_VARIABLE == gasto.tipo) {
                            struct nodo *prev;
                            while (aux != NULL && aux->info.tipo == TIPO_FIJO) {
                                prev = aux;
                                aux = aux->next;
                            }
                            gasto.id = gastos.head->info.nextID;
                            prev->next = CrearNodo(gasto);

                            if (aux != NULL && prev->next != NULL) {
                                prev->next->next = aux;
                            }
                        }
                    }
                    gastos.head->info.nextID++;
                }
                    break;
                case MOSTRAR_LISTA:
                {
                    if (siListaVacia(gastos) == TRUE) {
                        printf( "\n No hay gastos para mostrar.\n");// Caso de lista vacia
                        break;
                    }

                    printGastosLista(gastos);
                }
                    break;
                case BORRAR_GASTO:
                {
                    if (siListaVacia(gastos) == TRUE) {
                        printf("\n No hay gastos para eliminar.\n"); //Caso de lista vacia
                        break;
                    }

                    printGastosLista(gastos);

                    // Pedimos el id de la gasto a eliminar
                    printf("Ingrese el ID del gasto a eliminar: \n");
                    int gastoIDToBeRemoved = getGastoID();

                    aux = gastos.head->next;

                    int isGastoDeleted = FALSE;
                    // A1-N2-null borrar: a1 ; después: N2-null v
                    if (gastos.head->next->info.id == gastoIDToBeRemoved) {
                        gastos.head->next = aux->next;
                        isGastoDeleted = TRUE;
                    } else {
                        // A1-N2-null borrar: n2; a1-null v
                        // A1-N3-N2-null  borrar: n3;  a1-n2-null v
                        // A1-null  borrar: n2; a1-null
                        // A1-null  borrar: a1; null
                        struct nodo *prev = aux;
                        while (aux != NULL && !isGastoDeleted) {
                            if (aux->info.id == gastoIDToBeRemoved) {
                                prev->next = aux->next;
                                isGastoDeleted = TRUE;
                            } else {
                                prev = aux;
                                aux = aux->next;
                            }
                        }
                    }

                    // Si no se ha eliminado un gasto, avisamos al usuario
                    if (FALSE == isGastoDeleted) {
                        printf("\n No existe un gasto con ese ID.\n");
                    } else {
                        printf("\n Se ha eliminado el gasto indicado.\n");
                    }

                    if (siListaVacia(gastos) == TRUE) {
                        printf("\n No hay gastos en la lista.\n");
                        break;
                    }

                    printf("\n Esta es la lista actual de gastos.\n");

                    // Mostrar la lista de gastos después de esta operación.
                    showHeader();
                    aux = gastos.head->next;
                    while (aux != NULL) {
                        TGasto auxGasto = aux->info;
                        aux = aux->next;
                        printGastos(auxGasto);
                    }
                }
                    break;
                case MODIFY_GASTO_DETAILS:
                    if (siListaVacia(gastos) == TRUE) {
                        printf("\n No hay gastos para modificar.\n");
                        break;
                    }

                    printGastosLista(gastos);

                    // Pedimos el id del gasto que vamos a modificar
                    printf("Ingrese el ID del gasto a modificar:\n");
                    int gastoIDToBeRemoved = getGastoID();

                    // Procedemos a buscar el gasto en la lista
                    aux = gastos.head->next;
                    int encontrado = FALSE;
                    while (aux != NULL && !encontrado) {
                        if (aux->info.id == gastoIDToBeRemoved) {
                            modifyGastoDetails(&aux->info);
                            encontrado = TRUE;
                        } else {
                            aux = aux->next;
                        }
                    }

                    break;
                case DETALLE_GASTOS_MES:
                    if (siListaVacia(gastos) == TRUE) {
                        printf("\n No hay gastos para ver de este mes. \n");
                        break;
                    }
                    printGastosLista(gastos);

                    int mesAux;
                    detalleGasto(gastos,mesAux );
                    break;
                case LISTAR_NOMBRE_GASTOS:
                    if (siListaVacia(gastos) == TRUE) {
                        printf("\n No hay gastos en la lista. \n");
                        break;
                    }

                    int montoFinal = 0;
                    listaGastosNombre(gastos, montoFinal);
                    break;
                case LISTAR_GASTO_FIJOS:
                    if (siListaVacia(gastos) == TRUE) {
                        printf("\n No hay gastos en la lista. \n");
                        break;
                    }

                    int totalFijos = 0;
                    TFecha feaux;

                    mostrarGastosFijos(gastos, feaux,totalFijos);

                    break;
                case LISTAR_GASTO_VAR:
                    if (siListaVacia(gastos) == TRUE) {
                        printf("\n No hay gastos en la lista. \n");
                        break;
                    }
                    int totalVar = 0;
                    mostrarGastosVar(gastos,feaux,totalVar);

                    break;
                default:
                    break;
            }
        }
    } while(opcionUsuario != EXIT);

    if(siListaVacia(gastos) == FALSE) {
        status = guardarGastos(gastos, gastoBD, GASTO_BD);
        if (ISSUE_FOUND == status) {
            printf("\n Los gastos no se han podido guardar. Error \n");
        }
        if (ELEMENTOS_GUARDADOS == status) {
            printf("\n Los gastos se guardaron exitosamente.\n");
        }
    }

    printf("\n Gracias por usar este programa.\n");
    return EXIT_SUCCESS;
}



int getGastoID() {
    int gastoIDToBeRemoved;

    scanf("%d", &gastoIDToBeRemoved);           //Toma el id y lo guarda, el que debe ser borrado/ modificado etc

    return gastoIDToBeRemoved;
}



void inLista(TGastoLista *gastos)  {
    gastos->head = (THeadNodo *) calloc(1, sizeof(THeadNodo));  //Inicializa la lista
    gastos->head->next = NULL;
    gastos->head->info.nextID = INITIAL_ID;
}


int siListaVacia(TGastoLista gastos) {                          //Funcion preegunta si la lista esta vacia
    return gastos.head->next == NULL;
}

struct nodo * CrearNodo(TGasto gasto){                                          //Funcion para crear nodo vacio
    struct nodo *nuevo = (struct nodo *) malloc (sizeof(struct nodo));

    if (nuevo != NULL){
        nuevo -> info = gasto;
        nuevo -> next = NULL;
    }

    return nuevo;
}


void getGastoDetails(TGasto *nuevoGasto) {

    while ((getchar()) != '\n');                                        // Limpia el buffer del teclado

    printf("Ingrese la descripcion del gasto:\n");              //Ingresa c/u de los datos para saber cuales son los gastos
    fgets(nuevoGasto->descripcion, 20, stdin);

    printf("Ingrese el nombre del gasto:\n");
    fgets(nuevoGasto->nombre, 20, stdin);

    printf("Ingrese el monto del gasto a pagar:\n");
    scanf("%d", &nuevoGasto->monto);

    enterTipoGasto(&nuevoGasto->tipo);

}

void showHeader() {
    printf("----------------------------------------------------------------------------------------------\n"); //Elemento visual
}

void enterTipoGasto(int *tipoGasto){
    do {
        printf("Ingrese el tipo:\n");                       //Ingresar el tipo con un numero y ser mostrado con el nombre (fijo o variable)
        printf("%d - Tipo Fijo \n", TIPO_FIJO);
        printf("%d - Tipo Variable\n", TIPO_VARIABLE);
        printf("Elija una tipo:\n");
        scanf ("%d", tipoGasto);
    } while(*tipoGasto < TIPO_FIJO || *tipoGasto > TIPO_VARIABLE);
}


void enterFechaGasto(TFecha *fecha){
    printf("Ingresa el dia \n");
    scanf("%d",&fecha->dia);
    printf("Ingresa el mes \n");        //Accion autocontenida para ingresar fecha
    scanf("%d",&fecha->mes);
    printf("Ingresa el año \n");
    scanf("%d",&fecha->anio);
}

int getUserOption() {                                           //Menu para que el usuario seleccione lo que quiera hacer
    int userOption;
    do {
        printf("%d. Insertar gasto.\n", INSERT_GASTO);
        printf("%d. Mostrar la lista de gastos.\n", MOSTRAR_LISTA);
        printf("%d. Eliminar un gasto de la lista por su ID.\n",BORRAR_GASTO );
        printf("%d. Modificar un gasto.\n", MODIFY_GASTO_DETAILS);
        printf("%d. Mostrar los detalles de un gasto de un mes.\n", DETALLE_GASTOS_MES);
        printf("%d. Mostrar los gastos por nombre.\n", LISTAR_NOMBRE_GASTOS);
        printf("%d. Mostrar gastos fijos.\n", LISTAR_GASTO_FIJOS);
        printf("%d. Mostrar gastos variables.\n", LISTAR_GASTO_VAR);

        printf("%d. Salir del programa.\n", EXIT);
        printf("Elija una opción (%d-%d):\n", INSERT_GASTO, EXIT);
        scanf("%d", &userOption);
    } while(userOption < INSERT_GASTO || userOption > EXIT);

    return userOption;
}

void printGastosLista(TGastoLista gastos) {                     //Muestreo de la lista
    printf("Estas son los gastos actualmente \n");
    showHeader();
    struct nodo *aux = gastos.head->next;
    while (aux != NULL) {
        TGasto gasto = aux->info;
        aux = aux->next;
        printGastos(gasto);
    }
}

void printGastos(TGasto gasto) {                        //Muestra c/u de los datos de la lista
    printf("ID: %d \n\n",        gasto.id);
    printf("Descripcion: %s", gasto.descripcion);
    printf("Nombre: %s\n", gasto.nombre);
    printf("Fecha: %d/ %d/ %d \n",   gasto.fechaCreacion.dia, gasto.fechaCreacion.mes, gasto.fechaCreacion.anio);
    printf("Monto: %d \n", gasto.monto);
    printf("Tipo: %s\n \n",  gasto.tipo == TIPO_FIJO ? TIPO_FIJO_TEXTO : TIPO_VARIABLE_TEXTO );
    puts("------------------------------------------------------------------------------------------------------\n");
}


void modifyGastoDetails(TGasto *nuevoGasto) {

    while ((getchar()) != '\n'); // Limpia el buffer del teclado

    printf("Ingrese la descripcion del gasto: \n");
    fgets(nuevoGasto->descripcion, 20, stdin);

    fflush(stdin);

    printf("Ingrese el nombre del gasto:\n");
    fgets(nuevoGasto->nombre, 20, stdin);

    printf("Ingrese el nuevo monto:\n");
    scanf("d%", &nuevoGasto->monto);

    fflush(stdin);
    enterTipoGasto(&nuevoGasto->tipo);

}

void detalleGasto(TGastoLista gastos, int mes) {
    showHeader();

    struct nodo *aux = gastos.head->next;
    // Pedimos el id del gasto que vamos a modificar
    printf("Ingrese el ID del gasto a detallar:\n");
    int gastoIDToBeRemoved = getGastoID();

    printf("Ingrese el numero del mes que quiere ver al detalle:");
    scanf("%d", &mes);

    // Procedemos a buscar el gasto en la lista
    aux = gastos.head->next;
    int encontrado = FALSE;
    while (aux != NULL && !encontrado) {
        if (aux->info.id == gastoIDToBeRemoved) {
            if (mes == aux->info.fechaCreacion.mes) {                   //Compara el mes ingresado con el perteneciente al ID
                printf("Total acumulado en el mes %d, es de: %d \n ", mes,  aux->info.monto);
            }else{
                printf("Los meses no coinciden, no existe monto con ese ID y Mes.\n");
            }
            encontrado = TRUE;
        } else {
            aux = aux->next;
        }
    }

}

void listaGastosNombre(TGastoLista gastos, int montoFinal) {

    printf("Estas son los gastos actualmente por nombre: \n");

    struct nodo *aux = gastos.head->next;
    struct nodo *aux2 = gastos.head->next;

    while (aux!=NULL){                               //Recorre la lista mediante dos punteros auxiliares.
        aux->info.var = TRUE;
        aux= aux->next;
    }
    aux = aux2;

    while (aux != NULL ) {
        if (aux->info.var == TRUE){
            while (aux2 != NULL) {                                           //Creo una variable logica (Var) para verificar si ya habia pasado por ese nodo de la lista
                if (strcmp(aux->info.nombre,aux2->info.nombre) == 0){
                    montoFinal = montoFinal + aux2->info.monto;
                    aux2->info.var = FALSE;
                }
                aux2 = aux2->next;
            }
            showHeader();
            printf("Nombre del gasto: %s\n", aux->info.nombre);
            printf("Monto total: %d\n", montoFinal);
        }
        montoFinal = 0;
        aux = aux->next;
        aux2 = aux;
    }
}


void mostrarGastosFijos(TGastoLista gastos, TFecha ayu, int totalFijos) {
    showHeader();
    int numGastos = 0;
    struct nodo *aux = gastos.head->next;

    printf("Ingrese un mes determinado, para ver sus gastos fijos:\n");
    scanf("%d", &ayu.mes);
    while(aux != NULL) {
        if (aux->info.fechaCreacion.mes == ayu.mes && aux->info.tipo == 1){
            numGastos++;
            totalFijos = totalFijos + aux->info.monto;
        }
                                                            //Muestra los gastos fijos de un mes determinado
        aux = aux->next;
    }
    if(numGastos==0) {
        printf("\n No hay gastos que pertenezcan a ese mes. \n");
    }else{
        printf("Estos son los gastos fijos actuales: \n ");
        aux=gastos.head->next;
        while (aux != NULL){
            if (aux->info.tipo == 1){
                printGastos(aux->info);
            }

            aux = aux-> next;
        }

        printf("El monto total de gastos fijos es de %d \n\n", totalFijos);

    }
}

void mostrarGastosVar(TGastoLista gastos, TFecha ayu, int totalVar) {
    showHeader();
    int numGastos = 0;
    struct nodo *aux = gastos.head->next;
    printf("Ingrese un mes determinado, para ver sus gastos variables:\n"); //Muestra los gastos variables de un mes determinado
    scanf("%d", &ayu.mes);
    while(aux != NULL) {
        if (aux->info.fechaCreacion.mes == ayu.mes && aux->info.tipo != 1) {
            numGastos++;
            totalVar = totalVar + aux->info.monto;
        }

        aux = aux->next;
    }
    if(numGastos==0) {
        printf("\n No hay gastos que pertenezcan a ese mes. \n");
    }else{
        printf("Estos son los gastos fijos actuales: \n ");
        aux=gastos.head->next;
        while (aux != NULL){
            if (aux->info.tipo != 1){
                printGastos(aux->info);
            }

            aux = aux-> next;
        }
        printf("El monto total de gastos variables es de %d \n\n", totalVar);
    }
}




//Carga de datos que ya han sido guardados en el archivo

int cargarGastosDB(TGastoLista *gastos, FILE *gastoBD, const char *fileDB) {

    gastoBD = fopen(fileDB,"a+b");

    if (gastoBD == NULL) {
        return ISSUE_FOUND;
    }

    inLista(gastos);

    while (!feof(gastoBD)) {
        TGasto *gasto = (TGasto *) calloc(1, sizeof(TGasto));
        int numGastoReads;
        numGastoReads = fread(gasto, sizeof(TGasto), 1, gastoBD);
        if (numGastoReads != 0) {
            struct nodo *auxNode = CrearNodo(*gasto);
            if (auxNode != NULL) {
                if (gastos->head->next == NULL) {
                    gastos->head->next = auxNode;
                } else {
                    struct nodo *aux = gastos->head->next;
                    // head->a2->a1->n1->null
                    while(aux != NULL && aux->next != NULL) {
                        aux = aux->next;
                    }
                    aux->next = auxNode;
                }
            }
        }

    }

    if(gastos->head->next != NULL) {

        struct nodo *aux = gastos->head->next;
        int maxID = 0;
        while (aux != NULL) {
            if (aux->info.id > maxID) {
                maxID = aux->info.id;
            }
            aux = aux->next;
        }
        gastos->head->info.nextID = maxID + 1;
    }

    int issueFound = ferror(gastoBD);

    fclose(gastoBD);

    if (issueFound) {
        return ISSUE_FOUND;
    } else {
        return NOT_ISSUE_FOUND;
    }

}

int guardarGastos(TGastoLista gastos, FILE * gastoBD, const char * fileDB) { //Guardado de datos

    gastoBD = fopen(fileDB,"wb");

    if (gastoBD == NULL) {
        return ISSUE_FOUND;
    }

    struct nodo *aux = gastos.head->next;
    while (aux != NULL) {
        fwrite(&aux->info, sizeof(aux->info) ,1 ,gastoBD);
        aux = aux->next;
    }

    int issueFound = ferror(gastoBD);

    fclose(gastoBD);

    if (issueFound) {
        return ISSUE_FOUND;
    } else {
        return ELEMENTOS_GUARDADOS;
    }

}



