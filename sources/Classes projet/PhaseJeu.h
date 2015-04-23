#pragma once
#include "PhaseMenuPause.h"

class PhaseJeu : public Phase{

private:

	Joueur* joueur;
	gfx::Texte2D* texte;
	Objet* objetVise;

	void appliquerPhysique(float frameTime) {
		if (joueur->obtVitesse().norme() != 0) {
			if (!Physique::obtInstance().collisionJoueurSalle(joueur)) {
				if (joueur->obtEtat() != STABLE)
					Physique::obtInstance().appliquerGravite(joueur->obtVitesse(), frameTime);
				joueur->defPosition(joueur->obtPosition() + joueur->obtVitesse() * frameTime);
			}
			else{
				if (joueur->obtEtat() != CHUTE)
					joueur->defEtat(CHUTE);
				else if (joueur->obtEtat() == CHUTE){
					if (Physique::obtInstance().collisionAuSol(joueur)){
						joueur->defEtat(STABLE);
						joueur->obtVitesse().y = 0.f;
					}
					else {
						joueur->obtVitesse().x = 0.f;
						joueur->obtVitesse().z = 0.f;
					}
				}

			}
		}
		Physique::obtInstance().appliquerPhysiqueSurListeObjet(frameTime);
	}

	bool detectionObjet() {

		std::list<Objet*> liste = Carte::obtInstance().salleActive->obtListeObjet();
		bool objetDetecte = false;

		for (auto it : liste) {
			if (Physique::obtInstance().distanceEntreDeuxPoints(joueur->obtPosition(), it->obtPosition()) < 2) {
				texte->defTexte("Press E to open the door");
				gfx::Gestionnaire2D::obtInstance().ajouterObjet(texte);
				objetDetecte = true;
				return objetDetecte;
				objetVise = it;
			}
		}

		if (!objetDetecte)
			gfx::Gestionnaire2D::obtInstance().retObjet(texte);
	}

public:

	PhaseJeu() : Phase(){
		joueur = new Joueur(Vecteur3d(-1, 0, 0));
		joueur->ajouterScene();
		texte = new gfx::Texte2D("123", gfx::GestionnaireRessources::obtInstance().obtPolice("arial.ttf", "arial20", 20), Vecteur2f(300, 200));

		Carte::obtInstance().creer(20);
		//Carte::obtInstance().salleActive = new Salle(new gfx::Modele3D(gfx::GestionnaireRessources::obtInstance().obtModele("SalleCarree4x4.obj"), gfx::GestionnaireRessources::obtInstance().obtTexture("SalleCarree4x4.png")), 2, 0);
	}

	void rafraichir(float frameTime) {

		if (!this->pause) {
		
			joueur->deplacement(frameTime);
			appliquerPhysique(frameTime);
			detectionObjet();
			ControlleurAudio::obtInstance().jouerTout(joueur);
		}


		/* by sam
		if (detectionObjet()){
			if (Clavier::toucheAppuyee(SDLK_e)){
				//objetVise->appliquerAction(Interagir);
				joueur->changementSalle(objetVise, joueur);
			}
		}
		*/

		if (Clavier::toucheAppuyee(SDLK_ESCAPE)) {
			pause = true;
			GestionnairePhases::obtInstance().ajouterPhase(new PhaseMenuPause());
			SDL_SetRelativeMouseMode(SDL_FALSE);
			SDL_ShowCursor(SDL_ENABLE);
			gfx::Gestionnaire3D::obtInstance().obtCamera()->defPause(true);
		}
	}

	void remplir() {

	}
};