#ifndef PLANCK_H
#define PLANCK_H

#include "auto_derivative.h"

// Computes black body function plus derivative using LIDORT get_planckfunction_plus
namespace FullPhysics {

double planck(double wn, double temperature);
AutoDerivative<double> planck(double wn, double temperature, blitz::Array<double, 1>& gradient);

}

#endif
