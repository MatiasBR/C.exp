//Universidad Nacional de Rio Cuarto
//Carrera: Analista en computación
//Materia: Algoritmica y Programación
 
// Proyecto para alumnos libres

// Alumno: Bricco Matias - DNI 44203047

//Año 2021

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INSERT_TASK                 1
#define SHOW_TASK_LIST              2
#define REMOVE_TASK_BY_ID           3
#define MODIFY_TASK_DETAILS         4
#define SHOW_URGENT_TASKS           5
#define SHOW_TASKS_BY_EMPLOYEE      6
#define EXIT                        7

#define HIGH_PRIORITY               1
#define NORMAL_PRIORITY             2
#define HIGH_PRIORITY_TEXT          "Alta"
#define NORMAL_PRIORITY_TEXT        "Normal"

#define TASK_BD                     "tareas.dat"

#define ISSUE_FOUND                 100
#define STORED_TASKS_SUCCESSFULLY   200
#define NOT_ISSUE_FOUND             201

#define TRUE                        1
#define FALSE                       0

#define INITIAL_ID                  1

typedef struct {
    int dia;
    int mes;
    int anio;
} TFecha;

typedef struct {
    int id;
    char descripcion[20];
    char encargado[20];
    int prioridad;
    TFecha fechaCreacion;
    TFecha fechaFinalizacion;
} TTask;

struct nodo {					// Define tipo nodo, registro.
    TTask info; 			// Guarda la informacion, de tipoTarea
    struct nodo *next;			// Puntero recursivo. Llamado next.
};

typedef struct {
    int nextID;
}THeadInfo;

typedef struct {
    struct nodo *next;
    THeadInfo info;
}THeadNodo;

typedef struct {				// Defino una estructura lista.
    THeadNodo *head;		// Puntero recursivo, llamado head.
} TTaskList;

// 1º nivel
void getTaskDetails(TTask *);
void modifyTaskDetails(TTask *);
void showUrgentTasks(TTaskList, TFecha);
void showTasksByEmployee(TTaskList, const char *);

// 2º nivel
int loadTasksFromDB(TTaskList *, FILE *, const char *);
void initTaskList(TTaskList *);
int isTaskListEmpty(TTaskList);
void enterTaskPriority(int *);
void enterTaskDate(TFecha *);
void printTaskListDetails(TTaskList);
void printTaskDetails(TTask);
struct nodo * CrearNodo(TTask);
int getTaskID();
int getUserOption();
int storeTasks(TTaskList, FILE *, const char *);
void showHeader();

