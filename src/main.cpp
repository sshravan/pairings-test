#include <libsnark/common/default_types/r1cs_gg_ppzksnark_pp.hpp>

#include <libff/common/profiling.hpp>

#include <libff/algebra/curves/bn128/bn128_pp.hpp>
#include <libff/algebra/curves/mnt/mnt4/mnt4_pp.hpp>
#include <libff/algebra/curves/mnt/mnt6/mnt6_pp.hpp>
#include <libfqfft/evaluation_domain/get_evaluation_domain.hpp>

#include "depends/ate-pairing/include/bn.h"
#include "test_point.hpp"

#include <vector>
#include <chrono>
#include <fstream>

#include <gmp.h>
#include <gmpxx.h>


using namespace libsnark;
using namespace libff;
using namespace libfqfft;
using namespace std;
using namespace bn;

template<typename ppT>
bool pairing_test_libff(const int &txn_count, const int &num_pairing){

    int N = txn_count * num_pairing;
    auto t1 = chrono::high_resolution_clock::now(), t2 = chrono::high_resolution_clock::now();
    auto t3 = t2 - t1;

    GT<ppT> GT_one = GT<ppT>::one();
    vector<G1<ppT> > P(N, (Fr<ppT>::random_element()) * G1<ppT>::one());
    vector<G2<ppT> > Q(N, (Fr<ppT>::random_element()) * G2<ppT>::one());

    vector<Fr<ppT> > s(N, Fr<ppT>::random_element());

    vector<G1<ppT> > sP(N);
    vector<G2<ppT> > sQ(N);

    for (int i = 0; i < N; ++i){
        sP[i] = s[i] * P[i];
        sQ[i] = s[i] * Q[i];
    }

    bool flag = 1;
    GT<ppT> ans1, ans2, ans3;

    t1 = chrono::high_resolution_clock::now();
    for (int i = 0; i < N; ++i){
        ans1 = ppT::reduced_pairing(sP[i], Q[i]);
        ans2 = ppT::reduced_pairing(P[i], sQ[i]);
        // ans3 = ppT::reduced_pairing(P[i], Q[i])^s[i];
        flag = flag & (ans1 == ans2);
    }
    t2 = chrono::high_resolution_clock::now();
    t3 = t2 - t1;
    cout << "Pairing: " << chrono::duration<double, milli>(t3).count() / 1000 << "s, Status: " << flag << endl;

    return flag;
}

template<typename ppT>
bool affine_pairing_test_libff(const int &txn_count, const int &num_pairing) {

    int N = txn_count * num_pairing;
    auto t1 = chrono::high_resolution_clock::now(), t2 = chrono::high_resolution_clock::now();
    auto t3 = t2 - t1;

    GT<ppT> GT_one = GT<ppT>::one();
    vector<G1<ppT> > P(N, (Fr<ppT>::random_element()) * G1<ppT>::one());
    vector<G2<ppT> > Q(N, (Fr<ppT>::random_element()) * G2<ppT>::one());

    vector<Fr<ppT> > s(N, Fr<ppT>::random_element());

    vector<G1<ppT> > sP(N);
    vector<G2<ppT> > sQ(N);

    for (int i = 0; i < N; ++i){
        sP[i] = s[i] * P[i];
        sQ[i] = s[i] * Q[i];
    }

    bool flag = 1;
    GT<ppT> ans1, ans2, ans3;

    t1 = chrono::high_resolution_clock::now();
    for (int i = 0; i < N; ++i){
        ans1 = ppT::affine_reduced_pairing(sP[i], Q[i]);
        ans2 = ppT::affine_reduced_pairing(P[i], sQ[i]);
        // ans3 = ppT::reduced_pairing(P[i], Q[i])^s[i];
        flag = flag & (ans1 == ans2);
    }
    t2 = chrono::high_resolution_clock::now();
    t3 = t2 - t1;
    cout << "Pairing Affine: " << chrono::duration<double, milli>(t3).count() / 1000 << "s, Status: " << flag << endl;
    return flag;
}

