# IOC20 5 Mabed Peugnet

## Serveur

Le serveur est lancé sur le port 8000.

## Etape 1

On créé la fonction `envoyer_message` à partir du main de client2.c, c'est assez pratique parce qu'il n'y a qu'à déplacer certaines lignes de code.

## Etape 2

à partir du fichier ntest.c on ajoute à l'interface un envoi de message en incluant la fonction `envoyer_message` dedans.
On crée  pour ça un joli header à client2.c tout en retirant son main pour pouvoir l'utiliser comme une bibliothèque.
On l'ajoute ensuite au Makefile afin que tout se link correctement.

## Etape 3

On ajoute au serveur le moyen de compter le nombre de vote pour chaque choix avec un array de int.
