#pragma once
#include <stack>
#include <list>
#include <math.h>
#include "PhaseMenuPause.h"
#include "Maths.h"
#include "Item.h"
#include "MenuAccesRapide.h"
#include "PhaseMenuInventaire.h"
#include "GestionnaireSucces.h"
#include "UsineItem.h"

class PhaseJeu : public Phase{

private:

	Joueur* joueur;
	gfx::Texte2D* texteUtiliser;
	Objet* objetVise;
	bool toucheRelachee;
	bool retour;
	std::stack<unsigned int> cheminRecursif;
	std::list<unsigned int> cheminLogique;
	double iterateur_x, iterateur_z;
	int sensPrecedent, sensActuel;
	Chrono tempsJeu;
	gfx::Texte2D* texteChrono;
	std::string stringTexteChrono;
	double tempsRestant;
	int heures, minutes, secondes;
	Item *itemEquipe;
	Item *test;
	char dizaine[5];
	char unite[5];
	MenuAccesRapide* accesRapide;
	gfx::Sprite2D* point;

	double exponentielle(double a, double b, double h, double k, double x, int limite){
		double temp = a * pow(M_E, b * (x - h)) + k;
		if (temp < limite){
			return temp;
		}
		return limite;
	}

	void santeMentale(){
		double pourcentagePerdu(0);
		if (cheminRecursif.size() > 1){
			pourcentagePerdu += exponentielle(1, 0.2f, 0, -1, cheminRecursif.size(), 5);
		}
		int idActif = cheminLogique.front();
		std::list<unsigned int>::iterator it = cheminLogique.begin();
		for (++it; it != cheminLogique.end(); ++it){
			if (((*it) == idActif) && ((iterateur_x > 2.5f) || (iterateur_z > 2.5))){
				pourcentagePerdu += exponentielle(1, -0.5f, -3.8f, -1, cheminLogique.size(), 5);
				cheminLogique.erase(cheminLogique.begin(), --it);
				iterateur_x = 0;
				iterateur_z = 0;
				break;
			}
		}
		short santeMentalePrecedente = joueur->obtSanteMentale();
		joueur->defSanteMentale((double)joueur->obtSanteMentale() - ((double)joueur->obtSanteMentale() * (pourcentagePerdu / 100.f)));
		if (joueur->obtSanteMentale() != santeMentalePrecedente)
			GestionnaireSucces::obtInstance().obtSucces(0);
		if ((joueur->obtSanteMentale() < 25) && !(GestionnaireSucces::obtInstance().obtChronoDeclenche())){
			GestionnaireSucces::obtInstance().obtChronoSanteMentale()->repartir();
			GestionnaireSucces::obtInstance().defChronoDeclenche();
		}
	}

	void mettreAJourtexteChrono(){
		stringTexteChrono = "";
		char temp[255];
		heures = tempsRestant / 3600;
		minutes = (int)(tempsRestant / 60) % 60;
		secondes = (int)tempsRestant % 60;

		if (heures > 10){
			stringTexteChrono.append(SDL_itoa(heures, temp, 10));
		}
		else{
			stringTexteChrono.append("0");
			stringTexteChrono.append(SDL_itoa(heures, temp, 10));
		}
		stringTexteChrono.append(":");
		if (minutes > 10){
			stringTexteChrono.append(SDL_itoa(minutes, temp, 10));
		}
		else{
			stringTexteChrono.append("0");
			stringTexteChrono.append(SDL_itoa(minutes, temp, 10));
		}
		stringTexteChrono.append(":");
		if (secondes > 10){
			stringTexteChrono.append(SDL_itoa(secondes, temp, 10));
		}
		else{
			stringTexteChrono.append("0");
			stringTexteChrono.append(SDL_itoa(secondes, temp, 10));
		}


		texteChrono->defTexte(&stringTexteChrono);
	}

	void appliquerPhysique(float frameTime) {
		if (joueur->obtVitesse().norme() != 0) {
			if (joueur->obtNormale().y != 1) {
				Physique::obtInstance().appliquerGravite(joueur->obtVitesse(), frameTime);
			}
			//if (joueur->obtNormaleMur().y == 0. && (joueur->obtNormaleMur().x != 0. || joueur->obtNormaleMur().z != 0.)){
			//	joueur->longer();
			//}
			joueur->defPosition(joueur->obtPosition() + joueur->obtVitesse() * frameTime);
			iterateur_x += joueur->obtVitesse().x * frameTime;
			iterateur_z += joueur->obtVitesse().z * frameTime;
			Physique::obtInstance().collisionJoueurSalle(Carte::obtInstance().salleActive->obtModele(), joueur);
			//Physique::obtInstance().collisionJoueurObjet(joueur, Carte::obtInstance().salleActive->obtListeObjet());
		}
		Physique::obtInstance().appliquerPhysiqueSurListeObjet(Carte::obtInstance().salleActive->obtModele(), Carte::obtInstance().salleActive->obtListeObjet(), frameTime, tempsJeu.obtTempsEcoule().enSecondes());
	}

