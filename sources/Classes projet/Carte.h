#pragma once
#include <map>
#include <tuple>
#include <list>
#include <vector>
#include <fstream>
#include <cmath>
#include "Modele3D.h"
#include "Porte.h"
#include "Salle.h"
#include "Info.h"
#include "Graphe.h"
#include "GestionnaireChemin.h"
#include "GestionnaireRessources.h"
#include "Gestionnaire3D.h"
#include "LecteurFichier.h"
#include "Joueur.h"
#include "Physique.h"

typedef std::tuple<unsigned int, unsigned int, bool> Entree;
typedef std::tuple<unsigned int, unsigned int> Sortie;
typedef std::tuple<char*, char*, char*> Modele_Text;

class Carte : public Singleton < Carte > {
private:
	graphe::Graphe carte;
	std::map<Entree, Sortie> liens;
	std::list<InfoSalle> infosSalles;
	gfx::Modele3D *modeleMur;
	gfx::Modele3D *modelePorte;
	Vecteur3d vecteur;
	Vecteur3d vecteurAide;

	std::vector<Modele_Text> cheminsModeleText;

	void ajouterLien(Entree entree, Sortie sortie){
		liens[entree] = sortie;
	}

	void positionnerPorte(InfoSalle& salle, InfoObjet& objet, bool recalcul) {
		if (recalcul)
			int i = 0;
		bool boPorte = false;
		bool directionPossible;
		unsigned int pos;
		double x(0), y(0), z(0), xMin, xMax, yMin, yMax, zMin, zMax;
		double randomRatio;
		while (!boPorte) {

			// Obtenir des coordonn�s de la d'une bo�te de collision de la salle.
			boPorte = true;
			auto it = salle.boitesCollision.begin();
			pos = rand() % salle.boitesCollision.size();
			std::advance(it, pos);

			//Donn�es y
			yMin = (*it).obtYMin();
			yMax = (*it).obtYMax();
			y = (*it).obtGrandeurY();

			if (y >= 2) {

				//Donn�es x
				xMin = (*it).obtXMin();
				xMax = (*it).obtXMax();
				x = (*it).obtGrandeurX();

				// Donn�es z
				zMin = (*it).obtZMin();
				zMax = (*it).obtZMax();
				z = (*it).obtGrandeurZ();


				// Positionnement de la porte...
				objet.position.y = yMin;
				directionPossible = false;
				while (!directionPossible)
				{
					switch (rand() % 4) {
					case 0:

						if (z >= 1) {

							randomRatio = (double)rand() / RAND_MAX;
							objet.position.x = xMin + randomRatio * (x);

							do {
								randomRatio = (double)rand() / RAND_MAX;
								objet.position.z = zMin + randomRatio * (z);
							} while ((objet.position.z < zMin) || (objet.position.z > zMax - 1));

							directionPossible = true;
							objet.direction = { 1, 0, 0 };
							objet.rotation = 180;
						}

						break;
					case 1:

						if (z >= 1) {

							randomRatio = (double)rand() / RAND_MAX;
							objet.position.x = xMin + randomRatio * (x);

							do {
								randomRatio = (double)rand() / RAND_MAX;
								objet.position.z = zMin + randomRatio * (z);
							} while ((objet.position.z < zMin + 1) || (objet.position.z > zMax));

							directionPossible = true;
							objet.direction = { -1, 0, 0 };
							objet.rotation = 0;
						}

						break;
					case 2:

						if (x >= 1) {

							do {
								randomRatio = (double)rand() / RAND_MAX;
								objet.position.x = xMin + randomRatio * (x);
							} while ((objet.position.x < xMin + 1) || (objet.position.x > xMax));

							randomRatio = (double)rand() / RAND_MAX;
							objet.position.z = zMin + randomRatio * (z);

							directionPossible = true;
							objet.direction = { 0, 0, 1 };
							objet.rotation = 90;
						}

						break;
					case 3:

						if (x >= 1) {

							do {
								randomRatio = (double)rand() / RAND_MAX;
								objet.position.x = xMin + randomRatio * (x);
							} while ((objet.position.x < xMin) || (objet.position.x > xMax - 1));

							randomRatio = (double)rand() / RAND_MAX;
							objet.position.z = zMin + randomRatio * (z);

							directionPossible = true;
							objet.direction = { 0, 0, -1 };
							objet.rotation = -90;
						}

						break;
					}



				}

				/*
				// Boucle qui v�rifie si la porte est dans une bo�te de remplissage...
				for (auto it : salle.puzzle.boitesRemplissage) {
				if (it.pointDansBoite(objet.position)) {
				boPorte = false;
				}
				switch ((int)objet.rotation) {
				case 0:
				if (it.pointDansBoite({ objet.position.x, objet.position.y, objet.position.z - 1 })) {
				boPorte = false;
				}
				break;
				case 90:
				if (it.pointDansBoite({ objet.position.x - 1, objet.position.y, objet.position.z })) {
				boPorte = false;
				}
				break;
				case 180:
				if (it.pointDansBoite({ objet.position.x, objet.position.y, objet.position.z + 1 })) {
				boPorte = false;
				}
				break;
				case -90:
				if (it.pointDansBoite({ objet.position.x + 1, objet.position.y, objet.position.z })) {
				boPorte = false;
				}
				break;
				}
				}

				// Boucle qui v�rifie si la porte est dans une bo�te de remplissage...
				for (auto it : salle.puzzle.boitesPuzzle) {
				if (it.pointDansBoite(objet.position)) {
				boPorte = false;
				}
				switch ((int)objet.rotation) {
				case 0:
				if (objet.position.z - 1 >= it.obtZMin() && objet.position.z - 1 <= it.obtZMax()) {
				boPorte = false;
				}
				break;
				case 90:
				if (objet.position.x - 1 >= it.obtXMin() && objet.position.x - 1 <= it.obtXMax()) {
				boPorte = false;
				}
				break;
				case 180:
				if (objet.position.z + 1 >= it.obtZMin() && objet.position.z + 1 <= it.obtZMax()) {
				boPorte = false;
				}
				break;
				case -90:
				if (objet.position.x + 1 >= it.obtXMin() && objet.position.x + 1 <= it.obtXMax()) {
				boPorte = false;
				}
				break;
				}
				}

				for (auto it : salle.puzzle.boitesRemplissage) {
				switch ((int)objet.rotation) {
				case 0:
				if (it.pointDansBoiteZ(objet.position.z) ^ it.pointDansBoiteZ(objet.position.z - 1)) {
				boPorte = false;
				}
				break;
				case 90:
				if (it.pointDansBoiteX(objet.position.x) ^ it.pointDansBoiteX(objet.position.x - 1)) {
				boPorte = false;
				}
				break;
				case 180:
				if (it.pointDansBoiteZ(objet.position.z) ^ it.pointDansBoiteZ(objet.position.z + 1)) {
				boPorte = false;
				}
				break;
				case -90:
				if (it.pointDansBoiteX(objet.position.x) ^ it.pointDansBoiteX(objet.position.x + 1)) {
				boPorte = false;
				}
				break;
				}
				}
				*/

				// Boucle qui v�rifie si une porte sera en collision avec une autre...
				for (auto it : salle.Objet) {

					// Si les portes ont la m�me direction...
					if ((objet.direction == it.direction) && !(objet.position == it.position)) {

						// Axe x
						switch ((int)objet.direction.x) {
						case 1:
							if ((objet.position.z >= it.position.z && objet.position.z <= it.position.z + 1) || (objet.position.z + 1 >= it.position.z && objet.position.z + 1 <= it.position.z + 1)) {
								boPorte = false;
							}
							break;
						case -1:
							if ((objet.position.z <= it.position.z && objet.position.z >= it.position.z - 1) || (objet.position.z - 1 <= it.position.z && objet.position.z - 1 >= it.position.z - 1)) {
								boPorte = false;
							}
							break;
						}

						// Axe z
						switch ((int)objet.direction.z) {
						case 1:
							if ((objet.position.x <= it.position.x && objet.position.x >= it.position.x - 1) || (objet.position.x - 1 <= it.position.x && objet.position.x - 1 >= it.position.x - 1)) {
								boPorte = false;
							}
							break;
						case -1:
							if ((objet.position.x >= it.position.x && objet.position.x <= it.position.x + 1) || (objet.position.x + 1 >= it.position.x && objet.position.x + 1 <= it.position.x + 1)) {
								boPorte = false;
							}
							break;
						}
					}
				}
			}

		}

		// Ajout de l'objet.
		if (!recalcul)
			salle.Objet.push_back(objet);
	}

