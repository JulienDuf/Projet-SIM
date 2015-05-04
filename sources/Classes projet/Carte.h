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
typedef std::tuple<char*, char*> Modele_Text;

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

	void positionnerPorte(gfx::Modele3D& modeleSalle, InfoSalle& salle, InfoObjet& objet) {
		unsigned int depart;
		Vecteur3d normale;
		Vecteur3d point[3];

		bool PorteAuMur = false;
		double* vertices = modeleSalle.obtSommetsModifies();
		double* normales = modeleSalle.obtNormalesModifies();
		while (!PorteAuMur)
		{
			PorteAuMur = true;
			do{
				double y;
				do{
					depart = (rand() % modeleSalle.obtModele()->obtNbrFaces()) * 9;
					normale = { normales[depart], normales[depart + 1], normales[depart + 2] };
				} while (normale.y != 0);
				for (int i = 0; i < 3; ++i) {

					point[i] = { vertices[depart + i * 3], vertices[depart + i * 3 + 1], vertices[depart + i * 3 + 2] };

				}
			} while (!espacePorte(point[0], point[1], point[2]));

			// Angle de la porte...
			if (Vecteur3d({ 1, 0, 0 }).produitScalaire(normale) > 0) {

				Vecteur3d pivot = { 0, 1, 0 };
				Vecteur3d pointDeRotPoingnee = Vecteur3d({ -1, 0, 0 }).produitVectoriel(pivot);
				double angle = pointDeRotPoingnee.angleEntreVecteurs(normale);
				objet.rotation = (90 - Maths::radianADegre(angle));
				objet.direction = normale * -1;
			}
			else
			{
				Vecteur3d pivot = { 0, -1, 0 };
				Vecteur3d pointDeRotPoingnee = Vecteur3d({ -1, 0, 0 }).produitVectoriel(pivot);
				double angle = pointDeRotPoingnee.angleEntreVecteurs(normale);
				objet.rotation = (270 - Maths::radianADegre(angle));
				objet.direction = normale * -1;
			}

			double y1 = point[0].y;
			double y2;
			do {
				y2 = point[rand() % 2 + 1].y;
			} while (y1 == y2);

			objet.position.y = (y1 < y2) ? y1 : y2;

			unsigned int i;
			unsigned int j;
			do {
				do{
					i = rand() % 3;
					j = rand() % 3;
				} while (i == j);
			} while (point[i].y != point[j].y);

			Vecteur3d swap;

			if (abs(normale.x) != 1 && abs(normale.z) != 1) {
				if ((normale.x >= 0 && normale.z >= 0) || (normale.x < 0 && normale.z >= 0)) {
					if (point[i].x > point[j].x) {
						swap = point[i];
						point[i] = point[j];
						point[j] = swap;
					}
				}
				else if ((normale.x < 0 && normale.z < 0) || (normale.x >= 0 && normale.z < 0)) {
					if (point[i].x < point[j].x) {
						swap = point[i];
						point[i] = point[j];
						point[j] = swap;
					}
				}
			}
			else
			{
				switch ((int)normale.x) {
				case 1:
					if (point[i].z < point[j].z) {//ok
						swap = point[i];
						point[i] = point[j];
						point[j] = swap;
					}
					break;
				case -1:
					if (point[i].z > point[j].z) {
						swap = point[i];
						point[i] = point[j];
						point[j] = swap;
					}
					break;
				}
				switch ((int)normale.z) {
				case 1:
					if (point[i].x > point[j].x) {
						swap = point[i];
						point[i] = point[j];
						point[j] = swap;
					}
					break;
				case -1:
					if (point[i].x < point[j].x) {
						swap = point[i];
						point[i] = point[j];
						point[j] = swap;
					}
					break;
				}
			}

			Vecteur3d vecteurRatio = Physique::obtInstance().vecteurEntreDeuxPoints(point[i], point[j]);
			vecteurRatio *= ((vecteurRatio.norme() - 1) / vecteurRatio.norme());
			vecteurRatio *= ((double)rand() / RAND_MAX);
			Vecteur3d position = point[i] + vecteurRatio;
			objet.position.x = position.x;
			objet.position.z = position.z;

			// Boucle qui v�rifie si une porte sera en collision avec une autre...
			Vecteur3d pivot = { 0, 1, 0 };
			for (auto it_Porte : salle.Objet) {

				// Si les portes ont la m�me direction...
				if ((objet.direction == it_Porte.direction) && !(objet.position == it_Porte.position)) {

					Vecteur3d it_PortePosPoignee = it_Porte.position + it_Porte.direction.produitVectoriel(pivot);
					Vecteur3d it_PosPoignee = objet.position + objet.direction.produitVectoriel(pivot);

					if ((objet.position.x >= it_Porte.position.x && objet.position.x <= it_PortePosPoignee.x) && (objet.position.z >= it_Porte.position.z && objet.position.z <= it_PortePosPoignee.z)) {
						PorteAuMur = false;
					}

					if ((it_PosPoignee.x >= it_Porte.position.x && it_PosPoignee.x <= it_PortePosPoignee.x) && (it_PosPoignee.z >= it_Porte.position.z && it_PosPoignee.z <= it_PortePosPoignee.z)) {
						PorteAuMur = false;
					}
				}
			}
		}
	}

	bool espacePorte(Vecteur3d point1, Vecteur3d point2, Vecteur3d point3) {

		if (point1.y != point2.y && point1.y != point3.y && point2.y != point3.y) {
			return false;
		}

		Vecteur3d pointDeCalcul1;
		Vecteur3d pointDeCalcul2;

		if (point1.y != point2.y && point1.y != point3.y) {
			pointDeCalcul1 = point1;
			if (point1.x == point2.x && point1.z == point2.z) {
				pointDeCalcul2 = point3;
			}
			else
			{
				pointDeCalcul2 = point2;
			}
		}
		else if (point2.y != point1.y && point2.y != point3.y) {
			pointDeCalcul1 = point2;
			if (point2.x == point3.x && point2.z == point3.z) {
				pointDeCalcul2 = point1;
			}
			else
			{
				pointDeCalcul2 = point3;
			}
		}
		else {
			pointDeCalcul1 = point3;
			if (point3.x == point1.x && point3.z == point1.z) {
				pointDeCalcul2 = point2;
			}
			else
			{
				pointDeCalcul2 = point1;
			}
		}

		Vecteur3d hypothenuse = Physique::obtInstance().vecteurEntreDeuxPoints(pointDeCalcul1, pointDeCalcul2);

		if (abs(hypothenuse.y) <= 2) {
			return false;
		}

		if (SDL_sqrt(SDL_pow(hypothenuse.x, 2) + SDL_pow(hypothenuse.z, 2)) <= 1) {
			return false;
		}
		return true;
	}

	void creerSalle(InfoSalle& infoSalleActive) {
		
		salleActive = new Salle(new gfx::Modele3D(gfx::GestionnaireRessources::obtInstance().obtModele(infoSalleActive.cheminModele), gfx::GestionnaireRessources::obtInstance().obtTexture(infoSalleActive.cheminTexture)), infoSalleActive.nbrPorte, infoSalleActive.ID);
		salleActive->defEchelle(infoSalleActive.echelle);

		Objet* porte;

		for (auto& it : infoSalleActive.Objet) {
				gfx::Modele3D* modeleporte = new gfx::Modele3D(gfx::GestionnaireRessources::obtInstance().obtModele(it.cheminModele), gfx::GestionnaireRessources::obtInstance().obtTexture(it.cheminTexture));
				modeleporte->defPosition(it.position);
				modeleporte->defOrientation(0, it.rotation, 0);
				salleActive->ajoutObjet(new Porte(modeleporte, it.ID, "metal", it.position, { 0, 0, 0 }, false, true, false, false));
		}
	}