bool bn_pairing(const int &txn_count, const int &num_pairing) {

    int N = txn_count * num_pairing;
    mpz_class p;
    bn::CurveParam cp = bn::CurveSNARK1;
    p.set_str("21888242871839275222246405745257275088548364400416034343698204186575808495617", 10);
    puts("BN2 Snark Curves");

    Param::init(cp);
    const Point &pt = selectPoint(cp);
    const Ec2 g2(
        Fp2(Fp(pt.g2.aa), Fp(pt.g2.ab)),
        Fp2(Fp(pt.g2.ba), Fp(pt.g2.bb)));
    const Ec1 g1(pt.g1.a, pt.g1.b);

    unsigned long int seed;
    gmp_randstate_t r_state;
    short size = sizeof(seed);
    ifstream urandom("/dev/urandom", ios::in | ios::binary);
    urandom.read((char*)&seed, size);
    urandom.close();
    seed = 8;

    gmp_randinit_default(r_state);
    gmp_randseed_ui(r_state, seed);

    bool flag = true;

    auto t1 = chrono::high_resolution_clock::now(), t2 = chrono::high_resolution_clock::now();
    auto t3 = t2 - t1;

    vector<Ec1> P(N);
    vector<Ec2> Q(N);
    vector<mpz_class> s(N);

    mpz_class fr;

    for (int i = 0; i < N; i++){
        mpz_urandomm(fr.get_mpz_t(), r_state, p.get_mpz_t());
        const mie::Vuint temp((fr.get_str()).c_str());
        P[i] = g1 * temp;
    }

    for (int i = 0; i < N; i++){
        mpz_urandomm(fr.get_mpz_t(), r_state, p.get_mpz_t());
        const mie::Vuint temp((fr.get_str()).c_str());
        Q[i] = g2 * temp;
    }

    for (int i = 0; i < N; i++){
        mpz_urandomm(s[i].get_mpz_t(), r_state, p.get_mpz_t());
    }

    Fp12 e1, e2;

    t1 = chrono::high_resolution_clock::now();
    for (int i = 0; i < N; i++){
        const mie::Vuint temp((s[i].get_str()).c_str());
        // opt_atePairing(e1, Q[i], P[i] * s[i]);
        opt_atePairing(e1, Q[i], P[i] * temp);
        opt_atePairing(e2, Q[i] * temp, P[i]);
        flag = flag * (e1 == e2);
    }
    t2 = chrono::high_resolution_clock::now();
    t3 = t2 - t1;
    cout << "Opt-ate: " << chrono::duration<double, milli>(t3).count() / 1000 << "s, Status: " << flag << endl;
    return flag;
    return 0;

}

template <typename ppT>
void driver(const int &txn_count, const int &num_pairing){

    bool flag = 1;

    ppT::init_public_params();
    flag = pairing_test_libff<ppT>(txn_count, num_pairing);
    // cout << "Pairing: " << chrono::duration<double, milli>(t3).count() / 1000 << "s, " << " Status: " << flag << endl;

    flag = affine_pairing_test_libff<ppT>(txn_count, num_pairing);
    // cout << "Pairing Affine: " << chrono::duration<double, milli>(t3).count() / 1000 << "s, " << " Status: " << flag << endl;
}

int main(int argc, char **argv){

    libff::inhibit_profiling_info = true;
    libff::inhibit_profiling_counters = true;

    int num_pairing = atoi(argv[1]);
    int runs = 1024;

    cout << "No of pairings: " << 2 * num_pairing << endl;
    cout << "No of runs: " << runs << endl;
    cout << "=============================================================" << endl;

    cout << "MNT6" << endl;
    mnt6_pp::init_public_params();
    driver<mnt6_pp>(runs, num_pairing);
    cout << "=============================================================" << endl;

    cout << "MNT4" << endl;
    mnt4_pp::init_public_params();
    driver<mnt4_pp>(runs, num_pairing);
    cout << "=============================================================" << endl;

    cout << "BN" << endl;
    bn_pairing(runs, num_pairing);
    cout << "=============================================================" << endl;

    return 0;
}