	void creerSalle(InfoSalle& infoSalleActive, bool enPositionnement) {

		if (enPositionnement)
			salleActive = new Salle(new gfx::Modele3D(gfx::GestionnaireRessources::obtInstance().obtModele(infoSalleActive.cheminModele), gfx::Texture()), infoSalleActive.nbrPorte, infoSalleActive.ID);
		else
			salleActive = new Salle(new gfx::Modele3D(gfx::GestionnaireRessources::obtInstance().obtModele(infoSalleActive.cheminModele), gfx::GestionnaireRessources::obtInstance().obtTexture(infoSalleActive.cheminTexture)), infoSalleActive.nbrPorte, infoSalleActive.ID);
		salleActive->defEchelle(infoSalleActive.echelle);

		double orientation;
		bool PorteAuMur;
		Vecteur3d v3dtmp;
		Vecteur3d pivot;
		for (auto& it : infoSalleActive.Objet) {
			if (!enPositionnement) {
				gfx::Modele3D* modeleporte = new gfx::Modele3D(gfx::GestionnaireRessources::obtInstance().obtModele(it.cheminModele), gfx::GestionnaireRessources::obtInstance().obtTexture(it.cheminTexture));
				modeleporte->defPosition(it.position);
				modeleporte->defOrientation(0, it.rotation, 0);
				salleActive->ajoutObjet(new Porte(modeleporte, it.ID, "metal", it.position, { 0, 0, 0 }, false, true, false, false));
			}
			else {
				orientation = 0;
				gfx::Modele3D* modeleporte = new gfx::Modele3D(gfx::GestionnaireRessources::obtInstance().obtModele(it.cheminModele), gfx::Texture());
				modeleporte->defPosition(it.position);
				modeleporte->defOrientation(0, it.rotation, 0);

				Objet* porte = new Porte(modeleporte, it.ID, "metal", it.position, { 0, 0, 0 }, false, true, false, false);
				porte->defVitesse(it.direction);
				porte->defPosition(Vecteur3d(porte->obtPosition().x, porte->obtPosition().y + 0.01, porte->obtPosition().z));
				PorteAuMur = false;

				while (!PorteAuMur) {

					// Tant que ma porte ne frappe pas un mur...
					while (!Physique::obtInstance().collisionPorte(salleActive->obtModele(), *porte, false)) {
						porte->defPosition(porte->obtPosition() + (porte->obtVitesse() / 100));
					}

					// 
					unsigned int compteur = Physique::obtInstance().collisionPorteQuatrePoints(salleActive->obtModele(), *porte);
					switch (compteur) {
					case 0:
						break;
					case 1:
					case 12: // Collision au bas de la porte
						porte->defPosition(porte->obtPosition() + (porte->obtVitesse() / 4));
						v3dtmp = porte->obtVitesse();
						porte->defVitesse({ 0, -1, 0 });
						while (Physique::obtInstance().collisionPorteQuatrePoints(salleActive->obtModele(), *porte) == 12 || Physique::obtInstance().collisionPorteQuatrePoints(salleActive->obtModele(), *porte) == 1)
							porte->defPosition(porte->obtPosition() + Vecteur3d({ 0.0, 0.01, 0.0 }));
						porte->defVitesse(v3dtmp);
						break;
					case 22: // Collision au c�t� rotatif de la porte (vue de face en sortant = c�t� gauche de la porte)
						porte->defPosition(porte->obtPosition() + (porte->obtVitesse() / 4));
						pivot = { 0, -1, 0 };
						v3dtmp = pivot.produitVectoriel(porte->obtVitesse());
						while (Physique::obtInstance().collisionPorteQuatrePoints(salleActive->obtModele(), *porte) == 22)
							porte->defPosition(porte->obtPosition() + (v3dtmp / 100));
						break;
					case 42: // Collision au c�t� poign�e de la porte (vue de face en sortant = c�t� droit de la porte)
						porte->defPosition(porte->obtPosition() + (porte->obtVitesse() / 4));
						pivot = { 0, 1, 0 };
						v3dtmp = pivot.produitVectoriel(porte->obtVitesse());
						while (Physique::obtInstance().collisionPorteQuatrePoints(salleActive->obtModele(), *porte) == 42)
							porte->defPosition(porte->obtPosition() + (v3dtmp / 100));
						break;
					case 52: // Collision au haut de la porte
						porte->defPosition(porte->obtPosition() + (porte->obtVitesse() / 4));
						v3dtmp = porte->obtVitesse();
						porte->defVitesse({ 0, 1, 0 });
						while (Physique::obtInstance().collisionPorteQuatrePoints(salleActive->obtModele(), *porte) == 52)
							porte->defPosition(porte->obtPosition() + Vecteur3d({ 0.0, -0.01, 0.0 }));
						porte->defVitesse(v3dtmp);
						break;
					case 32:
					case 33:
					case 43:
					case 53:
					case 63:
					case 64: // Collision aux quatres points de la porte de la porte
						porte->defPosition(Vecteur3d(porte->obtPosition().x, porte->obtPosition().y + 0.05, porte->obtPosition().z));
						Physique::obtInstance().repositionner(salleActive->obtModele(), *porte);
						porte->defPosition(porte->obtPosition() - (porte->obtVitesse()));
						v3dtmp = porte->obtVitesse();
						porte->defVitesse({ 0, -1, 0 });
						while (!Physique::obtInstance().collisionPorte(salleActive->obtModele(), *porte, true) && porte->obtPosition().y >= -1000)
							porte->defPosition(porte->obtPosition() + (porte->obtVitesse() / 10));
						porte->defVitesse(v3dtmp);
						porte->defPosition(porte->obtPosition() + (porte->obtVitesse()));
						if (porte->obtPosition().y >= -1000)
							PorteAuMur = true;
						else {
							positionnerPorte(infoSalleActive, it, true);
							orientation = 0;
							modeleporte->defPosition(it.position);
							modeleporte->defOrientation(0, it.rotation, 0);
							porte->defVitesse(it.direction);
							porte->defPosition(Vecteur3d(porte->obtPosition().x, porte->obtPosition().y + 0.01, porte->obtPosition().z));
						}
						break;
					default:
						break;
					}

				}
				porte->obtModele3D()->defOrientation(porte->obtModele3D()->obtOrientation() + Vecteur3d(0, orientation, 0));

				it.position = porte->obtPosition();
				it.rotation = porte->obtModele3D()->obtOrientation().y;
				it.direction = porte->obtVitesse();
				delete porte;
			}
		}
		if (enPositionnement) {
			delete salleActive;
		}
	}

public:

