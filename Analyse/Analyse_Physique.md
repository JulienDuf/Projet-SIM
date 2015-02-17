#Analyse pour la physique du projet d'int�gration

##Description des �l�ments du jeu contenant de la physique

###Physique de l'environnement
La gravit� : Tous les objets et personnages sont bien s�r affect�s par la gravit�. Elle peut changer en cours de partie.

Le vent : Le vent affecte les projectiles et le mouvement des personnages. Plus le vent sera fort, plus le d�placement sera difficile ou plus il sera facile, tout d�pendra de l'orientation du vent.

L'�lectricit� : Les fils �lectriques et les courants �lectriques cr�eront des champs �lectriques et les balles de fusil seront affect�es � ce champ. Tout autre objet m�tallique aussi affect� � cette force.

###Physique du personnage
Mouvement du personnage : Le personnage est affect� par la gravit� dans chaque d�placement qu'il fait.

Saut du personnage : Le personnage pourra atteindre une hauteur maximale et avoir une port�e qui varie selon sa vitesse.

###Physique des objets
Projectile de lanc�e : Le projectile suit une trajectoire normale, mais qui rebondit quand il atteint un autre objet. Les rebonds varient d'un objet � l'autre. Le projectile cr�e des dommages.

Arme � projectile : L'arme envoie une balle de fusil, ou autre. La particule qui est lanc�e avance en ligne droite tr�s rapidement, mais ne rebondit pas, elle inflige seulement des d�g�ts.

##Librairie utilis�e
SDL [www.libsdl.org](https://www.libsdl.org "www.libsdl.org").

OpenGl [www.opengl.org](https://www.opengl.org "www.opengl.org"). 

##Programme pour la gestion de projet

GanttProject : [www.ganttproject.biz](http://www.ganttproject.biz/index.php)
