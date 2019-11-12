/**
 * @file DistributionLaplace
 * This file contains the functions of the Laplace distribution.
 *
 * @brief Implementation of the Laplace distribution.
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes core development team
 * @license GPL version 3
 * @since 2017-10-19
 *
 * $Id$
 */

#include <cmath>

#include "DistributionLaplace.h"
#include "RandomNumberGenerator.h"

using namespace RevBayesCore;

/*!
 * This function calculates the probability density 
 * for a standard Laplace-distributed random variable.
 *
 * \brief Standard Laplace probability density.
 * \param x is the Laplace random variable. 
 * \return Returns the probability density.
 * \throws Does not throw an error.
 */
double RbStatistics::Laplace::pdf(double x) 
{
	return Laplace::pdf(0, 1, x);
}

/*!
 * This function calculates the probability density 
 * for a Laplace-distributed random variable.
 *
 * \brief Laplace probability density.
 * \param mu is the mean parameter. 
 * \param scale is the variance parameter. 
 * \param x is the normal random variable. 
 * \return Returns the probability density.
 */
double RbStatistics::Laplace::pdf(double mu, double scale, double x) 
{
	double y = fabs(( x - mu ) / scale);
	return exp( -y )  / ( 2 * scale );
}



/*!
 * This function calculates the natural log of the probability density 
 * for a standard Laplace-distributed random variable.
 *
 * \brief Natural log of standard Laplace probability density.
 * \param x is the Laplace random variable. 
 * \return Returns the natural log of the probability density.
 * \throws Does not throw an error.
 */
double RbStatistics::Laplace::lnPdf(double x) 
{
	return log( Laplace::pdf(x) );
}


/*!
 * This function calculates the natural log of the probability density 
 * for a Laplace-distributed random variable.
 *
 * \brief Natural log of Laplace probability density.
 * \param mu is the mean parameter. 
 * \param scale is the scale parameters. 
 * \param x is the Laplce random variable. 
 * \return Returns the natural log of the probability density.
 * \throws Does not throw an error.
 */
double RbStatistics::Laplace::lnPdf(double mu, double scale, double x) 
{
    return log( Laplace::pdf(mu, scale, x) );
}



/*!
 * This function calculates the cumulative probability 
 * for a standard Laplace-distributed random variable.
 *
 * \brief Standard Laplace cumulative probability.
 * \param x is the Laplace random variable. 
 * \return Returns the cumulative probability.
 * \throws Does not throw an error.
 */
double RbStatistics::Laplace::cdf(double x) 
{
	return Laplace::cdf(0, 1, x);
}

/*!
 * This function calculates the cumulative probability 
 * for a normally-distributed random variable.
 *
 * \brief Laplace cumulative probability.
 * \param mu is the mean parameter of the normal. 
 * \param scale is the variance parameter of the normal. 
 * \param x is the normal random variable. 
 * \return Returns the cumulative probability.
 * \throws Does not throw an error.
 */
double RbStatistics::Laplace::cdf(double mu, double scale, double x) 
{
    double z = (x - mu);

    if (z < 0)
    {
        z = -1;
    }
    else if (z == 0)
    {
        z = 0;
    }
    else
    {
        z = 1;
    }

	double y = fabs(x - mu) / scale;

    return 0.5 + 0.5 * z * (1 - exp( -y ));
}


double RbStatistics::Laplace::quantile(double p) 
{
    return Laplace::quantile(0, 1, p);
}


double RbStatistics::Laplace::quantile(double mu, double scale, double p) 
{
    if (p < 0.5)
    {
        return mu + scale * log(2 * p);
    }
    else
    {
        return mu - scale * log(2.0 - 2 * p);
    }
}



double RbStatistics::Laplace::rv(RandomNumberGenerator& rng) 
{
	return Laplace::rv(0 ,1, rng);
}



double RbStatistics::Laplace::rv(double mu, double scale, RandomNumberGenerator& rng) 
{
    double u = rng.uniform01() - 0.5;
    double z = u;

    if (z < 0)
    {
        z = -1;
    }
    else if (z == 0)
    {
        z = 0;
    }
    else
    {
        z = 1;
    }

    return mu - scale * z * log( 1 - 2 * fabs(u) );
}


