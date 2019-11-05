# So Project

Edoardo Chiavazza; Matr: 872450
Gamba Michele; Matr: 860928
Gloria Ghirardi; Matr: 857017

## Folder Structure:

>bin

*  gestore      //Eseguibile  

*  studente      //Eseguibile

>config

*   opt.conf        //File di configurazione; valori da passare a studente

>src
  >gestore

*   gestore.c

>obj  
*   config.o   

*   gestore_utils.o    

*   sem_utils.o    

*   studente_utils.o    

*   studente.o    

  >lib

*   config.c  //Metodi per leggere dal file opt.conf

*   config.h  

*   gestore_utils.h  

*   gestore_utils.c  //Metodi utilizzati dal gestore  

*   sem_utils.h  

*   sem_utils.c  

*   studente_utils.c  //Metodi utilizzati dagli studenti   

*   studente_utils.h  

  >studente

*   studente.c

## Relazione progetto Sistemi Operativi:


GESTORE:
Il processo Gestore inizializza tutte le ipcs che si utilizzeranno durante la simulazione.
Per le shared memory abbiamo:
shm_config -> serve per mettere a disposizione, per tutti i processi, i dati letti dal file di configurazione
showcase_config -> è una shared memory in cui si trova array di puntatori a strutture di tipo studente
shm_ipcs -> contiente gli id per collegarsi alle varie ipcs del progetto

Per le message queue abbiamo:
code_answer -> utilizzata per rispondere agli inviti degli altri studenti
code_invite -> utilizzata per invitare gli altri studenti
code_studente_gestore -> utilizzata dagli studenti per comunicare con il gestore

Per i semafori abbiamo:
sem_id -> sincronizza gli studenti per iniziare la simulazione dopo aver fatto in modo che ognuno si collegasse alla propria porzione di memoria della bacheca
config_sem_id -> sincronizza i processi in modo che tutti siano collegati alle ipcs
critic_sem_id -> fa accedere alla sezione degli inviti un processo per volta

Una volta inizializzate tutte le ipcs, si inizializza la lista che conterrà tutti i gruppi.
Il gestore a questo punto inizia a forkare studenti in numero uguale a POP_SIZE, definito nel file di configurazione.
Nello switch case, nella condizione di default, si trova il metodo fill_studente_struct() che inizializza la struttura studente definita in studente_utils.h.

Una volta terminato, il gestore fa partire un Timer con valore SIM_TIME preso dal config ed entra in un while(1) in cui aspetta i messaggi degli studenti per formare i gruppi. Una volta che riceve un messaggio controlla se esiste già il gruppo  (control_existing_gruppo).
Se esiste aggiunge la matr che ha accettato l’invito al gruppo del leader (add_studente_to_gruppo).
Se non esiste il gruppo lo crea (create_new_gruppo) e lo inizializza (init_new_gruppo) aggiungendo il leader e lo studente che ha accettato al gruppo.
Quando la simulazione sta per finire, il tipo messaggio che gli arriva sarà dei leader di gruppo ancora non chiusi che gli chiedono di chiudere i gruppi o di studente che decidono di formare un gruppo singolarmente.
Una volta che la simulazione finisce chiude i processi studenti e inizia a calcolare le medie voto di Architettura degli Elaboratori e di SO.

studenteE:
Una volta creato si attacca alle varie ipcs che gli saranno utili:
shm_config
Showcase_config
config_sem_id
sem_id
critic_sem_id

Lo studente poi entra in un while(1) dove si trova la sezione critica che è controllata dal semaforo (critic_sem_id) in modo che acceda uno studente per volta.
Inoltre se lo studente ha già un gruppo e non è un leader oppure il gruppo ha raggiunto la capienza voluta non entrerà più nella sezione degli inviti per via del metodo free_to_enter().
Una volta nella sezione degli inviti il processo calcola quanto tempo manca prima della fine della simulazione in modo da modificare a run-time il proprio comportamento.
Se manca più dell’80% della simulazione se il processo ha invitato qualcuno controlla se ha ricevuto risposte. In caso di nessuna risposta esce dalla sezione critica e aspetta che gli venga risposto. Nel caso di una risposta positiva scrive al gestore di aggiungere al proprio gruppo lo studente, altrimenti controlla se ha qualche invito da qualche altro studente. Se ne ha allora valuta il voto della matr che l’ha invitato e in base alla fase della simulazione decide se il voto è abbastanza buono. Se accetta l’invito allora il processo non parteciperà più alla simulazione.
Se rifiuta decremento il numero di MAX_REJECT (config file), quando max rejects <= 0 l’invito deve essere per forza accettato.
Se il processo rifiuta tutti i messaggi allora può accedere alla parte degli inviti, per invitare lo studente deve ancora avere inviti a disposizione, NOF_INVITES.

PROBLEMATICHE RISCONTRATE DURANTE LO SVILUPPO:  
Quale è la condizione che lo studente accetti l’invito al gruppo?  
L’idea è che per massimizzare devi avere come minimo il tuo voto.
Ogni studente all’inizio della simulazione prova quindi ad invitare studenti con voti maggiori di almeno 3 voti.
Il voto finale è deciso dal voto massimo di Architettura degli elaboratori del gruppo ma nel caso in cui il numero degli studenti appartenenti al gruppo non corrisponde a valore di nof_elems, cioè la preferenza di membri nel gruppo, si prende una penalità di 3 voti.
Al leader conviene quindi piuttosto avere persone con voti più bassi ma chiudere il gruppo rispettando la preferenza.

Come modifico il comportamento a run-time degli studenti?  
Utilizzo un timer e calcolo la percentuale del tempo rimante con SIM-TIME.
Così divido la simulazione in fasi e faccio in modo che più tempo passa più invito gente con voti più bassi.

Problematica per portabilità:  
Lavorando su sistemi operativi diversi, abbiamo riscontrato errori e warning come la struttura semun dei semafori. Questa union è già definita nel caso di MAC OSX ma in Linux no.
Allora abbiamo risolto con un questa soluzione:  
#if ! defined(__FreeBSD__) && ! defined(__OpenBSD__) && ! defined(__sgi) && ! defined(__APPLE__)
