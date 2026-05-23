# ile celeste ☁️

Projet réalisé par Benoît Baraille, Alexandre Grosdidier, Romane Jouvet

## 1) Bruit fractal
Pour le noise, je suis parti sur le tutoriel de The Book of Shaders et je l’ai intégré.
Le point un peu bloquant a été quand on appelle une fonction dans une autre fonction.
Je ne comprenais pas bien le principe, mais Jules Fouchy me l’a expliqué ! (Fun fact : c’est le même système dans Coollab !)
Après, j’ai ajouté une ImGui, sans oublier d’ajouter un bouton "Appliquer", sinon ça ne recharge pas les fonctions.

## 3) Distribution de points par Poisson disk sampling
J’ai suivi point par point la vidéo tutoriel de Sebastian Lague pour implémenter le Poisson Disk Sampling.
Au départ, je pensais que la grid_size était de 800 par 800, en pensant que c’était la taille en pixels de la fenêtre de l'application. J’avais donc normalisé les valeurs à la fin pour être entre 0 et 1. Mais après réflexion, je suis simplement parti directement de 0 à 1, ce qui évite toute conversion inutile.
Ensuite, ce tutoriel m’a permis de découvrir de nouvelles fonctions incluses dans la bibliothèque standard en C++. Par exemple, la fonction ceil, qui permet d’arrondir au nombre supérieur.
J’ai également adapté le tutoriel en effectuant les modifications nécessaires dans la struct.