	Salle *salleActive;
	unsigned int nombreDeSalle;
	float chargement;
	bool finChargement;

	void initialiser() {
		thread_Creation = std::thread(&Carte::creer, this);
	}

	int destination(std::tuple<unsigned int, unsigned int, bool> sortie, Joueur *joueur) {

		delete salleActive;

		Sortie pieceSuivante = liens[sortie];

		auto debut = infosSalles.begin();
		std::advance(debut, std::get<0>(pieceSuivante));

		creerSalle(*debut, false);

		gfx::Gestionnaire3D::obtInstance().ajouterObjet(modeleMur);
		gfx::Gestionnaire3D::obtInstance().ajouterObjet(modelePorte);

		// Positionnement du joueur...
		auto it = (*debut).Objet.begin();
		std::advance(it, std::get<1>(pieceSuivante));
		Vecteur3d vecteurMur;

		vecteurAide = { (*it).direction.x, (*it).direction.y, (*it).direction.z };
		vecteur = { (*it).position.x + (-1.2 * (*it).direction.x) + (-0.5 * (*it).direction.z), (*it).position.y, (*it).position.z + (-1.2 * (*it).direction.z) + (-0.5 * (*it).direction.x) };
		vecteurMur = { (*it).position.x + (-2.5 * (*it).direction.x), (*it).position.y, (*it).position.z + (-2.5 * (*it).direction.z) };
		modeleMur->defPosition(vecteurMur);
		modelePorte->defPosition(vecteurMur.x + (-1 * (*it).direction.z), vecteurMur.y, vecteurMur.z + (1 * (*it).direction.x));
		modeleMur->defOrientation(0, (*it).rotation, 0);
		modelePorte->defOrientation(0, (*it).rotation + 180, 0);


		joueur->defPosition(vecteur);
		Vecteur3d tempVecteur = Vecteur3d(joueur->obtCamera()->obtDevant().x, 0, joueur->obtCamera()->obtDevant().y);
		joueur->defAngleHorizontal(tempVecteur.angleEntreVecteurs((*it).direction) + 180);
		//joueur->defAngleHorizontal(0);
		//joueur.defDevant((*it).direction);
		//	joueur.defAngleHorizontal((*it).rotation + 180);
		//joueur.defNormale((*it).direction);
		return std::get<1>(pieceSuivante);
	}