int main () {
    struct nodo *aux;

    FILE *taskBD = NULL;

    TTaskList tasks;

    TFecha fechaActual;

    printf("Ingrese la fecha actual:\n");
    enterTaskDate(&fechaActual);

    int status = loadTasksFromDB(&tasks, taskBD, TASK_BD);

    if (ISSUE_FOUND == status) {
        printf("\n No se han podido cargar. El programa no continuara. \n");
        return EXIT_FAILURE;
    }

    int userOption;

    do {

        userOption = getUserOption();

        if (userOption != EXIT) {
            switch(userOption) {
                case INSERT_TASK:
                    {
                        TTask task;

                        getTaskDetails(&task);

                        task.fechaCreacion = fechaActual;

                        if (isTaskListEmpty(tasks) == 1) {
                            tasks.head->next = CrearNodo(task);
                            if (tasks.head->next != NULL)  {
                                tasks.head->next->info.id = tasks.head->info.nextID;
                            }
                        } else {
                            aux = tasks.head->next;
                            if (HIGH_PRIORITY == task.prioridad) {
                                task.id = tasks.head->info.nextID;
                                aux = CrearNodo(task);
                                if (aux != NULL) {
                                    aux->next = tasks.head->next;
                                    tasks.head->next = aux;
                                }
                            }
                            if (NORMAL_PRIORITY == task.prioridad) {
                                struct nodo *prev;
                                while (aux != NULL && aux->info.prioridad == HIGH_PRIORITY) {
                                    prev = aux;
                                    aux = aux->next;
                                }
                                task.id = tasks.head->info.nextID;
                                prev->next = CrearNodo(task);
            
                                if (aux != NULL && prev->next != NULL) {
                                    prev->next->next = aux;
                                }
                            }
                        }
                        tasks.head->info.nextID++;
                    }
                    break;
                case SHOW_TASK_LIST:
                    {
                        if (isTaskListEmpty(tasks) == TRUE) {
                            printf( "\n No existen tareas para mostrar.\n");
                            break;
                        }

                        printTaskListDetails(tasks);
                    }
                    break;
                case REMOVE_TASK_BY_ID:
                    {
                        if (isTaskListEmpty(tasks) == TRUE) {
                            printf("\n No hay tareas para eliminar.\n");
                            break;
                        }

                        printTaskListDetails(tasks);

                        // Pedimos el id de la tarea a eliminar
                        printf("Ingrese el ID de la tarea a eliminar: \n");
                        int taskIDToBeRemoved = getTaskID();

                        aux = tasks.head->next;

                        int isTaskDeleted = FALSE;
                        // A1-N2-null borrar: a1 ; después: N2-null v
                        if (tasks.head->next->info.id == taskIDToBeRemoved) {
                            tasks.head->next = aux->next;
                            isTaskDeleted = TRUE; 
                        } else {
                            // A1-N2-null borrar: n2; a1-null v
                            // A1-N3-N2-null  borrar: n3;  a1-n2-null v
                            // A1-null  borrar: n2; a1-null
                            // A1-null  borrar: a1; null
                            struct nodo *prev = aux;
                            while (aux != NULL && !isTaskDeleted) {
                                if (aux->info.id == taskIDToBeRemoved) {
                                    prev->next = aux->next;
                                    isTaskDeleted = TRUE;
                                } else {
                                    prev = aux;
                                    aux = aux->next;
                                }
                            }
                        }

                        // Si no se ha eliminado una tarea, avisamos al usuario
                        if (FALSE == isTaskDeleted) {
                            printf("\n No existe una tarea con ese ID.\n");
                        } else {
                            printf("\n Se ha eliminado la tarea indicada.\n");
                        }

                        if (isTaskListEmpty(tasks) == TRUE) {
                            printf("\n No hay tareas en la lista.\n");
                            break;
                        }

                        printf("\n Esta es la lista actual de tareas.\n");

                        // Mostrar la lista de tareas después de esta operación.
                        showHeader();
                        aux = tasks.head->next;
                        while (aux != NULL) {
                            TTask task2 = aux->info;
                            aux = aux->next;
                            printTaskDetails(task2);
                        }
                    }
                    break;
                case MODIFY_TASK_DETAILS:
                    {
                        if (isTaskListEmpty(tasks) == TRUE) {
                            printf("\n No hay tareas para modificar.\n");
                            break;
                        }

                        printTaskListDetails(tasks);

                        // Pedimos el id de la tarea a modificar
                        printf("Ingrese el ID de la tarea a modificar:\n");
                        int taskIDToBeRemoved = getTaskID();

                        // Procedemos a buscar la tarea en la lista
                        aux = tasks.head->next;
                        int encontrado = FALSE;
                        while (aux != NULL && !encontrado) {
                            if (aux->info.id == taskIDToBeRemoved) {
                                modifyTaskDetails(&aux->info);
                                encontrado = TRUE;
                            } else {
                                aux = aux->next;
                            }
                        }
                    }
                case SHOW_URGENT_TASKS:
                    if (isTaskListEmpty(tasks) == TRUE) {
                        printf("\n No hay tareas urgentes para este mes. \n");
                        break;
                    }
                    showUrgentTasks(tasks, fechaActual);
                    break;
                case SHOW_TASKS_BY_EMPLOYEE:
                    if (isTaskListEmpty(tasks) == TRUE) {
                        printf("\n No hay tareas en la lista. \n");
                        break;
                    }

                    char encargado[20];
                    while ((getchar()) != '\n');
                    printf("Ingrese el nombre del encargado de la tarea: \n");
                    fgets(encargado, 20, stdin);

                    showTasksByEmployee(tasks, encargado);
                    break;
                default:
                    break;
            }
        }
    } while(userOption != EXIT);

    if(isTaskListEmpty(tasks) == FALSE) {
        status = storeTasks(tasks, taskBD, TASK_BD);
        if (ISSUE_FOUND == status) {
            printf("\n Las tareas no se guardaron. Error \n");
        }
        if (STORED_TASKS_SUCCESSFULLY == status) {
            printf("\n Las tareas se guardaron exitosamente.\n");
        }
    }

    printf("\n Gracias por usar este programa.\n");
    return EXIT_SUCCESS;
}


