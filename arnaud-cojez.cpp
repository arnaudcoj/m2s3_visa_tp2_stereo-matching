/* --------------------------------------------------------------------------
Mise en correspondance de points d'interet detectes dans deux images
Copyright (C) 2010, 2011  Universite Lille 1

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------
Inclure les fichiers d'entete
-------------------------------------------------------------------------- */
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;
#include "glue.hpp"
#include "arnaud-cojez.hpp"

// -----------------------------------------------------------------------
/// \brief Detecte les coins.
///
/// @param mImage: pointeur vers la structure image openCV
/// @param iMaxCorners: nombre maximum de coins detectes
/// @return matrice des coins
// -----------------------------------------------------------------------
Mat iviDetectCorners(const Mat& mImage,
                     int iMaxCorners) {
    vector<Point2f> vCorners;

    goodFeaturesToTrack(mImage, vCorners, iMaxCorners, 0.01, 10);
    Mat mCorners(3, int(vCorners.size()), CV_64F);

    for (unsigned int i = 0; i < vCorners.size(); i++) {
      mCorners.at<double>(0,i) = (double)vCorners[i].x;
      mCorners.at<double>(1,i) = (double)vCorners[i].y;
      mCorners.at<double>(2,i) = 1.;
    }
    // Retour de la matrice
    return mCorners;
}

// -----------------------------------------------------------------------
/// \brief Initialise une matrice de produit vectoriel.
///
/// @param v: vecteur colonne (3 coordonnees)
/// @return matrice de produit vectoriel
// -----------------------------------------------------------------------
Mat iviVectorProductMatrix(const Mat& v) {
    double vx = v.at<double>(0);
    double vy = v.at<double>(1);
    double vz = v.at<double>(2);

    Mat mVectorProduct = (Mat_<double>(3, 3) <<
       0., -vz,  vy,
       vz,  0., -vx,
      -vy,  vx,  0.
    );

    // Retour de la matrice
    return mVectorProduct;
}

// -----------------------------------------------------------------------
/// \brief Initialise et calcule la matrice fondamentale.
///
/// @param mLeftIntrinsic: matrice intrinseque de la camera gauche
/// @param mLeftExtrinsic: matrice extrinseque de la camera gauche
/// @param mRightIntrinsic: matrice intrinseque de la camera droite
/// @param mRightExtrinsic: matrice extrinseque de la camera droite
/// @return matrice fondamentale
// -----------------------------------------------------------------------
Mat iviFundamentalMatrix(const Mat& mLeftIntrinsic,
                         const Mat& mLeftExtrinsic,
                         const Mat& mRightIntrinsic,
                         const Mat& mRightExtrinsic) {

    Mat reduc = (Mat_<double>(3,4) <<
       1.0, 0.0, 0.0, 0.0,
       0.0, 1.0, 0.0, 0.0,
       0.0, 0.0, 1.0, 0.0
       );
    Mat pLeft = mLeftIntrinsic * reduc * mLeftExtrinsic;
    Mat pRight = mRightIntrinsic * reduc * mRightExtrinsic;

    Mat invPLeft = pLeft.inv(DECOMP_SVD);

    Mat h = pRight * invPLeft;

    Mat mInvLeftExtrinsic = mLeftExtrinsic.inv();

    Mat o1 = mInvLeftExtrinsic.col(3);


    // Doit utiliser la fonction iviVectorProductMatrix
    Mat p201 = pRight * o1;
    Mat p201x = iviVectorProductMatrix(p201);
    Mat mFundamental =  p201x * pRight * invPLeft;
    // Retour de la matrice fondamentale
    return mFundamental;
}

// -----------------------------------------------------------------------
/// \brief Initialise et calcule la matrice des distances entres les
/// points de paires candidates a la correspondance.
///
/// @param mLeftCorners: liste des points 2D image gauche
/// @param mRightCorners: liste des points 2D image droite
/// @param mFundamental: matrice fondamentale
/// @return matrice des distances entre points des paires
// -----------------------------------------------------------------------
Mat iviDistancesMatrix(const Mat& m2DLeftCorners,
                       const Mat& m2DRightCorners,
                       const Mat& mFundamental) {
    // A modifier !

    Mat mDistances = Mat(m2DLeftCorners.cols, m2DRightCorners.cols, CV_64F);

        for(int i = 0; i < m2DLeftCorners.cols; i++) {
          for(int j = 0; j < m2DRightCorners.cols; j++) {
              Mat mLeft = m2DLeftCorners.col(i);
              Mat mRight = m2DLeftCorners.col(j);
              Mat dLeft = mFundamental * mLeft;
              Mat dRight = mFundamental.t() * mRight;

              double aRight = dRight.at<double>(0,0);
              double bRight = dRight.at<double>(1,0);
              double cRight = dRight.at<double>(2,0);

              double xLeft = mLeft.at<double>(0,0);
              double yLeft = mLeft.at<double>(1,0);

              double aLeft = dLeft.at<double>(0,0);
              double bLeft = dLeft.at<double>(1,0);
              double cLeft = dLeft.at<double>(2,0);

              double xRight = mRight.at<double>(0,0);
              double yRight = mRight.at<double>(1,0);

              double distance1 = fabs(aLeft * xRight + bLeft * yRight + cLeft) / sqrt(aRight * aRight + bRight * bRight);

              double distance2 = fabs(aRight * xLeft + bRight * yLeft + cRight) / sqrt(aLeft * aLeft + bLeft * bLeft);

              mDistances.at<double>(i,j) = distance1 + distance2;
          }
        }
    return mDistances;

}

// -----------------------------------------------------------------------
/// \brief Initialise et calcule les indices des points homologues.
///
/// @param mDistances: matrice des distances
/// @param fMaxDistance: distance maximale autorisant une association
/// @param mRightHomologous: liste des correspondants des points gauche
/// @param mLeftHomologous: liste des correspondants des points droite
/// @return rien
// -----------------------------------------------------------------------
void iviMarkAssociations(const Mat& mDistances,
                         double dMaxDistance,
                         Mat& mRightHomologous,
                         Mat& mLeftHomologous) {
    // A modifier !
}
