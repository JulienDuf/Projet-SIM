#pragma once

#include "Singleton.h"
#include "Vecteur3.h"
#include "Chrono.h"
#include "Droite.h"
#include "Plan.h"

class Physique : public Singleton<Physique>{
private:

	double gravite;
	double constanteDeFriction;
	double sensibiliteMagnetique;
	double frametime;
	std::map<char*, double> mapRestitution;
	Chrono temps;

	bool collisionDroiteModele(gfx::Modele3D& modele3D, Droite& rayonCollision, Vecteur3d pointCollision, Vecteur3d& normale) {

		Vecteur3d point1;
		Vecteur3d point2;
		Vecteur3d point3;
		double* tabVertices;
		Plan plan;

		gfx::Modele& modele = modele3D.obtModele();

		tabVertices = modele3D.obtSommetsModifies();

		for (unsigned int Nbrface = 0; Nbrface < modele.obtNbrSommets(); Nbrface += 3) {

			for (int j = 0; j < 3; j++) {
				switch (j) {

				case 0:
					point1 = { tabVertices[(Nbrface)* 3], tabVertices[(Nbrface)* 3 + 1], tabVertices[(Nbrface)* 3 + 2] };
					break;

				case 1:
					point2 = { tabVertices[(Nbrface)* 3 + 3], tabVertices[(Nbrface)* 3 + 4], tabVertices[(Nbrface)* 3 + 5] };
					break;

				case 2:
					point3 = { tabVertices[(Nbrface)* 3 + 6], tabVertices[(Nbrface)* 3 + 7], tabVertices[(Nbrface)* 3 + 8] };
					break;
				}
			}

			plan.calculerPlan(point1, point2, point3);
			normale = { modele.obtnormale()[Nbrface * 3], modele.obtnormale()[Nbrface * 3 + 1], modele.obtnormale()[Nbrface * 3 + 2]};

			if (plan.insertionDroitePlan(rayonCollision, pointCollision)) {

				point = pointCollision.top() + rayonCollision.obtenirVecteurDirecteur();

				if (pointDansFace(point1, point2, point3, pointCollision.top(), normale.top())) {
					if (memeCote(point, rayonCollision.obtenirPoint(), pointCollision.top(), point1)) {

						double angle = normale.angleEntreVecteurs(rayonCollision.obtenirVecteurDirecteur()) * (180 / M_PI);
						if (angle > 120 && angle < 260)
							return true;
					}
				}
			}
		}
		return false;
	}

public:

	Physique() {
		frametime = temps.repartir().enSecondes();
		constanteDeFriction = 0.5;
		gravite = -9.8;
		sensibiliteMagnetique = 0.0072;

		// Ajout des coefficients de restitution des diff�rents mat�riaux
		mapRestitution["metal"] = 0.9;
		mapRestitution["bois"] = 0.5;
		mapRestitution["plastic"] = 0.68;
		mapRestitution["ballerebondissante"] = 0.1;
	}

	void repartirTemps() {
		frametime = temps.repartir().enSecondes();
	}

	
	void RebondObjetCarte(Objet& objet, Vecteur3d vecteurNormal) {

		double dScalaire = (2 - mapRestitution[objet.obtMateriaux()]) * objet.obtVitesse().produitScalaire(vecteurNormal);
		objet.obtVitesse() -= vecteurNormal * dScalaire;
	}

	void RebondObjetObjet(Objet& objet1, Objet& objet2, Vecteur3d vecteurNormal) {

		Vecteur3d vVitesseRelative = objet1.obtVitesse() - objet2.obtVitesse();

		double e = (mapRestitution[objet1.obtMateriaux()] + mapRestitution[objet2.obtMateriaux()]) / 2; // Moyenne des coefficients de restitution

		double j = (-(1 + e) * (vVitesseRelative.produitScalaire(vecteurNormal))) / ((vecteurNormal.produitScalaire(vecteurNormal)) * (1 / objet1.obtMasse() + 1 / objet2.obtMasse()));

		Vecteur3d vecteurNormal2 = vecteurNormal;

		vecteurNormal *= j / objet1.obtMasse();
		objet1.obtVitesse() += vecteurNormal;

		vecteurNormal2 *= j / objet2.obtMasse();
		objet2.obtVitesse() -= vecteurNormal2;
	}
	
	double obtenirForceNormale(double masse, Vecteur3d& vitesse, Vecteur3d normale) {

		return masse * gravite * SDL_cos(vitesse.angleEntreVecteurs(normale) - 90);
	}

	void appliquerGravite(Vecteur3d &vecteurVitesseObjet) {
		vecteurVitesseObjet.y += gravite * frametime;
	}

