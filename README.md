# Brightness-analysis
A brightness analysis built with Arduino
--------------------------------------------------------------
Si chiede di realizzare un sistema che si occupi di rilevare la luminosità di un locale mediante una fotoresistenza ed indicare mediante due led L1 e L2 (uno rosso ed uno verde) lo stato della lettura. Inoltre mediante un terzo led L3 (giallo) è possibile indicare se il sistema è in fase di configurazione (C) o in fase di analisi (A) o in fase di debug (D). Un pulsante (T) serve per switchare ogni volta che si vuole tra le fasi C e A oppure tra le fasi C e D. Durante la fase A il sistema semplicemente si limita a leggere lo stato della luminosità AL ed accendere L1 e L2 secondo quelle che sono le regole stabilite in fase C per il parametro OM. Durante la fase C invece il sistema è in ascolto dalla porta seriale ed attende dall'elaboratore (PC) uno dei possibili comandi per modificare lo stato di funzionamento in fase A. Durante la fase D il sistema opera come in fase A ma comunicando mediante porta seriale con l'elaboratore (D) in modo schematico il suo stato di funzionamento ed i dati via via rilevati).

Le specifiche:

la fase C permette di modificare tramite tastiera il valore di sei parametri
soglia percentuale inferiore LL (numero compreso tra 0 e 100)
soglia percentuale superiore UL (numero compreso tra 0 e 100)
modo di indicazione OM (numero compreso tra 1 e 3)
tempo di aggiornamento RT (numero compreso tra 1 e 100)
numero campioni per calcolo media RC (numero compreso tra 1 e 5)
stato debug DS (valore vero se si vuole che usciti dalla fase C si passi alla fase D, mentre valore falso se si vuole che usciti dalla fase C si passi alla fase A)
ad ogni modifica con successo di un parametro in fase C devono corrispondere 3 lampeggi rapidi del led verde (L2) mentre ad ogni modifica non avvenuta (per errati parametri) devono corrispondere 3 lampeggi rapidi del led rosso)
l'intera configurazione deve essere salvata su memoria non volatile in modo che dopo l'eventuale riavvio del μC il sistema usi esattamente le stesse modalità e valori precedenti
l modi di indicazione (OM) sono 3 e sono:
si accende il led rosso (L1) se la luminosità è al di sotto di LL, si accende il led verde (L2) se la luminosità è al di sopra di UL e si devono accendere entrambi (L1 e L2) nei casi intermedi
si accende il led rosso (L1) se la luminosità è al di sotto di LL, si accende il led verde (L2) se la luminosità è al di sopra di UL e devono restare spenti entrambi (L1 e L2) nei casi intermedi
si accende il led rosso (L1) se la luminosità è al di sotto di LL, si accende il led verde (L2) se la luminosità è al di sopra di UL e si devono accendere entrambi (L1 e L2) ma con la rispettiva luminosità legata alla differenza di valore tra le rispettive soglie di accensione ed il valore letto (es: se LL=10, UL=90, AL=50, allora L1 si accende con luminosità 50% e L2 con luminosità 50%; se LL=10, UL=90, AL=30, allora L1 si accende con luminosità 75% e L2 si accende con luminosità 25%)
il led L3 deve rispettare le seguenti regole:
spento se si è in fase A
acceso se si è in fase C
lampeggiante duty-cycle 50% e periodo pari a RT se si è in fase D (ovvero si vuole che il led lampeggi in concomitanza con ogni nuova lettura della luminosità)
i valori limite della luminosità (pari al buio totale ed alla massima luminosità) possono essere memorizzati mediante due opportune costanti a livello di codice
in fase D si deve provvedere a stampare in modo opportuno tutte le variabili di interesse in modo che sia possibile eseguire effettivamente un debug totale ed in tempo reale del sistema
il tempo di aggiornamento RT deve essere inteso come il moltiplicatore del tempo base di refresh che è di 100ms (quindi ad esempio un RT=1 significa 100ms, un RT=10 significa 1000ms, un RT=100 significa 10000ms ovvero 10s)
il valore RC indica il numero di misure consecutive su cui basare la stima (media) di AL; per esempio un RC=1 significa che ogni lettura produce direttamente un valore di AL, mentre un RC=5 significa che ogni 5 letture si produce un unico valore di AL pari alla media delle 5 letture svolte (si tenga presente però che RT è il tempo di aggiornamento del valore AL e non delle letture)
il codice dello sketch deve essere pulito, coerente, ben commentato e ben organizzato (si faccia buon uso di variabili, costanti e funzioni).
