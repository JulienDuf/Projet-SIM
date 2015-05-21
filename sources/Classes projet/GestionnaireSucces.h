#pragma once
#include "Succes.h"
#include <list>
#include "Chrono.h"
#include "Singleton.h"
#include "Vecteur3.h"
#include "Texte.h"
#include "Texte2D.h"
#include "Police.h"
class GestionnaireSucces: public Singleton<GestionnaireSucces>{
private:
	std::list<Succes*> listeSucces;
	bool chronoDeclenche;
	Chrono* chronoSanteMentale;
	Chrono chronoAffichage;
	Chrono chronoOuiNon;
	Item* itemOuiNon;
	gfx::Sprite2D* boite;
	gfx::Texte2D* titre;
	gfx::Texte2D* description;
	unsigned short nbrItems;
public:
	void initialiser(){
		titre = nullptr;
		description = nullptr;
		boite = new gfx::Sprite2D(Vecteur2f(900, 540), new gfx::Texture("Ressources/Texture/achievement.png"));
		//Succ�s fonctionnels...
		listeSucces.push_back(new Succes(new gfx::Texte2D(new std::string("Wait, what?"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 25), Vecteur2f(905, 640)), new gfx::Texte2D(new std::string("Lose sanity!"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 19), Vecteur2f(905,575)), 0));
		listeSucces.push_back(new Succes(new gfx::Texte2D(new std::string("It's dangerous to go alone! Take this"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 20), Vecteur2f(905, 640)), new gfx::Texte2D(new std::string("Pick up an item"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 17), Vecteur2f(905, 575)), 1));
		listeSucces.push_back(new Succes(new gfx::Texte2D(new std::string("We've got a long way to go..."), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 25), Vecteur2f(905, 640)), new gfx::Texte2D(new std::string("Start a new game"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 20), Vecteur2f(905, 575)), 36));//2
		listeSucces.push_back(new Succes(new gfx::Texte2D(new std::string("Am I even real?"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 25), Vecteur2f(905, 640)), new gfx::Texte2D(new std::string("Stay under 25% mental state for 2 minutes"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 17), Vecteur2f(905, 575)), 3));
		listeSucces.push_back(new Succes(new gfx::Texte2D(new std::string("I should buy a boat!"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 25), Vecteur2f(905, 640)), new gfx::Texte2D(new std::string("Stay under 25% mental state for 3 minutes"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 17), Vecteur2f(905, 575)), 4));
		listeSucces.push_back(new Succes(new gfx::Texte2D(new std::string("Crazy? I'm not crazy, you're crazy!"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 20), Vecteur2f(905, 640)), new gfx::Texte2D(new std::string("Stay under 25% mental state for 4 minutes"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 17), Vecteur2f(905, 575)), 5));
		listeSucces.push_back(new Succes(new gfx::Texte2D(new std::string("Madness? THIS IS COUCOUNE!"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 23), Vecteur2f(905, 640)), new gfx::Texte2D(new std::string("Stay under 25% mental state for 5 minutes"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 17), Vecteur2f(905, 575)), 6));
		listeSucces.push_back(new Succes(new gfx::Texte2D(new std::string("Say Hello to my little friend!"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 25), Vecteur2f(905, 640)), new gfx::Texte2D(new std::string("Find a firearm."), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 20), Vecteur2f(905, 575)), 8));
		listeSucces.push_back(new Succes(new gfx::Texte2D(new std::string("Here be the holy hand grenade"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 24), Vecteur2f(905, 640)), new gfx::Texte2D(new std::string("Find a grenade"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 20), Vecteur2f(905, 575)), 9));
		listeSucces.push_back(new Succes(new gfx::Texte2D(new std::string("Oh baby a triple!"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 25), Vecteur2f(905, 640)), new gfx::Texte2D(new std::string("Have three items in your inventory."), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 20), Vecteur2f(905, 575)), 10));
		listeSucces.push_back(new Succes(new gfx::Texte2D(new std::string("Hope this doesn't go like Slender..."), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 23), Vecteur2f(905, 640)), new gfx::Texte2D(new std::string("Find a note"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 20), Vecteur2f(905, 575)), 13));
		listeSucces.push_back(new Succes(new gfx::Texte2D(new std::string("\"Oui Non Non Oui?\""), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 25), Vecteur2f(905, 640)), new gfx::Texte2D(new std::string("Drop an item then pick it up again"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 20), Vecteur2f(905, 575)), 16));
		listeSucces.push_back(new Succes(new gfx::Texte2D(new std::string("And thou shalt use the Holy Rod!"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 23), Vecteur2f(905, 640)), new gfx::Texte2D(new std::string("Find the divine object"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 20), Vecteur2f(905, 575)), 17));
		listeSucces.push_back(new Succes(new gfx::Texte2D(new std::string("H20, too?"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 25), Vecteur2f(905, 640)), new gfx::Texte2D(new std::string("Find some water"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 20), Vecteur2f(905, 575)), 18));
		//Succ�s fonctionnels mais pas encore v�rifiables dans le code...
		listeSucces.push_back(new Succes(new gfx::Texte2D(new std::string("I'm Batman!"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 25), Vecteur2f(905, 640)), new gfx::Texte2D(new std::string("Find Batman"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 20), Vecteur2f(905, 575)), 7));
		listeSucces.push_back(new Succes(new gfx::Texte2D(new std::string("We shall engage in a jolly cooperation!"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 20), Vecteur2f(905, 640)), new gfx::Texte2D(new std::string("Find the companion"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 20), Vecteur2f(905, 575)), 11));
		listeSucces.push_back(new Succes(new gfx::Texte2D(new std::string("Praise the sun!"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 25), Vecteur2f(905, 640)), new gfx::Texte2D(new std::string("Get outside"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 20), Vecteur2f(905, 575)), 12));
		listeSucces.push_back(new Succes(new gfx::Texte2D(new std::string("Death by HCN"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 25), Vecteur2f(905, 640)), new gfx::Texte2D(new std::string("Get killed by the toxic gas"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 20), Vecteur2f(905, 575)), 14));
		listeSucces.push_back(new Succes(new gfx::Texte2D(new std::string("Restart?"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 25), Vecteur2f(905, 640)), new gfx::Texte2D(new std::string("You have to restart the game!"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 20), Vecteur2f(905, 575)), 15));
		listeSucces.push_back(new Succes(new gfx::Texte2D(new std::string("Out of Mein Kampfort zone"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 25), Vecteur2f(905, 640)), new gfx::Texte2D(new std::string("Finish the game in easy or medium"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 20), Vecteur2f(905, 575)), 19));
		listeSucces.push_back(new Succes(new gfx::Texte2D(new std::string("Holocaust 2, the return"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 25), Vecteur2f(905, 640)), new gfx::Texte2D(new std::string("Finish the game in hardcore"), { 0, 0, 0, 255 }, gfx::GestionnaireRessources::obtInstance().obtPolice("Ressources/Font/arial.ttf", 20), Vecteur2f(905, 575)), 20));




		chargerFichier("Ressources/Info/sauvegarde_succes.txt");
		chronoSanteMentale = new Chrono();
		chronoDeclenche = false;
		
		nbrItems = 1;

		chargerFichier("Ressources/Info/sauvegarde_succes.txt");
	}
	void reinitialiserSauvegarde(){
		std::ofstream fichier;
		fichier.open("Ressources/Info/sauvegarde_succes.txt",std::ofstream::out | std::ofstream::trunc);
		fichier.close();
	}
	void reinitialiserListe(){
		for (auto it : listeSucces){
			it->defAccompli(false);
		}
	}
	void chargerFichier(const char* nomFichier){
		std::ifstream fichier;
		fichier.open(nomFichier);
		unsigned short ligne;
		while (!fichier.eof()){
			fichier >> ligne;
			for (auto it : listeSucces){
				if (it->obtID() == ligne){
					it->defAccompli(true);
				}
			}
		}
		fichier.close();
	}
	void afficherSucces(Succes* succes){
		if (titre != nullptr){
			gfx::Gestionnaire2D::obtInstance().retObjet(titre);
		}
		titre = succes->obtTitre();
		if (titre != nullptr){
			gfx::Gestionnaire2D::obtInstance().retObjet(description);
		}
		description = succes->obtDescription();
		gfx::Gestionnaire2D::obtInstance().retObjet(boite);
		gfx::Gestionnaire2D::obtInstance().ajouterObjet(boite);
		gfx::Gestionnaire2D::obtInstance().ajouterObjet(succes->obtTitre());
		gfx::Gestionnaire2D::obtInstance().ajouterObjet(succes->obtDescription());
		chronoAffichage.repartir();
	}
	void verifierTempsAffichage(){
		if (chronoAffichage.obtTempsEcoule().enMillisecondes() > 5000.){
			gfx::Gestionnaire2D::obtInstance().retObjet(boite);
			gfx::Gestionnaire2D::obtInstance().retObjet(titre);
			gfx::Gestionnaire2D::obtInstance().retObjet(description);
			titre = nullptr;
			description = nullptr;
		}
	}
	void sauvegarder(){
		reinitialiserSauvegarde();
		std::ofstream fichier("Ressources/Info/sauvegarde_succes.txt",std::ios::app);
		for (auto it : listeSucces){
			if (it->obtAccompli()){
				fichier << std::to_string(it->obtID());
				fichier << std::endl;
			}
		}
	}
	void obtSucces(unsigned short IDSucces){
		for (auto it : listeSucces){
			if (!it->obtAccompli() && it->obtID() == IDSucces){
				it->defAccompli(true);
				afficherSucces(it);
				break;
			}
		}
	}

	Chrono* obtChronoSanteMentale(){
		return chronoSanteMentale;
	}

	bool obtChronoDeclenche(){
		return chronoDeclenche;
	}

	void defChronoDeclenche(){
		chronoDeclenche = !chronoDeclenche;
	}

	void defNbrItems(unsigned short nbrItems){
		this->nbrItems = nbrItems;
	}
	unsigned short obtNbrItems(){
		return nbrItems;
	}

	void verifierOuiNon(Item* itemRamasse){
		if (itemOuiNon != nullptr)
			if (itemRamasse == itemOuiNon && chronoOuiNon.obtTempsEcoule().enSecondes() < 3.)
				obtSucces(16);
	}
	void defItemOuiNonLache(Item* itemOuiNon){
		this->itemOuiNon = itemOuiNon;
		chronoOuiNon.repartir();
	}

	void tuerGestionnaireSucces(){
		sauvegarder();
		for (auto it : listeSucces){
			delete it;
		}
		delete chronoSanteMentale;
		if (itemOuiNon != nullptr)
			delete itemOuiNon;
		if (boite != nullptr)
			delete boite;
	}
};