void showUrgentTasks(TTaskList tasks, TFecha fechaLimite) {
    showHeader();
    int numTasks = 0;
    struct nodo *aux = tasks.head->next;
    while(aux != NULL) {
        if (aux->info.fechaFinalizacion.mes == fechaLimite.mes && aux->info.fechaFinalizacion.anio == fechaLimite.anio) {
            numTasks++;
            printTaskDetails(aux->info);
        }
       if (aux->info.prioridad == 1){
           numTasks++;
           printTaskDetails(aux->info);
       }
        aux = aux->next;
    }
    if(numTasks==0) {
        printf("\n No hay tareas urgentes que finalizan hoy. \n");
    }
}

void showTasksByEmployee(TTaskList tasks, const char *encargado) {
    showHeader();
    int numTasks = 0;
    struct nodo *aux = tasks.head->next;
    while(aux != NULL) {
        if (strcmp(aux->info.encargado, encargado) == 0) {
            numTasks++;
            printTaskDetails(aux->info);
        }
        aux = aux->next;
    }
    if(numTasks==0) {
        printf("No hay tareas de este empleado.\n");
    }

}

int loadTasksFromDB(TTaskList *tasks, FILE *taskBD, const char *fileDB) {

    taskBD = fopen(fileDB,"a+b");

    if (taskBD == NULL) {
        return ISSUE_FOUND;
    }

    initTaskList(tasks);

    while (!feof(taskBD)) {
        TTask *task = (TTask *) calloc(1, sizeof(TTask));
        int numTaskReads;
        numTaskReads = fread(task, sizeof(TTask), 1, taskBD);
        if (numTaskReads != 0) {
            struct nodo *taskNode = CrearNodo(*task);
            if (taskNode != NULL) {
                if (tasks->head->next == NULL) {
                    tasks->head->next = taskNode;
                } else {
                    struct nodo *aux = tasks->head->next;
                    // head->a2->a1->n1->null
                    while(aux != NULL && aux->next != NULL) {
                        aux = aux->next;
                    }
                    aux->next = taskNode;
                }
            }
        }

    }

    if(tasks->head->next != NULL) {

        struct nodo *aux = tasks->head->next;
        int maxID = 0;
        while (aux != NULL) {
            if (aux->info.id > maxID) {
                maxID = aux->info.id;
            }
            aux = aux->next;
        }
        tasks->head->info.nextID = maxID + 1;
    }

    int issueFound = ferror(taskBD);

    fclose(taskBD);

    if (issueFound) {
        return ISSUE_FOUND;
    } else {
        return NOT_ISSUE_FOUND;
    }

}

void initTaskList(TTaskList *tasks)  {
    tasks->head = (THeadNodo *) calloc(1, sizeof(THeadNodo));
    tasks->head->next = NULL;
    tasks->head->info.nextID = INITIAL_ID;
}

int getUserOption() {
    int userOption;

    do {
        printf("%d. Insertar tarea.\n", INSERT_TASK);
        printf("%d. Mostrar la lista de tareas.\n", SHOW_TASK_LIST);
        printf("%d. Eliminar una tarea de la lista por su ID.\n", REMOVE_TASK_BY_ID);
        printf("%d. Modificar descripción, encargado y la prioridad de una tarea.\n", MODIFY_TASK_DETAILS);
        printf("%d. Mostrar tareas urgentes.\n", SHOW_URGENT_TASKS);
        printf("%d. Mostrar tareas por empleado.\n", SHOW_TASKS_BY_EMPLOYEE);
        printf("%d. Salir del programa.\n", EXIT);
        printf("Elija una opción (%d-%d):\n", INSERT_TASK, EXIT);
        scanf("%d", &userOption);
    } while(userOption < INSERT_TASK || userOption > EXIT);

    return userOption;
}