	bool detectionObjet() {
		
		std::list<Objet*> liste = Carte::obtInstance().salleActive->obtListeObjet();
		bool objetDetecte = false;
		
		for (auto it : liste) {
			Porte* it_Porte = dynamic_cast<Porte*>(it);
			Item* it_Item = dynamic_cast<Item*>(it);
			Commutateur* it_com = dynamic_cast<Commutateur*>(it);
			ObjetFixe* it_fixe = dynamic_cast<ObjetFixe*>(it);
			if(it_Porte || !it_fixe){
				Vecteur3d pointCollision, normale;
				Droite VueJoueur = Droite(joueur->obtPosition() + (Vecteur3d(0.0, joueur->obtModele3D()->obtModele()->obtTaille().y, 0.0)), joueur->obtDevant());
				if ((Maths::distanceEntreDeuxPoints(joueur->obtPosition(), it->obtPosition()) < 2) && Physique::obtInstance().collisionDroiteModele(it->obtModele3D(), VueJoueur, pointCollision, normale, nullptr, false)) {
					
					std::string str1 = "Press ";
					str1.append(*GestionnaireControle::obtInstance().obtTouche((UTILISER)));
					
					if (it_Porte != nullptr){
						str1.append(" to open door.");
					}
					else if (it_Item != nullptr){
						str1.append(" to pick up ");
						str1.append(it_Item->obtNom());
					}
					else if (it_com != nullptr){
						if (it_com->obtEtat())
							str1.append(" to turn off");
						else
							str1.append(" to turn on");
					}
					texteUtiliser->defTexte(&str1);
					gfx::Gestionnaire2D::obtInstance().ajouterObjet(texteUtiliser);
					objetDetecte = true;
					objetVise = it;
				}
			}
		}
		
		if (!objetDetecte)
			gfx::Gestionnaire2D::obtInstance().retObjet(texteUtiliser);
		return objetDetecte;
	}

public:

