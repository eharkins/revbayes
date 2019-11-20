/**
 * @file
 * This file contains the implementation of RateMatrix_Vt, which is
 * class that holds a rate matrix in RevBayes.
 *
 * @brief Implementation of RateMatrix_Vt
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date: 2012-12-11 14:46:24 +0100 (Tue, 11 Dec 2012) $
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-08-27, version 1.0
 * @interface Mcmc
 * @package distributions
 *
 * $Id: RateMatrix_Vt.cpp 1921 2012-12-11 13:46:24Z hoehna $
 */

#include "RateMatrix_Vt.h"

#include <stddef.h>
#include <vector>

#include "RbVectorUtilities.h"
#include "Cloneable.h"
#include "MatrixReal.h"
#include "RbVector.h"
#include "RbVectorImpl.h"


using namespace RevBayesCore;

/** Construct rate matrix with n states */
RateMatrix_Vt::RateMatrix_Vt( void ) : RateMatrix_Empirical( 20 ){
    
    MatrixReal &m = *the_rate_matrix;
    
    /* Vt */	  
    m[ 0][ 0] = 0.000000; m[ 0][ 1] = 0.233108; m[ 0][ 2] = 0.199097; m[ 0][ 3] = 0.265145; m[ 0][ 4] = 0.227333; 
	m[ 0][ 5] = 0.310084; m[ 0][ 6] = 0.567957; m[ 0][ 7] = 0.876213; m[ 0][ 8] = 0.078692; m[ 0][ 9] = 0.222972; 
	m[ 0][10] = 0.424630; m[ 0][11] = 0.393245; m[ 0][12] = 0.211550; m[ 0][13] = 0.116646; m[ 0][14] = 0.399143; 
	m[ 0][15] = 1.817198; m[ 0][16] = 0.877877; m[ 0][17] = 0.030309; m[ 0][18] = 0.087061; m[ 0][19] = 1.230985; 
	m[ 1][ 0] = 0.233108; m[ 1][ 1] = 0.000000; m[ 1][ 2] = 0.210797; m[ 1][ 3] = 0.105191; m[ 1][ 4] = 0.031726; 
	m[ 1][ 5] = 0.493763; m[ 1][ 6] = 0.255240; m[ 1][ 7] = 0.156945; m[ 1][ 8] = 0.213164; m[ 1][ 9] = 0.081510; 
	m[ 1][10] = 0.192364; m[ 1][11] = 1.755838; m[ 1][12] = 0.087930; m[ 1][13] = 0.042569; m[ 1][14] = 0.128480; 
	m[ 1][15] = 0.292327; m[ 1][16] = 0.204109; m[ 1][17] = 0.046417; m[ 1][18] = 0.097010; m[ 1][19] = 0.113146; 
	m[ 2][ 0] = 0.199097; m[ 2][ 1] = 0.210797; m[ 2][ 2] = 0.000000; m[ 2][ 3] = 0.883422; m[ 2][ 4] = 0.027495; 
	m[ 2][ 5] = 0.275700; m[ 2][ 6] = 0.270417; m[ 2][ 7] = 0.362028; m[ 2][ 8] = 0.290006; m[ 2][ 9] = 0.087225; 
	m[ 2][10] = 0.069245; m[ 2][11] = 0.503060; m[ 2][12] = 0.057420; m[ 2][13] = 0.039769; m[ 2][14] = 0.083956; 
	m[ 2][15] = 0.847049; m[ 2][16] = 0.471268; m[ 2][17] = 0.010459; m[ 2][18] = 0.093268; m[ 2][19] = 0.049824; 
	m[ 3][ 0] = 0.265145; m[ 3][ 1] = 0.105191; m[ 3][ 2] = 0.883422; m[ 3][ 3] = 0.000000; m[ 3][ 4] = 0.010313; 
	m[ 3][ 5] = 0.205842; m[ 3][ 6] = 1.599461; m[ 3][ 7] = 0.311718; m[ 3][ 8] = 0.134252; m[ 3][ 9] = 0.011720; 
	m[ 3][10] = 0.060863; m[ 3][11] = 0.261101; m[ 3][12] = 0.012182; m[ 3][13] = 0.016577; m[ 3][14] = 0.160063; 
	m[ 3][15] = 0.461519; m[ 3][16] = 0.178197; m[ 3][17] = 0.011393; m[ 3][18] = 0.051664; m[ 3][19] = 0.048769; 
	m[ 4][ 0] = 0.227333; m[ 4][ 1] = 0.031726; m[ 4][ 2] = 0.027495; m[ 4][ 3] = 0.010313; m[ 4][ 4] = 0.000000; 
	m[ 4][ 5] = 0.004315; m[ 4][ 6] = 0.005321; m[ 4][ 7] = 0.050876; m[ 4][ 8] = 0.016695; m[ 4][ 9] = 0.046398; 
	m[ 4][10] = 0.091709; m[ 4][11] = 0.004067; m[ 4][12] = 0.023690; m[ 4][13] = 0.051127; m[ 4][14] = 0.011137; 
	m[ 4][15] = 0.175270; m[ 4][16] = 0.079511; m[ 4][17] = 0.007732; m[ 4][18] = 0.042823; m[ 4][19] = 0.163831; 
	m[ 5][ 0] = 0.310084; m[ 5][ 1] = 0.493763; m[ 5][ 2] = 0.275700; m[ 5][ 3] = 0.205842; m[ 5][ 4] = 0.004315; 
	m[ 5][ 5] = 0.000000; m[ 5][ 6] = 0.960976; m[ 5][ 7] = 0.128660; m[ 5][ 8] = 0.315521; m[ 5][ 9] = 0.054602; 
	m[ 5][10] = 0.243530; m[ 5][11] = 0.738208; m[ 5][12] = 0.120801; m[ 5][13] = 0.026235; m[ 5][14] = 0.156570; 
	m[ 5][15] = 0.358017; m[ 5][16] = 0.248992; m[ 5][17] = 0.021248; m[ 5][18] = 0.062544; m[ 5][19] = 0.112027; 
	m[ 6][ 0] = 0.567957; m[ 6][ 1] = 0.255240; m[ 6][ 2] = 0.270417; m[ 6][ 3] = 1.599461; m[ 6][ 4] = 0.005321; 
	m[ 6][ 5] = 0.960976; m[ 6][ 6] = 0.000000; m[ 6][ 7] = 0.250447; m[ 6][ 8] = 0.104458; m[ 6][ 9] = 0.046589; 
	m[ 6][10] = 0.151924; m[ 6][11] = 0.888630; m[ 6][12] = 0.058643; m[ 6][13] = 0.028168; m[ 6][14] = 0.205134; 
	m[ 6][15] = 0.406035; m[ 6][16] = 0.321028; m[ 6][17] = 0.018844; m[ 6][18] = 0.055200; m[ 6][19] = 0.205868; 
	m[ 7][ 0] = 0.876213; m[ 7][ 1] = 0.156945; m[ 7][ 2] = 0.362028; m[ 7][ 3] = 0.311718; m[ 7][ 4] = 0.050876; 
	m[ 7][ 5] = 0.128660; m[ 7][ 6] = 0.250447; m[ 7][ 7] = 0.000000; m[ 7][ 8] = 0.058131; m[ 7][ 9] = 0.051089; 
	m[ 7][10] = 0.087056; m[ 7][11] = 0.193243; m[ 7][12] = 0.046560; m[ 7][13] = 0.050143; m[ 7][14] = 0.124492; 
	m[ 7][15] = 0.612843; m[ 7][16] = 0.136266; m[ 7][17] = 0.023990; m[ 7][18] = 0.037568; m[ 7][19] = 0.082579; 
	m[ 8][ 0] = 0.078692; m[ 8][ 1] = 0.213164; m[ 8][ 2] = 0.290006; m[ 8][ 3] = 0.134252; m[ 8][ 4] = 0.016695; 
	m[ 8][ 5] = 0.315521; m[ 8][ 6] = 0.104458; m[ 8][ 7] = 0.058131; m[ 8][ 8] = 0.000000; m[ 8][ 9] = 0.020039; 
	m[ 8][10] = 0.103552; m[ 8][11] = 0.153323; m[ 8][12] = 0.021157; m[ 8][13] = 0.079807; m[ 8][14] = 0.078892; 
	m[ 8][15] = 0.167406; m[ 8][16] = 0.101117; m[ 8][17] = 0.020009; m[ 8][18] = 0.286027; m[ 8][19] = 0.068575; 
	m[ 9][ 0] = 0.222972; m[ 9][ 1] = 0.081510; m[ 9][ 2] = 0.087225; m[ 9][ 3] = 0.011720; m[ 9][ 4] = 0.046398; 
	m[ 9][ 5] = 0.054602; m[ 9][ 6] = 0.046589; m[ 9][ 7] = 0.051089; m[ 9][ 8] = 0.020039; m[ 9][ 9] = 0.000000; 
	m[ 9][10] = 2.089890; m[ 9][11] = 0.093181; m[ 9][12] = 0.493845; m[ 9][13] = 0.321020; m[ 9][14] = 0.054797; 
	m[ 9][15] = 0.081567; m[ 9][16] = 0.376588; m[ 9][17] = 0.034954; m[ 9][18] = 0.086237; m[ 9][19] = 3.654430; 
	m[10][ 0] = 0.424630; m[10][ 1] = 0.192364; m[10][ 2] = 0.069245; m[10][ 3] = 0.060863; m[10][ 4] = 0.091709; 
	m[10][ 5] = 0.243530; m[10][ 6] = 0.151924; m[10][ 7] = 0.087056; m[10][ 8] = 0.103552; m[10][ 9] = 2.089890; 
	m[10][10] = 0.000000; m[10][11] = 0.201204; m[10][12] = 1.105667; m[10][13] = 0.946499; m[10][14] = 0.169784; 
	m[10][15] = 0.214977; m[10][16] = 0.243227; m[10][17] = 0.083439; m[10][18] = 0.189842; m[10][19] = 1.337571; 
	m[11][ 0] = 0.393245; m[11][ 1] = 1.755838; m[11][ 2] = 0.503060; m[11][ 3] = 0.261101; m[11][ 4] = 0.004067; 
	m[11][ 5] = 0.738208; m[11][ 6] = 0.888630; m[11][ 7] = 0.193243; m[11][ 8] = 0.153323; m[11][ 9] = 0.093181; 
	m[11][10] = 0.201204; m[11][11] = 0.000000; m[11][12] = 0.096474; m[11][13] = 0.038261; m[11][14] = 0.212302; 
	m[11][15] = 0.400072; m[11][16] = 0.446646; m[11][17] = 0.023321; m[11][18] = 0.068689; m[11][19] = 0.144587; 
	m[12][ 0] = 0.211550; m[12][ 1] = 0.087930; m[12][ 2] = 0.057420; m[12][ 3] = 0.012182; m[12][ 4] = 0.023690; 
	m[12][ 5] = 0.120801; m[12][ 6] = 0.058643; m[12][ 7] = 0.046560; m[12][ 8] = 0.021157; m[12][ 9] = 0.493845; 
	m[12][10] = 1.105667; m[12][11] = 0.096474; m[12][12] = 0.000000; m[12][13] = 0.173052; m[12][14] = 0.010363; 
	m[12][15] = 0.090515; m[12][16] = 0.184609; m[12][17] = 0.022019; m[12][18] = 0.073223; m[12][19] = 0.307309; 
	m[13][ 0] = 0.116646; m[13][ 1] = 0.042569; m[13][ 2] = 0.039769; m[13][ 3] = 0.016577; m[13][ 4] = 0.051127; 
	m[13][ 5] = 0.026235; m[13][ 6] = 0.028168; m[13][ 7] = 0.050143; m[13][ 8] = 0.079807; m[13][ 9] = 0.321020; 
	m[13][10] = 0.946499; m[13][11] = 0.038261; m[13][12] = 0.173052; m[13][13] = 0.000000; m[13][14] = 0.042564; 
	m[13][15] = 0.138119; m[13][16] = 0.085870; m[13][17] = 0.128050; m[13][18] = 0.898663; m[13][19] = 0.247329; 
	m[14][ 0] = 0.399143; m[14][ 1] = 0.128480; m[14][ 2] = 0.083956; m[14][ 3] = 0.160063; m[14][ 4] = 0.011137; 
	m[14][ 5] = 0.156570; m[14][ 6] = 0.205134; m[14][ 7] = 0.124492; m[14][ 8] = 0.078892; m[14][ 9] = 0.054797; 
	m[14][10] = 0.169784; m[14][11] = 0.212302; m[14][12] = 0.010363; m[14][13] = 0.042564; m[14][14] = 0.000000; 
	m[14][15] = 0.430431; m[14][16] = 0.207143; m[14][17] = 0.014584; m[14][18] = 0.032043; m[14][19] = 0.129315; 
	m[15][ 0] = 1.817198; m[15][ 1] = 0.292327; m[15][ 2] = 0.847049; m[15][ 3] = 0.461519; m[15][ 4] = 0.175270; 
	m[15][ 5] = 0.358017; m[15][ 6] = 0.406035; m[15][ 7] = 0.612843; m[15][ 8] = 0.167406; m[15][ 9] = 0.081567; 
	m[15][10] = 0.214977; m[15][11] = 0.400072; m[15][12] = 0.090515; m[15][13] = 0.138119; m[15][14] = 0.430431; 
	m[15][15] = 0.000000; m[15][16] = 1.767766; m[15][17] = 0.035933; m[15][18] = 0.121979; m[15][19] = 0.127700; 
	m[16][ 0] = 0.877877; m[16][ 1] = 0.204109; m[16][ 2] = 0.471268; m[16][ 3] = 0.178197; m[16][ 4] = 0.079511; 
	m[16][ 5] = 0.248992; m[16][ 6] = 0.321028; m[16][ 7] = 0.136266; m[16][ 8] = 0.101117; m[16][ 9] = 0.376588; 
	m[16][10] = 0.243227; m[16][11] = 0.446646; m[16][12] = 0.184609; m[16][13] = 0.085870; m[16][14] = 0.207143; 
	m[16][15] = 1.767766; m[16][16] = 0.000000; m[16][17] = 0.020437; m[16][18] = 0.094617; m[16][19] = 0.740372; 
	m[17][ 0] = 0.030309; m[17][ 1] = 0.046417; m[17][ 2] = 0.010459; m[17][ 3] = 0.011393; m[17][ 4] = 0.007732; 
	m[17][ 5] = 0.021248; m[17][ 6] = 0.018844; m[17][ 7] = 0.023990; m[17][ 8] = 0.020009; m[17][ 9] = 0.034954; 
	m[17][10] = 0.083439; m[17][11] = 0.023321; m[17][12] = 0.022019; m[17][13] = 0.128050; m[17][14] = 0.014584; 
	m[17][15] = 0.035933; m[17][16] = 0.020437; m[17][17] = 0.000000; m[17][18] = 0.124746; m[17][19] = 0.022134; 
	m[18][ 0] = 0.087061; m[18][ 1] = 0.097010; m[18][ 2] = 0.093268; m[18][ 3] = 0.051664; m[18][ 4] = 0.042823; 
	m[18][ 5] = 0.062544; m[18][ 6] = 0.055200; m[18][ 7] = 0.037568; m[18][ 8] = 0.286027; m[18][ 9] = 0.086237; 
	m[18][10] = 0.189842; m[18][11] = 0.068689; m[18][12] = 0.073223; m[18][13] = 0.898663; m[18][14] = 0.032043; 
	m[18][15] = 0.121979; m[18][16] = 0.094617; m[18][17] = 0.124746; m[18][18] = 0.000000; m[18][19] = 0.125733; 
	m[19][ 0] = 1.230985; m[19][ 1] = 0.113146; m[19][ 2] = 0.049824; m[19][ 3] = 0.048769; m[19][ 4] = 0.163831; 
	m[19][ 5] = 0.112027; m[19][ 6] = 0.205868; m[19][ 7] = 0.082579; m[19][ 8] = 0.068575; m[19][ 9] = 3.654430; 
	m[19][10] = 1.337571; m[19][11] = 0.144587; m[19][12] = 0.307309; m[19][13] = 0.247329; m[19][14] = 0.129315; 
	m[19][15] = 0.127700; m[19][16] = 0.740372; m[19][17] = 0.022134; m[19][18] = 0.125733; m[19][19] = 0.000000; 
    
	stationary_freqs[ 0] = 0.078837;
	stationary_freqs[ 1] = 0.051238;
	stationary_freqs[ 2] = 0.042313;
	stationary_freqs[ 3] = 0.053066;
	stationary_freqs[ 4] = 0.015175;
	stationary_freqs[ 5] = 0.036713;
	stationary_freqs[ 6] = 0.061924;
	stationary_freqs[ 7] = 0.070852;
	stationary_freqs[ 8] = 0.023082;
	stationary_freqs[ 9] = 0.062056;
	stationary_freqs[10] = 0.096371;
	stationary_freqs[11] = 0.057324;
	stationary_freqs[12] = 0.023771;
	stationary_freqs[13] = 0.043296;
	stationary_freqs[14] = 0.043911;
	stationary_freqs[15] = 0.063403;
	stationary_freqs[16] = 0.055897;
	stationary_freqs[17] = 0.013272;
	stationary_freqs[18] = 0.034399;
	stationary_freqs[19] = 0.073101;

    VectorUtilities::normalize(stationary_freqs);

    // multiply stationary frequencies into exchangeability matrix
    for (size_t i = 0; i < 20; i++)
    {
        for (size_t j = 0; j < 20; j++)
        {
            m[i][j] *= stationary_freqs[j];
        }
    }
    
    // set the diagonal values
    setDiagonal();
    
    // rescale 
    rescaleToAverageRate( 1.0 );
    
    // update the eigensystem
    updateEigenSystem();
    
}


/** Destructor */
RateMatrix_Vt::~RateMatrix_Vt(void) {
    
}




RateMatrix_Vt* RateMatrix_Vt::clone( void ) const {
    return new RateMatrix_Vt( *this );
}


