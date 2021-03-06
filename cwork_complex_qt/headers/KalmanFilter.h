#ifndef _KALMAN_FILTER_
#define _KALMAN_FILTER_


// #include "vector_matrix.h"
// #include <math.h>
// #include "WhiteNoiseGenerator.h"

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>

namespace ublas = boost::numeric::ublas;


// template<class T>
// bool InvertMatrix(const ublas::matrix<T>& input, ublas::matrix<T>& inverse) {
// 	using namespace boost::numeric::ublas;
// 	typedef permutation_matrix<std::size_t> pmatrix;
// 	// create a working copy of the input
// 	matrix<T> A(input);
// 	// create a permutation matrix for the LU-factorization
// 	pmatrix pm(A.size1());
// 	// perform LU-factorization
// 	int res = lu_factorize(A,pm);
//     if( res != 0 ) return false;
// 	// create identity matrix of "inverse"
// 	inverse.assign(ublas::identity_matrix<T>(A.size1()));
// 	// backsubstitute to get the inverse
// 	lu_substitute(A, pm, inverse);
// 	return true;
// };


bool InvertMatrix(const ublas::matrix<float>& input, ublas::matrix<float>& inverse) {
    using namespace boost::numeric::ublas;
    //matrix<float> inverse(input.size1(), input.size2());
    typedef permutation_matrix<float> pmatrix;
    // create a working copy of the input
    matrix<float> A(input);
    // create a permutation matrix for the LU-factorization
    pmatrix pm(A.size1());
    // perform LU-factorization
    int res = lu_factorize(A,pm);
    if( res != 0 ) return false;
    // create identity matrix of "inverse"
    inverse.assign(ublas::identity_matrix<float>(A.size1()));
    // backsubstitute to get the inverse
    lu_substitute(A, pm, inverse);
    return true;
};



class KalmanFilter{
public:
    KalmanFilter(vector<float> beginDeltaX){
        // vector<float> zero(6);
        // for (int i = 0; i < count; ++i)
        // {
        // 	zero(i) = 0;
        // }
        estDeltaX = beginDeltaX;
        matrix<float> P(6,6);
        for (int i = 0; i < 6; ++i){
            for (int j = 0; j < 6; ++j){
                P(i,j) = 0;
            }
        }
        for (int i = 0; i < 3; ++i)
        {
            P(i,i) = 10000;//9*pow(beginDeltaX(i),2);  /// какие начальные значения задавать для дельта x
        };
        for (int i = 3; i < 6; ++i)
        {
            P(i,i) = 1000;//9*pow(beginDeltaX(i),2);  /// какие начальные значения задавать для дельта x
        };
        estP = P;
    };



    vector<float> estimateDeltaX(vector<float> deltaX, vector<float> deltaY, matrix<float> F, matrix<float> H, matrix<float> D){
        estimateP(F,H,D);

        int numSat = H.size1();

        //std::cout<< D<< std::endl;

        matrix<float> Dinv(numSat, numSat);
        vector<float> Fx(6);
//        Fx(0) = 0;
//        Fx(1) = 0;
//        Fx(2) = 0;
//        Fx(3) = 0;
//        Fx(4) = 0;
//        Fx(5) = 0;
        matrix<float> PH(6, numSat);
        matrix<float> PHDinv(numSat, numSat);
        matrix<float> HF(numSat, 6);
        vector<float> HFx(numSat);
        vector<float> yHFx(numSat);

        InvertMatrix(D, Dinv);

        Fx = prod(F, deltaX);
        PH = prod(estP, trans(H)); //trans
        PHDinv = prod(PH, Dinv);
        HF = prod(H, F);
        HFx = prod(HF, deltaX);
        yHFx = deltaY - HFx;

        return Fx + prod(PHDinv, yHFx);
    };



private:
    vector<float> estDeltaX;
    matrix<float> estP;


    void estimateP(matrix<float> F, matrix<float> H, matrix<float> D){
        int numSat = H.size1();

        matrix<float> FP(6,6);
        matrix<float> FPF(6,6);
        matrix<float> FPFinv(6,6);
        matrix<float> Dinv(numSat, numSat);
        matrix<float> HDinv(6, numSat);
        matrix<float> HDinvH(6,6);
        matrix<float> FPFinvHDinvH(6,6);

        FP = prod(F, estP);
        FPF = prod(FP, trans(F));
        InvertMatrix(FPF, FPFinv);
        InvertMatrix(D, Dinv);
        HDinv = prod(trans(H), Dinv);
        HDinvH = prod(HDinv, H);
        FPFinvHDinvH = FPFinv + HDinvH;

        InvertMatrix(FPFinvHDinvH, estP);
    };

};


#endif
