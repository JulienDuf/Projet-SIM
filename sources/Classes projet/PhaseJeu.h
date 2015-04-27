#pragma once
#include "PhaseMenuPause.h"

class PhaseJeu : public Phase{

private:

	Joueur* joueur;
	gfx::Texte2D* texte;
	Objet* objetVise;

	bool toucheRelachee;

	void appliquerPhysique(float frameTime) {
 		if (joueur->obtVitesse().norme() != 0) {
			if (!Physique::obtInstance().collisionJoueurSalle(Carte::obtInstance().salleActive, joueur)) {
				if (joueur->obtEtat() != STABLE)
					Physique::obtInstance().appliquerGravite(joueur->obtVitesse(), frameTime);
				joueur->defPosition(joueur->obtPosition() + joueur->obtVitesse() * frameTime);
			}
			else{
				if (joueur->obtEtat() != CHUTE)
					joueur->defEtat(CHUTE);
				else if (joueur->obtEtat() == CHUTE){
					if (Physique::obtInstance().collisionAuSol(Carte::obtInstance().salleActive, joueur)){
						joueur->defEtat(STABLE);
						joueur->obtVitesse().y = 0.f;
					}
					joueur->obtVitesse().x = 0.f;
					joueur->obtVitesse().z = 0.f;
				}

			}
		}
		Physique::obtInstance().appliquerPhysiqueSurListeObjet(Carte::obtInstance().salleActive, frameTime);
	}

	bool detectionObjet() {

		std::list<Objet*> liste = Carte::obtInstance().salleActive->obtListeObjet();
		bool objetDetecte = false;

		for (auto it : liste) {
			if (Physique::obtInstance().distanceEntreDeuxPoints(joueur->obtPosition(), it->obtPosition()) < 2) {
				texte->defTexte("Press E to open the door");
				gfx::Gestionnaire2D::obtInstance().ajouterObjet(texte);
				objetDetecte = true;
				objetVise = it;
				return objetDetecte;
			}
		}

		if (!objetDetecte)
			gfx::Gestionnaire2D::obtInstance().retObjet(texte);
		return objetDetecte;
	}

public:

	PhaseJeu() : Phase(){
		toucheRelachee = false;
		joueur = new Joueur(Vecteur3d(-1, 0, 0));
		joueur->ajouterScene();
		texte = new gfx::Texte2D("123", gfx::GestionnaireRessources::obtInstance().obtPolice("arial.ttf", "arial20", 20), Vecteur2f(300, 200));

		Carte::obtInstance().creer(20);
	}

	void rafraichir(float frameTime) {

		if (!this->pause) {
			joueur->deplacement(frameTime);
			appliquerPhysique(frameTime);
			ControlleurAudio::obtInstance().jouer(COEUR, joueur);
			ControlleurAudio::obtInstance().jouer(PAS, joueur);
			detectionObjet();
			ControlleurAudio::obtInstance().jouerTout(joueur);
		}


		if (detectionObjet()){
			if (Clavier::toucheAppuyee(SDLK_e) && toucheRelachee){// Touche relach�e bient�t...
				if (objetVise->obtSiPorte()){
					Carte::obtInstance().destination(std::make_tuple(Carte::obtInstance().salleActive->obtID(), objetVise->obtID(), false), *joueur);
				}
				else{
					objetVise->appliquerAction(Interagir);
				}
				toucheRelachee = false;
			}
			if (Clavier::toucheAppuyee(SDLK_e))
				toucheRelachee = true;
		}

		if (Clavier::toucheAppuyee(SDLK_ESCAPE)) {
			pause = true;
			GestionnairePhases::obtInstance().ajouterPhase(new PhaseMenuPause());
			SDL_SetRelativeMouseMode(SDL_FALSE);
			SDL_ShowCursor(SDL_ENABLE);
			gfx::Gestionnaire3D::obtInstance().obtCamera()->bloquer();
		}
	}

	void remplir() {

	}
};