	PhaseJeu(Vecteur3d positionJoueur, double hAngle, double vAngle) : Phase(){
		joueur = new Joueur(positionJoueur, hAngle, vAngle);
		joueur->defEtat(CHUTE);
		joueur->ajouterScene();

		GestionnairePhases::obtInstance().ajouterPhase(new PhaseMenuInventaire(joueur->obtInventaire()));

		texteUtiliser = new gfx::Texte2D(new std::string("123"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 20), Vecteur2f(300, 200));
		toucheRelachee = false;

		cheminRecursif.push(Carte::obtInstance().salleActive->obtID());
		cheminLogique.push_back(Carte::obtInstance().salleActive->obtID());
		iterateur_x = 0;
		iterateur_z = 0;
		tempsJeu = Chrono();

		itemEquipe = nullptr;

		test = UsineItem::obtInstance().obtItemParType(11, 0);

		joueur->obtInventaire()->ajouterObjet(test);
		joueur->obtInventaire()->ajouterObjet(UsineItem::obtInstance().obtItemParType(10, 0));
		accesRapide = new MenuAccesRapide(joueur->obtInventaire());
		accesRapide->remplir();
		GestionnaireEvenements::obtInstance().ajouterUnRappel(SDL_KEYDOWN, std::bind(&PhaseJeu::toucheAppuyee, this, std::placeholders::_1));
		GestionnaireEvenements::obtInstance().ajouterUnRappel(SDL_CONTROLLERBUTTONDOWN, std::bind(&PhaseJeu::toucheAppuyee, this, std::placeholders::_1));

		point = new gfx::Sprite2D(Vecteur2f(638, 358), gfx::GestionnaireRessources().obtTexture("Ressources/Texture/point.png"));
		gfx::Gestionnaire2D::obtInstance().ajouterObjet(point);
		retour = false;
		pause = false;
		for (int i = 0; i < 4; ++i){
			unite[i] = '\0';
			dizaine[i] = '\0';
		}

		texteChrono = new gfx::Texte2D(new std::string(""), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 40), Vecteur2f(RESOLUTION_DEFAUT_X / 2 - 40, 670));
		stringTexteChrono = "";
		if (Carte::obtInstance().nombreDeSalle == 15)
			tempsRestant = 3600;
		else
			tempsRestant = 1800;
		mettreAJourtexteChrono();
		gfx::Gestionnaire2D().obtInstance().ajouterObjet(texteChrono);
		tempsJeu = Chrono();
	}

	~PhaseJeu(){
		delete joueur;
		delete texteUtiliser;
		delete point;
		delete accesRapide;
		delete texteChrono;
	}

	void rafraichir(float frameTime) {
		GestionnaireSucces::obtInstance().obtSucces(2);
		if (pause)
			return;

		if ((Clavier::toucheAppuyee(SDLK_q)) || Manette::boutonAppuyer(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)){
			if (itemEquipe != nullptr){
				itemEquipe->defEtat(EtatItem::DEPOSE);
				GestionnaireSucces::obtInstance().defItemOuiNonLache(joueur->obtInventaire()->obtObjetAccesRapide(joueur->obtInventaire()->obtItemSelectionne()));
				joueur->obtInventaire()->retirerObjetAccesRapide(joueur->obtInventaire()->obtItemSelectionne());
				GestionnaireSucces::obtInstance().defNbrItems(GestionnaireSucces::obtInstance().obtNbrItems() - 1);
				itemEquipe = nullptr;
			}
		}
		if (!this->pause) {
			if (Carte::obtInstance().salleActive->obtID() == (Carte::obtInstance().nombreDeSalle + 2)){
				Carte::obtInstance().salleActive->obtObjet(0)->defPosition(joueur->obtPosition());
				Carte::obtInstance().salleActive->obtObjet(0)->defCollisionInterne(true);
				GestionnaireSucces::obtInstance().obtSucces(12);
			}
			joueur->deplacement();
			appliquerPhysique(frameTime);
			joueur->obtInventaire()->actualiser();
			itemEquipe = joueur->obtInventaire()->obtObjetAccesRapide(joueur->obtInventaire()->obtItemSelectionne());
			if (itemEquipe != nullptr){
				itemEquipe->actualiser(Carte::obtInstance().salleActive, joueur, frameTime);
			}
			accesRapide->actualiserAffichage();
			ControlleurAudio::obtInstance().jouer(COEUR, joueur);
			ControlleurAudio::obtInstance().jouer(PAS, joueur);
			detectionObjet();
			ControlleurAudio::obtInstance().jouerTout(joueur);
			Carte::obtInstance().animationTransitionSalle(joueur, frameTime);
			Carte::obtInstance().animationLeverLit(joueur, frameTime);
			if (Carte::obtInstance().salleActive->obtID() != Carte::obtInstance().nombreDeSalle){
				tempsRestant -= tempsJeu.obtTempsEcoule().enSecondes();
				mettreAJourtexteChrono();
			}
			tempsJeu.repartir();
		}

		if (detectionObjet()){
			if ((Clavier::toucheRelachee(SDLK_e) && Manette::boutonRelacher(SDL_CONTROLLER_BUTTON_Y)) && toucheRelachee){// Touche relach�e bient�t...
				if (objetVise->obtSiPorte()){
					if (objetVise->obtSiPorte()){
						if (Carte::obtInstance().salleActive->obtID() == Carte::obtInstance().nombreDeSalle + 1){
							if (objetVise->obtID()){
								Commutateur* com = nullptr;
								for (auto it : Carte::obtInstance().salleActive->obtListeObjet()){
									com = dynamic_cast<Commutateur*>(it);
									if (com){
										if (com->obtID() < 15){
											dizaine[com->obtID() - 11] = com->obtEtat() + 48;
										}
										else{
											dizaine[4] = '\0';
											unite[com->obtID() - 15] = com->obtEtat() + 48;
										}
									}
								}
								unite[4] = '\0';
								short diz, uni;
								diz = strtoull(dizaine, NULL, 2);
								uni = strtoull(unite, NULL, 2);
								Carte::obtInstance().ajouterLien(std::make_tuple(Carte::obtInstance().salleActive->obtID(), objetVise->obtID(), false), std::make_tuple(diz * 10 + uni,0));
							}
						}
					}
					Carte::obtInstance().destination(std::make_tuple(Carte::obtInstance().salleActive->obtID(), objetVise->obtID(), false), joueur);
					if (Carte::obtInstance().salleActive->obtID() != cheminRecursif.top()){
						for (int i = 0; i < cheminRecursif.size(); ++i)
							cheminRecursif.pop();
					}
					cheminRecursif.push(Carte::obtInstance().salleActive->obtID());
					cheminLogique.push_front(Carte::obtInstance().salleActive->obtID());
					santeMentale();
				}
				else if (dynamic_cast<Item*>(objetVise)){
					joueur->obtInventaire()->ajouterObjet((Item*)objetVise);
					GestionnaireSucces::obtInstance().defNbrItems(GestionnaireSucces::obtInstance().obtNbrItems() + 1);
					char* nom = dynamic_cast<Item*>(objetVise)->obtNom();
					GestionnaireSucces::obtInstance().verifierOuiNon((Item*)objetVise);
					GestionnaireSucces::obtInstance().obtSucces(1);
					if (nom == "Water")
						GestionnaireSucces::obtInstance().obtSucces(18);
					if (nom == "Holy Rod")
						GestionnaireSucces::obtInstance().obtSucces(17);
				    if (nom == "Luger P08" || "Thompson M1")
						GestionnaireSucces::obtInstance().obtSucces(8);
					if (nom == "Grenade")
						GestionnaireSucces::obtInstance().obtSucces(9);
					if (nom == "Note")
						GestionnaireSucces::obtInstance().obtSucces(13);
					if (nom == "Corrections")
						GestionnaireSucces::obtInstance().obtSucces(24);
					objetVise = nullptr;
				}
				else if (dynamic_cast<Commutateur*>(objetVise)){
					dynamic_cast<Commutateur*>(objetVise)->actioner();
				}
				toucheRelachee = false;
			}
			if ((Clavier::toucheAppuyee(SDLK_e) || Manette::boutonAppuyer(SDL_CONTROLLER_BUTTON_Y)))
				toucheRelachee = true;
		}
		if ((GestionnaireSucces::obtInstance().obtChronoDeclenche()) && (GestionnaireSucces::obtInstance().obtChronoSanteMentale()->obtTempsEcoule().enMillisecondes() > 120000)){
			GestionnaireSucces::obtInstance().obtSucces(3);
		}
		if ((GestionnaireSucces::obtInstance().obtChronoDeclenche()) && (GestionnaireSucces::obtInstance().obtChronoSanteMentale()->obtTempsEcoule().enMillisecondes() > 180000)){
			GestionnaireSucces::obtInstance().obtSucces(4);
		}
		if ((GestionnaireSucces::obtInstance().obtChronoDeclenche()) && (GestionnaireSucces::obtInstance().obtChronoSanteMentale()->obtTempsEcoule().enMillisecondes() > 240000)){
			GestionnaireSucces::obtInstance().obtSucces(5);
		}
		if ((GestionnaireSucces::obtInstance().obtChronoDeclenche()) && (GestionnaireSucces::obtInstance().obtChronoSanteMentale()->obtTempsEcoule().enMillisecondes() > 300000)){
			GestionnaireSucces::obtInstance().obtSucces(6);
		}
	}

	void toucheAppuyee(SDL_Event &event){
		if (pause)
			return;
		if (retour){
			retour = false;
			return;
		}
		if ((event.key.keysym.sym == SDLK_ESCAPE) || Manette::boutonAppuyer(SDL_CONTROLLER_BUTTON_START)) {
			defPause(true);
			gfx::Gestionnaire3D::obtInstance().obtCamera()->bloquer();
			GestionnairePhases::obtInstance().defPhaseActive(MENUPAUSE);
			GestionnairePhases::obtInstance().obtPhaseActive()->defPause(false);
			GestionnairePhases::obtInstance().obtPhaseActive()->remplir();
			Curseur::defPosition(Vecteur2f(fenetre->obtTaille().x / 2, fenetre->obtTaille().y / 2));
			curseur->remplir();
		}
		if ((event.key.keysym.sym == SDLK_TAB) || Manette::boutonAppuyer(SDL_CONTROLLER_BUTTON_BACK)) {
			defPause(true);
			gfx::Gestionnaire3D::obtInstance().obtCamera()->bloquer();
			GestionnairePhases::obtInstance().defPhaseActive(MENUINVENTAIRE);
			GestionnairePhases::obtInstance().obtPhaseActive()->defPause(false);
			GestionnairePhases::obtInstance().obtPhaseActive()->remplir();
			Curseur::defPosition(Vecteur2f(fenetre->obtTaille().x / 2, fenetre->obtTaille().y / 2));
			curseur->remplir();
			accesRapide->vider();
		}
	}

	void remplir() {
		accesRapide->remplir();
	}

	void defPause(bool pause) {
		if (!pause)
			retour = true;
		this->pause = pause;
	}

	void actualiserEchelle(Vecteur2f vecteurEchelle) {
	}

	float obtTemps() {
		return tempsJeu.obtTempsEcoule().enSecondes();
	}
};