public:

	Salle *salleActive;
	unsigned int nombreDeSalle;
	double chargement;
	bool finChargement;

	void initialiser() {
		thread_Creation = std::thread(&Carte::creer, this);
	}

	int destination(std::tuple<unsigned int, unsigned int, bool> sortie, Joueur *joueur) {

		delete salleActive;

		Sortie pieceSuivante = liens[sortie];

		auto debut = infosSalles.begin();
		std::advance(debut, std::get<0>(pieceSuivante));

		creerSalle(*debut);

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
			fichierSalle >> curseur1; fichierSalle >> curseur2;
			cheminsModeleText.push_back(Modele_Text(curseur1, curseur2));
			++itterateur;
		}

		unsigned int aleatoire;
		InfoObjet objet;
		InfoSalle salle;
		gfx::Modele3D* modeleSalle;

		// Boucle sur toutes les salles...
		for (unsigned int i = 0; i < nombreDeSalle; ++i) {

			salle.ID = i;
			salle.nbrPorte = carte.degreSortant(i);
			salle.echelle = { rand() % 3 + 2.0, 2.0, rand() % 3 + 2.0 };
			aleatoire = rand() % itterateur;
			//aleatoire = rand() % 8; // en attendant que toutes les salles sont conformes
			salle.cheminModele = (char*)(std::get<0>(cheminsModeleText[aleatoire]));
			salle.cheminTexture = (char*)(std::get<1>(cheminsModeleText[aleatoire]));

			modeleSalle = new gfx::Modele3D(gfx::GestionnaireRessources::obtInstance().obtModele(salle.cheminModele), new gfx::Texture());
			modeleSalle->defEchelle(salle.echelle.x, salle.echelle.y, salle.echelle.z);
			// Boucle sur toutes les portes d'un salle pour les positionner...
			for (unsigned short IDPorte = 0; IDPorte < salle.nbrPorte; ++IDPorte) {
				objet.ID = IDPorte;
				objet.cheminModele = "portePlate.obj";// "HARDCOD�"
				objet.cheminTexture = "portePlate.png";// "HARDCOD�"
				positionnerPorte(*modeleSalle, salle, objet);
				salle.Objet.push_back(objet);
			}
			// Ajout et r�initialisation de la salle.
			delete modeleSalle;
			infosSalles.push_back(salle);
			salle.boitesCollision.clear();
			salle.Objet.clear();
			chargement += (100.0f / nombreDeSalle);
		}

		finChargement = true;
		//SDL_GL_DeleteContext(c);
	}

	void debut() {
		auto debut = infosSalles.begin();
		std::advance(debut, rand() % infosSalles.size());
		creerSalle(*debut);
		modeleMur = new gfx::Modele3D(gfx::GestionnaireRessources::obtInstance().obtModele("murSalle.obj"), gfx::GestionnaireRessources::obtInstance().obtTexture("murSalle.png"));
		modelePorte = new gfx::Modele3D(gfx::GestionnaireRessources::obtInstance().obtModele("portePlate.obj"), gfx::GestionnaireRessources::obtInstance().obtTexture("portePlate.png"));
		modeleMur->defOrientation(0, 0, 0);
		modelePorte->defOrientation(0, 0, 0);
	}
};
