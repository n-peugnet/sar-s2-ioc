# IOC20_T01

## 2. helloworld

1.  La redirection des ports permet de se connecter à l'une ou l'autre des raspberry pi en passant par un seul routeur.
2.  La raspberry pi n'a qu'un seul compte utilisateur et nous sommes plusieurs à nous connecter sur la même carte. On a donc besoin d'un repertoire par groupe pour ne pas toujours écraser le travail des autres groupes.

## 5. Controle de gpio

1.  `BCM2835_GPIO_BASE` correspond à l'adresse du premier registre du GPIO
2.  Cette structure correspond à l'ensemble des registre codés sur 32 bit du GPIO
3.  gpio_reg_virt se trouve dans l'espace d'adressage du processus.
4.  La variable reg correspond au numéro du registre auquel on doit accédé pour avoir la  valeur de pin.
5.  Comme on peut avoir plusieurs processus qui veulent écrire au même registre simultanément, on doit faire en sorte que les fonctions d'écriture et de lecture soient atomiques. C'est pour celà que l'ecriture est divisée en deux fonction : une pour mettre à 1 et l'autre à 0. Ainsi on écrit à deux adresses différentes en fonction de la valeur de val.
6.  Le flag de open :
O_RDWR : lecture et écriture.
O_SYNC : lecture et ecriture directes (sans buffer).
7.  
    dans mmap, on a :
    - NULL: on laisse le système choisir l'adresse de destination
    - RPI_BLOCK_SIZE : taille du bloc contenant l'ensemble des registres GPIO
    - PROT_READ | PROT_WRITE : Mode avec lequel on mappe la memoire, ici lecture + ecriture.
    - MAP_SHARED : flag indiquand qu'il s'agit de mémoire partagée
    - mmap_fd : le fichier correspondant à la memoire.
    - BCM2835_GPIO_BASE : l'adresse du début de ce qu'on veut mapper (les registres GPIO de la raspi)

8.  delay va convertir les secondes en milisec et fait un sleep correspondant à ce temps.

9.  les adresses auquelles on veut accéder avec mmap ne sont accessible qu'avec l'utilisateur root (fichier /dev/mem).