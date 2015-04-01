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
		Vecteur3d point;
		double* tabVertices;
		Plan plan;

		gfx::Modele& modele = *modele3D.obtModele();

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
			normale = { modele.obtNormales()[Nbrface * 3], modele.obtNormales()[Nbrface * 3 + 1], modele.obtNormales()[Nbrface * 3 + 2]};

			if (plan.insertionDroitePlan(rayonCollision, pointCollision)) {

				point = pointCollision + rayonCollision.obtenirVecteurDirecteur();

				if (pointDansFace(point1, point2, point3, pointCollision, normale)) {
					if (memeCote(point, rayonCollision.obtenirPoint(), pointCollision, point1)) {

						double angle = normale.angleEntreVecteurs(rayonCollision.obtenirVecteurDirecteur()) * (180 / M_PI);
						if (angle > 120 && angle < 260)
							return true;
					}
				}
			}
		}
		return false;
	}

	bool memeCote(Vecteur3d point1, Vecteur3d point2, Vecteur3d droite1, Vecteur3d droite2) {

		Vecteur3d produit1 = (droite2 - droite1).produitVectoriel(point1 - droite1);
		Vecteur3d produit2 = (droite2 - droite1).produitVectoriel(point2 - droite1);
		if (produit1.produitScalaire(produit2) >= 0)
			return true;
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

	void RebondDeFou(Objet& objet, Vecteur3d vecteurNormal, Vecteur3d pointdecollision) {

		Vecteur3d pointDepart = objet.obtModele3D().obtBoiteDeCollisionModifiee()[4];
		pointdecollision = pointDepart;
		Vecteur3d positionCM = { pointDepart.x + objet.obtModele3D().obtBoiteDeCollisionModifiee()[5].x,
			pointDepart.y + objet.obtModele3D().obtBoiteDeCollisionModifiee()[7].y,
			pointDepart.z + objet.obtModele3D().obtBoiteDeCollisionModifiee()[0].z };

		Vecteur3d vitesseAngulaireFinale = { 0, 0, 0 };
		Vecteur3d rayon = { positionCM.x - pointdecollision.x, positionCM.y - pointdecollision.y, positionCM.z - pointdecollision.z };

		double vitesseFinale;
		double c = 1 - mapRestitution[objet.obtMateriaux()];
		double r = rayon.norme();
		double theta = vecteurNormal.angleEntreVecteurs(positionCM);
		double a = Physique::obtInstance().distanceEntreDeuxPoints(objet.obtModele3D().obtBoiteDeCollisionModifiee()[4], objet.obtModele3D().obtBoiteDeCollisionModifiee()[5]);
		double b = Physique::obtInstance().distanceEntreDeuxPoints(objet.obtModele3D().obtBoiteDeCollisionModifiee()[4], objet.obtModele3D().obtBoiteDeCollisionModifiee()[0]);
		double masse = objet.obtMasse();
		double vi = objet.obtVitesse().norme();
		double I = (objet.obtMasse() * (pow(a, 2) + pow(b, 2))) / 12;


		vitesseFinale = vi*(((masse*pow(r, 2)*pow(sin(theta), 2)) + (sqrt(I*(pow(sin(theta), 2))*masse*pow(r, 2)*(pow(c, 2) - 1) + (pow(c, 2)*I))))) / ((masse*pow(r, 2) * pow(sin(theta), 2)) + (I));

		vitesseAngulaireFinale.z = (rayon.x * objet.obtMasse() * (vitesseFinale - objet.obtVitesse().norme()) * sin(theta)) / I;
		//vitesseAngulaireFinale.x = (rayon.y * objet.obtMasse() * (vitesseFinale - objet.obtVitesse().norme()) * sin(theta)) / I;
		vitesseAngulaireFinale.x = (rayon.z * objet.obtMasse() * (vitesseFinale - objet.obtVitesse().norme()) * sin(theta)) / I;
		objet.defVitesseAngulaire(vitesseAngulaireFinale);
		objet.obtVitesse().normaliser();
		objet.obtVitesse() *= vitesseFinale;
		RebondObjetCarte(objet, vecteurNormal);
	}

	void RebondDeFou2(Objet& objet, Vecteur3d vecteurNormal, Vecteur3d pointdecollision) {

		Vecteur3d pointDepart = objet.obtModele3D().obtBoiteDeCollisionModifiee()[4];

		double c = 2 - mapRestitution[objet.obtMateriaux()];
		double masse = objet.obtMasse();
		Vecteur3d vitesseAngulaireFinale = { 0, 0, 0 };

		// plan xy:
		{
			Vecteur2d positionCM = { pointDepart.x + (objet.obtModele3D().obtBoiteDeCollisionModifiee()[5].x / 2),
				pointDepart.y + (objet.obtModele3D().obtBoiteDeCollisionModifiee()[7].y / 2) };

			Vecteur2d vecteurNormaleXY = { vecteurNormal.x, vecteurNormal.y };

			if (vecteurNormaleXY.norme() != 0) {

				Vecteur2d vitInitiale = { objet.obtVitesse().x, objet.obtVitesse().y };

				double vitesseFinaleXY;

				double rayon = distanceEntreDeuxPoints2d(positionCM, { pointdecollision.x, pointdecollision.y });
				double theta = vecteurNormaleXY.angleEntreVecteurs(positionCM);

				Vecteur2d point4 = { objet.obtModele3D().obtBoiteDeCollisionModifiee()[4].x, objet.obtModele3D().obtBoiteDeCollisionModifiee()[4].y };
				Vecteur2d point5 = { objet.obtModele3D().obtBoiteDeCollisionModifiee()[5].x, objet.obtModele3D().obtBoiteDeCollisionModifiee()[5].y };
				Vecteur2d point7 = { objet.obtModele3D().obtBoiteDeCollisionModifiee()[7].x, objet.obtModele3D().obtBoiteDeCollisionModifiee()[7].y };

				double a = distanceEntreDeuxPoints2d(point4, point5);
				double b = distanceEntreDeuxPoints2d(point4, point7);

				double I = (objet.obtMasse() * (pow(a, 2) + pow(b, 2))) / 12;
				double vi = vitInitiale.norme();

				if (((pointdecollision.x - positionCM.x <= 0) && (pointdecollision.y - positionCM.y <= 0)) || ((pointdecollision.x - positionCM.x >= 0) && (pointdecollision.y - positionCM.y >= 0)))
					vitesseFinaleXY = ((vi) / ((masse*pow(rayon, 2)*pow(sin(theta), 2)) + (I)))*((masse*pow(rayon, 2)*pow(sin(theta), 2)) - (sqrt(I*(((masse*pow(rayon, 2)*pow(sin(theta), 2))*(pow(c, 2) - 1)) + (pow(c, 2)*I)))));
				else
					vitesseFinaleXY = ((vi) / ((masse*pow(rayon, 2)*pow(sin(theta), 2)) + (I)))*((masse*pow(rayon, 2)*pow(sin(theta), 2)) + (sqrt(I*(((masse*pow(rayon, 2)*pow(sin(theta), 2))*(pow(c, 2) - 1)) + (pow(c, 2)*I)))));

				vitesseAngulaireFinale.z = (rayon * objet.obtMasse() * (vitesseFinaleXY - vi) * sin(theta)) / I;
			}
		}
		// plan xz:

		{
			Vecteur2d positionCM = { pointDepart.x + (objet.obtModele3D().obtBoiteDeCollisionModifiee()[5].x / 2),
				pointDepart.z + (objet.obtModele3D().obtBoiteDeCollisionModifiee()[0].z / 2) };

			Vecteur2d vecteurNormaleXZ = { vecteurNormal.x, vecteurNormal.z };

			if (vecteurNormaleXZ.norme() != 0) {

				Vecteur2d vitInitiale = { objet.obtVitesse().x, objet.obtVitesse().z };

				double vitesseFinaleXZ;

				double rayon = distanceEntreDeuxPoints2d(positionCM, { pointdecollision.x, pointdecollision.z });
				double theta = vecteurNormaleXZ.angleEntreVecteurs(positionCM);

				Vecteur2d point4 = { objet.obtModele3D().obtBoiteDeCollisionModifiee()[4].x, objet.obtModele3D().obtBoiteDeCollisionModifiee()[4].y };
				Vecteur2d point5 = { objet.obtModele3D().obtBoiteDeCollisionModifiee()[5].x, objet.obtModele3D().obtBoiteDeCollisionModifiee()[5].y };
				Vecteur2d point0 = { objet.obtModele3D().obtBoiteDeCollisionModifiee()[0].x, objet.obtModele3D().obtBoiteDeCollisionModifiee()[0].y };

				double a = distanceEntreDeuxPoints2d(point4, point5);
				double b = distanceEntreDeuxPoints2d(point4, point0);

				double I = (objet.obtMasse() * (pow(a, 2) + pow(b, 2))) / 12;
				double vi = vitInitiale.norme();

				if (((pointdecollision.z - positionCM.y <= 0) && (pointdecollision.x - positionCM.x <= 0)) || ((pointdecollision.z - positionCM.y >= 0) && (pointdecollision.x - positionCM.x >= 0)))
					vitesseFinaleXZ = ((vi) / ((masse*pow(rayon, 2)*pow(sin(theta), 2)) + (I)))*((masse*pow(rayon, 2)*pow(sin(theta), 2)) - (sqrt(I*(((masse*pow(rayon, 2)*pow(sin(theta), 2))*(pow(c, 2) - 1)) + (pow(c, 2)*I)))));
				else
					vitesseFinaleXZ = ((vi) / ((masse*pow(rayon, 2)*pow(sin(theta), 2)) + (I)))*((masse*pow(rayon, 2)*pow(sin(theta), 2)) + (sqrt(I*(((masse*pow(rayon, 2)*pow(sin(theta), 2))*(pow(c, 2) - 1)) + (pow(c, 2)*I)))));

				vitesseAngulaireFinale.y = (rayon * objet.obtMasse() * (vitesseFinaleXZ - vi) * sin(theta)) / I;
			}
		}
		// plan yz:

		{
			Vecteur2d positionCM = { pointDepart.y + (objet.obtModele3D().obtBoiteDeCollisionModifiee()[7].y / 2),
				pointDepart.z + (objet.obtModele3D().obtBoiteDeCollisionModifiee()[0].z / 2) };

			Vecteur2d vecteurNormaleYZ = { vecteurNormal.y, vecteurNormal.z };

			if (vecteurNormaleYZ.norme() != 0) {

				Vecteur2d vitInitiale = { objet.obtVitesse().y, objet.obtVitesse().z };

				double vitesseFinaleYZ;

				double rayon = distanceEntreDeuxPoints2d(positionCM, { pointdecollision.y, pointdecollision.z });
				double theta = vecteurNormaleYZ.angleEntreVecteurs(positionCM);

				Vecteur2d point4 = { objet.obtModele3D().obtBoiteDeCollisionModifiee()[4].x, objet.obtModele3D().obtBoiteDeCollisionModifiee()[4].y };
				Vecteur2d point7 = { objet.obtModele3D().obtBoiteDeCollisionModifiee()[7].x, objet.obtModele3D().obtBoiteDeCollisionModifiee()[7].y };
				Vecteur2d point0 = { objet.obtModele3D().obtBoiteDeCollisionModifiee()[0].x, objet.obtModele3D().obtBoiteDeCollisionModifiee()[0].y };

				double a = distanceEntreDeuxPoints2d(point4, point7);
				double b = distanceEntreDeuxPoints2d(point4, point0);

				double I = (objet.obtMasse() * (pow(a, 2) + pow(b, 2))) / 12;
				double vi = vitInitiale.norme();

				if (((pointdecollision.z - positionCM.y <= 0) && (pointdecollision.y - positionCM.x <= 0)) || ((pointdecollision.z - positionCM.y >= 0) && (pointdecollision.y - positionCM.x >= 0)))
					vitesseFinaleYZ = ((vi) / ((masse*pow(rayon, 2)*pow(sin(theta), 2)) + (I)))*((masse*pow(rayon, 2)*pow(sin(theta), 2)) - (sqrt(I*(((masse*pow(rayon, 2)*pow(sin(theta), 2))*(pow(c, 2) - 1)) + (pow(c, 2)*I)))));
				else
					vitesseFinaleYZ = ((vi) / ((masse*pow(rayon, 2)*pow(sin(theta), 2)) + (I)))*((masse*pow(rayon, 2)*pow(sin(theta), 2)) + (sqrt(I*(((masse*pow(rayon, 2)*pow(sin(theta), 2))*(pow(c, 2) - 1)) + (pow(c, 2)*I)))));

				vitesseAngulaireFinale.x = (rayon * objet.obtMasse() * (vitesseFinaleYZ - vi) * sin(theta)) / I;
			}
		}

		objet.defVitesseAngulaire(vitesseAngulaireFinale);
		RebondObjetCarte(objet, vecteurNormal);
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

		double* tableaunormale = objet.obtModele3D().obtNormalesModifies(); //� MODIFIER LORSQUE LES MODIF DE normale FONCTIONNENT!
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

	double distanceEntreDeuxPoints2d(Vecteur2d point1, Vecteur2d point2) {
		return SDL_sqrt(SDL_pow((point2.x - point1.x), 2) + SDL_pow((point2.y - point1.y), 2));
	}

	bool pointDansFace(Vecteur3d& point1, Vecteur3d& point2, Vecteur3d& point3, Vecteur3d& point, Vecteur3d normale) {



		Vecteur3d v0 = point3 - point1;
		Vecteur3d v1 = point2 - point1;
		Vecteur3d v2 = point - point1;

		Vecteur2d vect0;
		Vecteur2d vect1;
		Vecteur2d vect2;

		normale.normaliser();

		if (abs(normale.x) > abs(normale.y) && abs(normale.x) > abs(normale.z)) {

			vect0 = Vecteur2d(v0.y, v0.z);
			vect1 = Vecteur2d(v1.y, v1.z);
			vect2 = Vecteur2d(v2.y, v2.z);
		}

		if (abs(normale.y) > abs(normale.x) && abs(normale.y) > abs(normale.z)) {

			vect0 = Vecteur2d(v0.x, v0.z);
			vect1 = Vecteur2d(v1.x, v1.z);
			vect2 = Vecteur2d(v2.x, v2.z);
		}
		if (abs(normale.z) > abs(normale.x) && abs(normale.z) > abs(normale.y)) {

			vect0 = Vecteur2d(v0.x, v0.y);
			vect1 = Vecteur2d(v1.x, v1.y);
			vect2 = Vecteur2d(v2.x, v2.y);
		}

		double produit00 = vect0 * vect0;
		double produit01 = vect0 * vect1;
		double produit02 = vect0 * vect2;
		double produit11 = vect1 * vect1;
		double produit12 = vect1 * vect2;

		double invDenom = 1 / (produit00 * produit11 - produit01 * produit01);
		double u = (produit11 * produit02 - produit01 * produit12) * invDenom;
		double v = (produit00 * produit12 - produit12 * produit02) * invDenom;

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
		Vecteur3d difference;
		Vecteur3d* tabObjet = objet.obtModele3D().obtBoiteDeCollisionModifiee();


		for (int i = 0; i < 8; i++) {

			point = tabObjet[i];

			rayonCollision = Droite(point, objet.obtVitesse());

			if (collisionDroiteModele(salle.obtModele(), rayonCollision, pointCollision, normale)) {
				
				difference = pointCollision - point;
				objet.defPosition(objet.obtPosition() + difference);

				normale.normaliser();
				RebondObjetCarte(objet, normale);
				return true;
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