	void appliquerVent(Vecteur3d vecteurVitesseVent, Objet& objet) {

		double* tableaunormale = objet.obtModele3D().obtnormaleModifies(); //� MODIFIER LORSQUE LES MODIF DE normale FONCTIONNENT!
		double* tableauVertices = objet.obtModele3D().obtSommetsModifies();

		double accelerationSelonForceVent = 0.5 * 1.204 * pow(vecteurVitesseVent.norme(), 2);

		double coefficientTrainer = 0;
		double surface = 0;

		double nombreVertice = objet.obtModele3D().obtModele()->obtNbrVertices();

		unsigned int nombreFaceSousPression = 0;

		// Boucle sur toutes les faces de l'objet
		for (unsigned int parcoursFace = 0; parcoursFace < nombreVertice;) {

			// Vecteur normal de la face selon une moyenne de ceux des vertices.
			Vecteur3d vecteurNormale = { (tableaunormale[parcoursFace] + tableaunormale[parcoursFace + 3] + tableaunormale[parcoursFace + 6]) / 3,
				(tableaunormale[parcoursFace + 1] + tableaunormale[parcoursFace + 4] + tableaunormale[parcoursFace + 7]) / 3,
				(tableaunormale[parcoursFace + 2] + tableaunormale[parcoursFace + 5] + tableaunormale[parcoursFace + 8]) / 3 };

			double angleEntreVecteursVentNormale = (vecteurVitesseVent.produitScalaire(vecteurNormale)) / (vecteurVitesseVent.norme() * vecteurNormale.norme());

			// Si l'angle entre le vecteur normal et le vecteur du vent est n�gatif
			if (angleEntreVecteursVentNormale < 0) {

				coefficientTrainer += abs(angleEntreVecteursVentNormale);
				// Calcul de la surface selon la formule d'h�ron: sqrt(s(s-a)(s-b)(s-c))

				// a : Point 1 � point 2;
				double a = sqrt(pow(tableauVertices[parcoursFace] - tableauVertices[parcoursFace + 3], 2) +
					pow(tableauVertices[parcoursFace + 1] - tableauVertices[parcoursFace + 4], 2) +
					pow(tableauVertices[parcoursFace + 2] - tableauVertices[parcoursFace + 5], 2));

				// a : Point 1 � point 3;
				double b = sqrt(pow(tableauVertices[parcoursFace] - tableauVertices[parcoursFace + 6], 2) +
					pow(tableauVertices[parcoursFace + 1] - tableauVertices[parcoursFace + 7], 2) +
					pow(tableauVertices[parcoursFace + 2] - tableauVertices[parcoursFace + 8], 2));

				// a : Point 2 � point 3;
				double c = sqrt(pow(tableauVertices[parcoursFace + 3] - tableauVertices[parcoursFace + 6], 2) +
					pow(tableauVertices[parcoursFace + 4] - tableauVertices[parcoursFace + 7], 2) +
					pow(tableauVertices[parcoursFace + 5] - tableauVertices[parcoursFace + 8], 2));

				// Demi-p�rim�tre;
				double s = 0.5 * (a + b + c);

				surface += sqrt(s*(s - a)*(s - b)*(s - c));

				nombreFaceSousPression++;

			}
			parcoursFace += 9; // Prochaine face(3vertice*xyz)
		}

		coefficientTrainer /= nombreFaceSousPression;

		// Fin du calcul...
		accelerationSelonForceVent *= (coefficientTrainer * surface / objet.obtMasse());

		// Mise en proportion pour l'addition...
		vecteurVitesseVent.normaliser();

		// N�c�ssite l'ajout d'un division par le temps...
		Vecteur3d vecteurVitesseAppliquer = { accelerationSelonForceVent * vecteurVitesseVent.x, accelerationSelonForceVent * vecteurVitesseVent.y, accelerationSelonForceVent * vecteurVitesseVent.z };

		objet.obtVitesse() += vecteurVitesseAppliquer * frametime;
	}
	
	// MANQUE LA NORMALE
	void appliquerFrottement(Objet& objet) {
		//objet.obtVitesse().soustraire(constanteDeFriction * obtenirForceNormale(objet.obtMasse(), objet.obtPosition()));
	}
	
