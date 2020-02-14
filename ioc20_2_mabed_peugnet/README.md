

# ioc20_2_mabed_peugnet

#Pilote de périphérique simple


## Etape 1

### Question
Dans un premier temps nous avons créé le module noyau que l’on a ensuite compilé sur la carte.
Puis nous avons exécuté la commande d’insertion du module : insmod, cependant le module existait déjà, en effet certain de nos camarades avait oublier de supprimer leurs fichiers. Nous l'avons constaté en utilisant la commande lsmod. De plus avec la commande dmesg, nous avons remarqué que le dernier message du module n’était pas le notre. Nous avons donc supprimé ce module avec la commande rmmod module. Enfin nous avons pu ajouter notre module noyau et voir notre message d’initialisation “MP Hello World!" puis en le retirant nous avons vu le message “MP Goodbye World!".


## Etape 2

### Question
Nous avons su que le paramètre a été bien lu simplement en regardant dans le terminal du kernel après l’insertion de notre module noyau.


## Etape 3

### Question
Pour savoir que le device a été créé, on utilise la commande suivante : cat /proc/devices qui nous permet de consulter tous les devices présent sur le processeur de la carte et trouver celui qui correspond au notre.

insdev lit le fichier /proc/devices dans lequel est enregistré la numéro major choisi par le système. À l'aide de awk on filtre la ligne correspondante à notre module.