int isTaskListEmpty(TTaskList tasks) {
    return tasks.head->next == NULL;
}

void printTaskListDetails(TTaskList tasks) {
    printf("Estas son las tareas dadas de alta actualmente \n");
    showHeader();
    struct nodo *aux = tasks.head->next;
    while (aux != NULL) {
        TTask task = aux->info;
        aux = aux->next;
        printTaskDetails(task);
    }
}

int getTaskID() {
    int taskIDToBeRemoved;

    scanf ("%d", &taskIDToBeRemoved);

    return taskIDToBeRemoved;
}

void printTaskDetails(TTask task) {
    printf("ID: %d \n\n",        task.id);
    printf("Descripcion: %s\n", task.descripcion);
    printf("Encargado: %s\n", task.encargado);
    printf("Prioridad: %s\n \n",  task.prioridad == HIGH_PRIORITY ? HIGH_PRIORITY_TEXT : NORMAL_PRIORITY_TEXT );
    printf("Fecha de creacion: %d/ %d/ %d \n",   task.fechaCreacion.dia, task.fechaCreacion.mes, task.fechaCreacion.anio);
    printf("Fecha de limite: %d/ %d/ %d \n",  task.fechaFinalizacion.dia, task.fechaFinalizacion.mes, task.fechaFinalizacion.anio);
    puts("------------------------------------------------------------------------------------------------------\n");
}

void modifyTaskDetails(TTask *newTask) {

    while ((getchar()) != '\n'); // Limpia el buffer del teclado

    printf("Ingrese la descripcion de la tarea: \n");
    fgets(newTask->descripcion, 20, stdin);

    fflush(stdin);

    printf("Ingrese quien sera el encargado:\n");
    fgets(newTask->encargado, 20, stdin);

    fflush(stdin);
    enterTaskPriority(&newTask->prioridad);

}

void getTaskDetails(TTask *newTask) {

    while ((getchar()) != '\n'); // Limpia el buffer del teclado

    printf("Ingrese la descripcion de la tarea:\n");
    fgets(newTask->descripcion, 20, stdin);

    printf("Ingrese quien sera el encargado:\n");
    fgets(newTask->encargado, 20, stdin);

    enterTaskPriority(&newTask->prioridad);

    printf("Ahora ingrese la fecha de cuando finalizara esta tarea:\n");
    enterTaskDate(&newTask->fechaFinalizacion);
}

struct nodo * CrearNodo(TTask task){
    struct nodo *nuevo = (struct nodo *) malloc (sizeof(struct nodo));

    if (nuevo != NULL){
        nuevo -> info = task;
        nuevo -> next = NULL;
    }

    return nuevo;
}

void enterTaskPriority(int *taskPriority) {
    do {
        printf("Ingrese la prioridad:\n");
        printf("%d - Prioridad alta\n", HIGH_PRIORITY);
        printf("%d - Prioridad normal\n", NORMAL_PRIORITY);
        printf("Elija una prioridad:\n");
        scanf ("%d", taskPriority);
    } while(*taskPriority < HIGH_PRIORITY || *taskPriority > NORMAL_PRIORITY);
}

void enterTaskDate(TFecha *fecha){
    printf("Ingresa el dia \n");
    scanf("%d",&fecha->dia);
    printf("Ingresa el mes \n");
    scanf("%d",&fecha->mes);
    printf("Ingresa el año \n");
    scanf("%d",&fecha->anio);
}

int storeTasks(TTaskList tasks, FILE * taskBD, const char * fileDB) {

    taskBD = fopen(fileDB,"wb");

    if (taskBD == NULL) {
        return ISSUE_FOUND;
    }

    struct nodo *aux = tasks.head->next;
    while (aux != NULL) {
        fwrite(&aux->info, sizeof(aux->info) ,1 ,taskBD);
        aux = aux->next;
    }

    int issueFound = ferror(taskBD);

    fclose(taskBD);

    if (issueFound) {
        return ISSUE_FOUND;
    } else {
        return STORED_TASKS_SUCCESSFULLY;
    }

}

void showHeader() {
    printf("----------------------------------------------------------------------------------------------\n");
}