	// Proc�dure qui applique la force d'attraction magn�tique sur un objet
	// (La force du champs et la sensibilit� magn�tique de l'objet sont constant).
	void appliquerMagnetisme(Objet& objet, Vecteur3d positionAimant, double force) {

		double distanceObjetAimant = distanceEntreDeuxPoints(positionAimant, objet.obtPosition());
		double accelerationMagnetique = (6 * sensibiliteMagnetique * force) / (objet.obtMasse() * distanceObjetAimant);

		Vecteur3d vecteurProportionnel = { positionAimant.x - objet.obtPosition().x, positionAimant.y - objet.obtPosition().y, positionAimant.z - objet.obtPosition().z };
		vecteurProportionnel.normaliser();

		Vecteur3d vecteurAcceleration = { accelerationMagnetique, accelerationMagnetique, accelerationMagnetique };

		vecteurAcceleration.produitParUnVecteur(vecteurProportionnel);

		objet.obtVitesse() += vecteurAcceleration * frametime;

	}

	double obtenirAnglePenduleSimple(double angleMaximal, double omega) {
		return angleMaximal * SDL_cos(omega * frametime);
	}

	double distanceEntreDeuxPoints(Vecteur3d& point1, Vecteur3d& point2) {
		return SDL_sqrt(SDL_pow((point2.x - point1.x), 2) + SDL_pow((point2.y - point1.y), 2) + SDL_pow((point2.z - point1.z), 2));
	}

	double pointDansTriangle(Vecteur3d& point1, Vecteur3d& point2, Vecteur3d& point3, Vecteur3d& point) {
		
		Vecteur3d vect1 = point3 - point1;
		Vecteur3d vect2 = point2 - point1;
		Vecteur3d vect3 = point - point1;

		double produit11 = vect1.produitScalaire(vect1);
		double produit12 = vect1.produitScalaire(vect2);
		double produit13 = vect1.produitScalaire(vect3);
		double produit22 = vect2.produitScalaire(vect2);
		double produit23 = vect2.produitScalaire(vect3);

		double invDenom = 1 / (produit11 * produit22 - produit12 * produit12);
		double u = (produit22 * produit13 - produit12 * produit23) * invDenom;
		double v = (produit11 * produit23 - produit23 * produit13) * invDenom;

		return (u >= 0) && (v >= 0) && (u + v < 1);

	}

	double obtenirEnergieCinetique(double masse, Vecteur3d& vecteurVitesseObjet) {
		return 0.5 * masse * SDL_pow(vecteurVitesseObjet.norme(), 2);
	}

	bool collisionObjetSalle(Objet& objet, Salle& salle) {
		Droite rayonCollision;
		Vecteur3d pointCollision;
		Vecteur3d point;
		Vecteur3d normale;
		double distance;
		double d;
		Vecteur3d* tabObjet = objet.obtModele3D().obtBoiteDeCollisionModifiee();


		for (int i = 0; i < 8; i++) {

			point = tabObjet[i];

			rayonCollision = Droite(point, objet.obtVitesse());

			if (collisionDroiteModele(salle.obtModele(), rayonCollision, pointCollision, normale)) {

				if (normale.y > 0 && pointCollision.y > point.y && objet.obtVitesse().y < 0) {
					
					difference = pointCollision - point;
					objet.defPosition(objet.obtPosition() + difference);

					normale.normaliser();
					RebondObjetCarte(objet, normale);
					return true;
				}
			}
		}
		return false;
	}

	bool collisionJoueurSalle(Joueur &joueur, Salle &salle) {
		Droite rayonCollision;
		Vecteur3d pointCollision;
		Vecteur3d point;
		Vecteur3d normale;
		Vecteur3d* tabJoueur = joueur.obtModele3D().obtBoiteDeCollisionModifiee();


		for (int i = 0; i < 8; i++) {

			point = tabJoueur[i];

			rayonCollision = Droite(point, joueur.obtVitesse());

			if (collisionDroiteModele(salle.obtModele(), rayonCollision, pointCollision, normale)) {
				Vecteur3d pointDiference = pointCollision - point;
				joueur.defPosition(joueur.obtPosition() + pointDiference);

				return true;
			}
		}
		return false;
	}

	bool collisionJoueurObjet(Joueur &joueur, Objet &objet) {
		Droite rayonCollision;
		Vecteur3d pointCollision;
		Vecteur3d point;
		Vecteur3d normale;
		Vecteur3d* tabJoueur = joueur.obtModele3D().obtBoiteDeCollisionModifiee();

		for (int i = 0; i < 8; i++) {
			point = tabJoueur[i];

			rayonCollision = Droite(point, joueur.obtVitesse());

			if (collisionDroiteBoiteDeCollision(objet.obtModele3D().obtBoiteDeCollisionModifiee(), rayonCollision, pointCollision, normale)) {
				Vecteur3d pointDiference = pointCollision - point;
				joueur.defPosition(joueur.obtPosition() + pointDiference);

				return true;
			}
		}
		return false;
	}

};