	void bougerMur(Joueur *joueur, float frameTime){
		if ((vecteurAide.x != 0) && (vecteurAide.z != 0)){
			if ((modeleMur->obtPosition().x == vecteur.x) && (modeleMur->obtPosition().z == vecteur.z)){
				joueur->deBloquer();
			}

		}
		if (vecteurAide.x != 0){
			if (modeleMur->obtPosition().x == vecteur.x){
				joueur->deBloquer();

			}

		}
		if (vecteurAide.y != 0){
			if (modeleMur->obtPosition().z == vecteur.z){
				joueur->deBloquer();

			}

		}
		if ((modelePorte->obtPosition().x != vecteur.x) && (modelePorte->obtPosition().z != vecteur.z)){
			//joueur->bloquer();
			modeleMur->defPosition(modeleMur->obtPosition() + (vecteurAide)* frameTime);
			modelePorte->defPosition(modelePorte->obtPosition() + (vecteurAide)* frameTime);
			modelePorte->rotationner(0, 0.5, 0);
		}

	}

	// Proc�dure qui permet de cr�er le graphe et la premi�re salle dans laquelle le joueur commence...
	// En entr�e:
	// Param1: Le nombre de salle easy = 15, normal = 20, difficile = 32.
	void creer() {

		SDL_GLContext c = fenetre->obtNouveauContext();

		// Cr�ation du graphe
		carte.creer(nombreDeSalle);
		int itterateurPorte(0);

		chargement = 0;
		finChargement = false;

		int* porte = new int[nombreDeSalle];
		Entree entree;
		Sortie sortie;
		for (unsigned int i = 0; i < nombreDeSalle; ++i)
			porte[i] = 0;

		for (unsigned int i = 0; i < nombreDeSalle; ++i){
			itterateurPorte = 0;
			for (unsigned int j = 0; j < nombreDeSalle; ++j){
				if (carte.matrice[i * nombreDeSalle + j]){
					entree = std::make_tuple(i, itterateurPorte++, false);
					sortie = std::make_tuple(j, porte[j]);
					++porte[j];
					ajouterLien(entree, sortie);

				}
			}
		}

		// Load des salles possibles
		std::ifstream fichierSalle("salle_text.txt");
		std::ifstream fichierObjet("objet_text.txt");

		int itterateur(0);
		while (!fichierSalle.eof()) {
			char* curseur1 = new char[20];
			char* curseur2 = new char[20];
			char* curseur3 = new char[20];
			fichierSalle >> curseur1; fichierSalle >> curseur2; fichierSalle >> curseur3;
			cheminsModeleText.push_back(Modele_Text(curseur1, curseur2, curseur3));
			++itterateur;
		}

		unsigned int aleatoire;
		unsigned int pos;
		double randomRatio;
		bool directionPossible;
		bool boPorte;
		double x(0), y(0), z(0), xMin, xMax, yMin, yMax, zMin, zMax;
		Vecteur3d pos3D(0, 0, 0);
		InfoObjet objet;
		InfoSalle salle;

		// Boucle sur toutes les salles...
		for (unsigned int i = 0; i < nombreDeSalle; ++i){

			salle.ID = i;
			salle.nbrPorte = carte.degreSortant(i);
			salle.echelle = { rand() % 3 + 2.0, 2.0, rand() % 3 + 2.0 };
			//aleatoire = rand() % itterateur;
			aleatoire = rand() % 6; // en attendant que toutes les salles sont conformes
			salle.cheminModele = (char*)(std::get<0>(cheminsModeleText[aleatoire]));
			salle.cheminTexture = (char*)(std::get<1>(cheminsModeleText[aleatoire]));
			LecteurFichier::lireBoite((char*)(std::get<2>(cheminsModeleText[aleatoire])), salle);

			// Boucle sur toutes les portes d'un salle pour les positionner...
			for (unsigned short IDPorte = 0; IDPorte < salle.nbrPorte; ++IDPorte) {
				objet.ID = IDPorte;
				objet.cheminModele = "portePlate.obj";// "HARDCOD�"
				objet.cheminTexture = "portePlate.png";// "HARDCOD�"
				positionnerPorte(salle, objet, false);
			}

			// Ajout et r�initialisation de la salle.
			infosSalles.push_back(salle);
			salle.boitesCollision.clear();
			salle.Objet.clear();
		}

		//infosSalles.resize(1);
		for (auto& it : infosSalles) {
			creerSalle(it, true);
			chargement += (100 / nombreDeSalle);
		}

		auto debut = infosSalles.begin();
		pos = rand() % infosSalles.size();
		std::advance(debut, pos);

		modeleMur = new gfx::Modele3D(gfx::GestionnaireRessources::obtInstance().obtModele("murSalle.obj"), gfx::GestionnaireRessources::obtInstance().obtTexture("murSalle.png"));
		modelePorte = new gfx::Modele3D(gfx::GestionnaireRessources::obtInstance().obtModele("portePlate.obj"), gfx::GestionnaireRessources::obtInstance().obtTexture("portePlate2.png"));
		modeleMur->defOrientation(0, 0, 0);
		modelePorte->defOrientation(0, 0, 0);
		finChargement = true;
		SDL_GL_DeleteContext(c);
	}

	void debut() {
		auto debut = infosSalles.begin();
		int pos = rand() % infosSalles.size();
		std::advance(debut, pos);
		creerSalle(*debut, false);
	}
};
