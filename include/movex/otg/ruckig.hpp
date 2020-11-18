#pragma once

// This one is still under development.

#include <Eigen/Core>

#include <movex/otg/parameter.hpp>


namespace movex {

inline double Power(double v, int e) {
    return std::pow(v, e);
}

inline double Power(double v, double e) {
    return std::pow(v, e);
}

inline double Sqrt(double v) {
    return std::sqrt(v);
}

template<size_t DOFs>
class Ruckig {
    InputParameter<DOFs> current_input;

    enum class ProfileType {
        UP_1, UP_2, UP_3, UP_4, UP_5, UP_6, UP_7, UP_8,
        DOWN_1, DOWN_2, DOWN_3, DOWN_4, DOWN_5, DOWN_6, DOWN_7, DOWN_8
    };

    struct Profile {
        ProfileType type;
        std::array<double, 7> t, t_sum, j;
        std::array<double, 8> a, v, p;

        void set(double p0, double v0, double a0, std::array<double, 7> j) {
            this->j = j;
            t_sum[0] = t[0];
            a[0] = a0;
            v[0] = v0;
            p[0] = p0;

            for (size_t i = 0; i < 6; i += 1) {
                t_sum[i+1] = t_sum[i] + t[i+1];
            }
            for (size_t i = 0; i < 7; i += 1) {
                a[i+1] = a[i] + t[i] * j[i];
                v[i+1] = v[i] + t[i] * a[i] + 0.5 * std::pow(t[i], 2) * j[i];
                p[i+1] = p[i] + t[i] * v[i] + 0.5 * std::pow(t[i], 2) * a[i] + 1. / 6 * std::pow(t[i], 3) * j[i];
            }
        }
    };

    void time_UDDU_case1(Profile& profile, double p0, double v0, double a0, double pf, double vMax, double aMax, double jMax) {
        profile.t[0] = (-a0 + aMax)/jMax;
        profile.t[1] = (Power(a0,2) - 2*Power(aMax,2) - 2*jMax*v0 + 2*jMax*vMax)/(2.*aMax*jMax);
        profile.t[2] = aMax/jMax;
        profile.t[3] = (3*Power(a0,4) - 8*Power(a0,3)*aMax + 24*a0*aMax*jMax*v0 + 6*Power(a0,2)*(Power(aMax,2) - 2*jMax*v0) - 12*jMax*(2*aMax*jMax*(p0 - pf) + Power(aMax,2)*(v0 + 2*vMax) - jMax*(Power(v0,2) - 2*Power(vMax,2))))/(24.*aMax*Power(jMax,2)*vMax);
        profile.t[4] = aMax/jMax;
        profile.t[5] = -(aMax/jMax) + vMax/aMax;
        profile.t[6] = aMax/jMax;

        profile.set(p0, v0, a0, {jMax, 0, -jMax, 0, -jMax, 0, jMax});
    }

    void time_UDDU_case2(Profile& profile, double p0, double v0, double a0, double pf, double vMax, double aMax, double jMax) {
        profile.t[0] = (-a0 + aMax)/jMax;
        profile.t[1] = (6*Power(a0,2)*aMax*jMax - 18*Power(aMax,3)*jMax - 12*aMax*Power(jMax,2)*v0 + Sqrt(6)*Sqrt(Power(aMax,2)*Power(jMax,2)*(3*Power(a0,4) - 8*Power(a0,3)*aMax + 24*a0*aMax*jMax*v0 + 6*Power(a0,2)*(Power(aMax,2) - 2*jMax*v0) + 6*(Power(aMax,4) + 4*aMax*Power(jMax,2)*(-p0 + pf) - 2*Power(aMax,2)*jMax*v0 + 2*Power(jMax,2)*Power(v0,2)))))/(12.*Power(aMax,2)*Power(jMax,2));
        profile.t[2] = aMax/jMax;
        profile.t[3] = 0;
        profile.t[4] = aMax/jMax;
        profile.t[5] = (-18*Power(aMax,3)*jMax + Sqrt(6)*Sqrt(Power(aMax,2)*Power(jMax,2)*(3*Power(a0,4) - 8*Power(a0,3)*aMax + 24*a0*aMax*jMax*v0 + 6*Power(a0,2)*(Power(aMax,2) - 2*jMax*v0) + 6*(Power(aMax,4) + 4*aMax*Power(jMax,2)*(-p0 + pf) - 2*Power(aMax,2)*jMax*v0 + 2*Power(jMax,2)*Power(v0,2)))))/(12.*Power(aMax,2)*Power(jMax,2));
        profile.t[6] = aMax/jMax;

        profile.set(p0, v0, a0, {jMax, 0, -jMax, 0, -jMax, 0, jMax});
    }

    void time_UDDU_case3(Profile& profile, double p0, double v0, double a0, double pf, double vMax, double aMax, double jMax) {
        profile.t[0] = (-2*a0*jMax + Sqrt(2)*Sqrt(Power(jMax,2)*(Power(a0,2) + 2*jMax*(-v0 + vMax))))/(2.*Power(jMax,2));
        profile.t[1] = 0;
        profile.t[2] = Sqrt(Power(jMax,2)*(Power(a0,2) + 2*jMax*(-v0 + vMax)))/(Sqrt(2)*Power(jMax,2));
        profile.t[3] = (-4*Power(a0,3)*aMax*jMax + 12*a0*aMax*Power(jMax,2)*v0 + 3*Sqrt(2)*Power(a0,2)*aMax*Sqrt(Power(jMax,2)*(Power(a0,2) - 2*jMax*v0 + 2*jMax*vMax)) - 6*jMax*(2*aMax*Power(jMax,2)*(p0 - pf) + Power(aMax,2)*jMax*vMax + Power(jMax,2)*Power(vMax,2) + Sqrt(2)*aMax*(v0 + vMax)*Sqrt(Power(jMax,2)*(Power(a0,2) + 2*jMax*(-v0 + vMax)))))/(12.*aMax*Power(jMax,3)*vMax);
        profile.t[4] = aMax/jMax;
        profile.t[5] = -(aMax/jMax) + vMax/aMax;
        profile.t[6] = aMax/jMax;

        profile.set(p0, v0, a0, {jMax, 0, -jMax, 0, -jMax, 0, jMax});
    }

    void time_UDDU_case4(Profile& profile, double p0, double v0, double a0, double pf, double vMax, double aMax, double jMax) {
        profile.t[0] = (-a0 + aMax)/jMax;
        profile.t[1] = (Power(a0,2) - 2*Power(aMax,2) - 2*jMax*v0 + 2*jMax*vMax)/(2.*aMax*jMax);
        profile.t[2] = aMax/jMax;
        profile.t[3] = (3*Power(a0,4) - 8*Power(a0,3)*aMax + 24*a0*aMax*jMax*v0 + 6*Power(a0,2)*(Power(aMax,2) - 2*jMax*v0) - 12*jMax*(Power(aMax,2)*(v0 + vMax) + 2*aMax*(jMax*p0 - jMax*pf + Sqrt(jMax)*Power(vMax,1.5)) + jMax*(-Power(v0,2) + Power(vMax,2))))/(24.*aMax*Power(jMax,2)*vMax);
        profile.t[4] = Sqrt(vMax)/Sqrt(jMax);
        profile.t[5] = 0;
        profile.t[6] = Sqrt(vMax)/Sqrt(jMax);

        profile.set(p0, v0, a0, {jMax, 0, -jMax, 0, -jMax, 0, jMax});
    }

    void time_UDDU_case5(Profile& profile, double p0, double v0, double a0, double pf, double vMax, double aMax, double jMax) {
        profile.t[0] = (-2*a0*jMax + Sqrt(2)*Sqrt(Power(jMax,2)*(Power(a0,2) + 2*jMax*(-v0 + vMax))))/(2.*Power(jMax,2));
        profile.t[1] = 0;
        profile.t[2] = Sqrt(Power(jMax,2)*(Power(a0,2) + 2*jMax*(-v0 + vMax)))/(Sqrt(2)*Power(jMax,2));
        profile.t[3] = (-4*Power(a0,3)*jMax + 12*a0*Power(jMax,2)*v0 + 3*Sqrt(2)*Power(a0,2)*Sqrt(Power(jMax,2)*(Power(a0,2) - 2*jMax*v0 + 2*jMax*vMax)) - 6*(2*Power(jMax,3)*(p0 - pf) + 2*Power(jMax,2.5)*Power(vMax,1.5) + Sqrt(2)*jMax*(v0 + vMax)*Sqrt(Power(jMax,2)*(Power(a0,2) - 2*jMax*v0 + 2*jMax*vMax))))/(12.*Power(jMax,3)*vMax);
        profile.t[4] = Sqrt(vMax)/Sqrt(jMax);
        profile.t[5] = 0;
        profile.t[6] = Sqrt(vMax)/Sqrt(jMax);

        profile.set(p0, v0, a0, {jMax, 0, -jMax, 0, -jMax, 0, jMax});
    }

    void time_UDDU_case6(Profile& profile, double p0, double v0, double a0, double pf, double vMax, double aMax, double jMax) {
        double h1 = 2*a0 + aMax;
        double h2 = 5*Power(a0,2) + 6*a0*aMax + Power(aMax,2) + 2*jMax*v0;
        double h3 = Power(a0,3)*jMax + 2*Power(a0,2)*aMax*jMax + a0*Power(aMax,2)*jMax + 2*a0*Power(jMax,2)*v0 + 2*aMax*Power(jMax,2)*v0;
        double h4 = 3*Power(a0,4) + 8*Power(a0,3)*aMax + 24*a0*aMax*jMax*v0 + 6*Power(a0,2)*(Power(aMax,2) + 2*jMax*v0) + 12*jMax*(2*aMax*jMax*(p0 - pf) + Power(aMax,2)*v0 + jMax*Power(v0,2));
        double h5 = 144*Power(jMax,4)*(4*Power(a0,4) + 8*Power(a0,3)*aMax + Power(aMax,4) + 24*aMax*Power(jMax,2)*(p0 - pf) - 24*a0*aMax*jMax*v0 - 8*Power(aMax,2)*jMax*v0 + 16*Power(jMax,2)*Power(v0,2) + 4*Power(a0,2)*(Power(aMax,2) - 4*jMax*v0));
        double h6 = Power(jMax,4)*(4*Power(a0,4) + 8*Power(a0,3)*aMax + Power(aMax,4) + 24*aMax*Power(jMax,2)*(p0 - pf) - 24*a0*aMax*jMax*v0 - 8*Power(aMax,2)*jMax*v0 + 16*Power(jMax,2)*Power(v0,2) + 4*Power(a0,2)*(Power(aMax,2) - 4*jMax*v0));
        double h7 = Power(186624*Power(h3,2)*Power(jMax,4) - 62208*h1*h2*h3*Power(jMax,5) + 3456*Power(h2,3)*Power(jMax,6) + 15552*Power(h1,2)*h4*Power(jMax,6) - 10368*h2*h4*Power(jMax,6) + Sqrt(-4*Power(h5,3) + 2985984*Power(jMax,8)*Power(108*Power(h3,2) - 36*h1*h2*h3*jMax + (2*Power(h2,3) + 9*Power(h1,2)*h4 - 6*h2*h4)*Power(jMax,2),2)),0.3333333333333333);

        profile.t[0] = (-12*h1 - jMax*Sqrt((576*Power(2,0.3333333333333333)*h6 + 2*h7*(Power(2,0.6666666666666666)*h7 + 24*(3*Power(h1,2) - 2*h2)*Power(jMax,2)))/(h7*Power(jMax,4))) + Sqrt(2)*jMax*Sqrt(-(((288*Power(2,0.3333333333333333)*h6)/h7 + Power(2,0.6666666666666666)*h7 - 144*Power(h1,2)*Power(jMax,2) + 96*h2*Power(jMax,2) - (864*Sqrt(2)*jMax*(2*Power(a0,3) + 4*Power(a0,2)*aMax + Power(h1,3) - h1*h2 + 4*aMax*jMax*v0 + 2*a0*(Power(aMax,2) + 2*jMax*v0)))/Sqrt((288*Power(2,0.3333333333333333)*h6 + h7*(Power(2,0.6666666666666666)*h7 + 24*(3*Power(h1,2) - 2*h2)*Power(jMax,2)))/(h7*Power(jMax,4))))/Power(jMax,4))))/(24.*jMax);
        profile.t[1] = 0;
        profile.t[2] = -(12*aMax + jMax*Sqrt((576*Power(2,0.3333333333333333)*h6 + 2*h7*(Power(2,0.6666666666666666)*h7 + 24*(3*Power(h1,2) - 2*h2)*Power(jMax,2)))/(h7*Power(jMax,4))) - Sqrt(2)*jMax*Sqrt(-(((288*Power(2,0.3333333333333333)*h6)/h7 + Power(2,0.6666666666666666)*h7 - 144*Power(h1,2)*Power(jMax,2) + 96*h2*Power(jMax,2) - (864*Sqrt(2)*jMax*(2*Power(a0,3) + 4*Power(a0,2)*aMax + Power(h1,3) - h1*h2 + 4*aMax*jMax*v0 + 2*a0*(Power(aMax,2) + 2*jMax*v0)))/Sqrt((288*Power(2,0.3333333333333333)*h6 + h7*(Power(2,0.6666666666666666)*h7 + 24*(3*Power(h1,2) - 2*h2)*Power(jMax,2)))/(h7*Power(jMax,4))))/Power(jMax,4))))/(24.*jMax);
        profile.t[3] = 0;
        profile.t[4] = aMax/jMax;
        profile.t[5] = -(Power(aMax,2) + (Power(a0,2)*aMax)/(Sqrt((4*Power(2,0.3333333333333333)*h6)/(h7*Power(jMax,4)) + h7/(36.*Power(2,0.3333333333333333)*Power(jMax,4)) + Power(h1,2)/Power(jMax,2) - (2*h2)/(3.*Power(jMax,2)))*jMax) - aMax*Sqrt((4*Power(2,0.3333333333333333)*h6)/(h7*Power(jMax,4)) + h7/(36.*Power(2,0.3333333333333333)*Power(jMax,4)) + Power(h1,2)/Power(jMax,2) - (2*h2)/(3.*Power(jMax,2)))*jMax - (2*aMax*v0)/Sqrt((4*Power(2,0.3333333333333333)*h6)/(h7*Power(jMax,4)) + h7/(36.*Power(2,0.3333333333333333)*Power(jMax,4)) + Power(h1,2)/Power(jMax,2) - (2*h2)/(3.*Power(jMax,2))) + (aMax*jMax*Sqrt(-(((288*Power(2,0.3333333333333333)*h6)/h7 + Power(2,0.6666666666666666)*h7 - 144*Power(h1,2)*Power(jMax,2) + 96*h2*Power(jMax,2) - (864*Sqrt(2)*jMax*(2*Power(a0,3) + 4*Power(a0,2)*aMax + Power(h1,3) - h1*h2 + 4*aMax*jMax*v0 + 2*a0*(Power(aMax,2) + 2*jMax*v0)))/Sqrt((288*Power(2,0.3333333333333333)*h6 + h7*(Power(2,0.6666666666666666)*h7 + 24*(3*Power(h1,2) - 2*h2)*Power(jMax,2)))/(h7*Power(jMax,4))))/Power(jMax,4))))/(6.*Sqrt(2)) + (Power(jMax,2)*Sqrt((288*Power(2,0.3333333333333333)*h6 + h7*(Power(2,0.6666666666666666)*h7 + 24*(3*Power(h1,2) - 2*h2)*Power(jMax,2)))/(h7*Power(jMax,4)))*Sqrt(-(((288*Power(2,0.3333333333333333)*h6)/h7 + Power(2,0.6666666666666666)*h7 - 144*Power(h1,2)*Power(jMax,2) + 96*h2*Power(jMax,2) - (864*Sqrt(2)*jMax*(2*Power(a0,3) + 4*Power(a0,2)*aMax + Power(h1,3) - h1*h2 + 4*aMax*jMax*v0 + 2*a0*(Power(aMax,2) + 2*jMax*v0)))/Sqrt((288*Power(2,0.3333333333333333)*h6 + h7*(Power(2,0.6666666666666666)*h7 + 24*(3*Power(h1,2) - 2*h2)*Power(jMax,2)))/(h7*Power(jMax,4))))/Power(jMax,4))))/72.)/(2.*aMax*jMax);
        profile.t[6] = aMax/jMax;

        profile.set(p0, v0, a0, {jMax, 0, -jMax, 0, -jMax, 0, jMax});
    }

    void time_UDDU_case7(Profile& profile, double p0, double v0, double a0, double pf, double vMax, double aMax, double jMax) {
        double h1 = 2*a0 + aMax;
        double h2 = 5*Power(a0,2) + 6*a0*aMax + Power(aMax,2) + 2*jMax*v0;
        double h3 = Power(a0,3)*jMax + 2*Power(a0,2)*aMax*jMax + a0*Power(aMax,2)*jMax + 2*a0*Power(jMax,2)*v0 + 2*aMax*Power(jMax,2)*v0;
        double h4 = 3*Power(a0,4) + 8*Power(a0,3)*aMax + 24*a0*aMax*jMax*v0 + 6*Power(a0,2)*(Power(aMax,2) + 2*jMax*v0) + 12*jMax*(2*aMax*jMax*(p0 - pf) + Power(aMax,2)*v0 + jMax*Power(v0,2));
        double h5 = 144*Power(jMax,4)*(4*Power(a0,4) + 8*Power(a0,3)*aMax + Power(aMax,4) + 24*aMax*Power(jMax,2)*(p0 - pf) - 24*a0*aMax*jMax*v0 - 8*Power(aMax,2)*jMax*v0 + 16*Power(jMax,2)*Power(v0,2) + 4*Power(a0,2)*(Power(aMax,2) - 4*jMax*v0));
        double h6 = Power(jMax,4)*(4*Power(a0,4) + 8*Power(a0,3)*aMax + Power(aMax,4) + 24*aMax*Power(jMax,2)*(p0 - pf) - 24*a0*aMax*jMax*v0 - 8*Power(aMax,2)*jMax*v0 + 16*Power(jMax,2)*Power(v0,2) + 4*Power(a0,2)*(Power(aMax,2) - 4*jMax*v0));
        double h7 = Power(186624*Power(h3,2)*Power(jMax,4) - 62208*h1*h2*h3*Power(jMax,5) + 3456*Power(h2,3)*Power(jMax,6) + 15552*Power(h1,2)*h4*Power(jMax,6) - 10368*h2*h4*Power(jMax,6) + Sqrt(-4*Power(h5,3) + 2985984*Power(jMax,8)*Power(108*Power(h3,2) - 36*h1*h2*h3*jMax + (2*Power(h2,3) + 9*Power(h1,2)*h4 - 6*h2*h4)*Power(jMax,2),2)),0.3333333333333333);
        double h8 = -Sqrt(-(-3*Power(a0,4) + 8*Power(a0,3)*aMax - 24*a0*aMax*jMax*v0 - 6*Power(a0,2)*(Power(aMax,2) - 2*jMax*v0) + 12*jMax*(2*aMax*jMax*(p0 - pf) + Power(aMax,2)*v0 - jMax*Power(v0,2))));
        
        profile.t[0] = (-a0 + aMax)/jMax;
        profile.t[1] = -(-Power(a0,2) + Power(aMax,2) + h8/Sqrt(3) + aMax*Sqrt(Power(aMax,2) - (2*h8)/Sqrt(3)) + 2*jMax*v0)/(2.*aMax*jMax);
        profile.t[2] = aMax/jMax;
        profile.t[3] = 0;
        profile.t[4] = (-aMax + Sqrt(Power(aMax,2) - (2*h8)/Sqrt(3)))/(2.*jMax);
        profile.t[5] = 0;
        profile.t[6] = (-3*aMax + Sqrt(9*Power(aMax,2) - 6*Sqrt(3)*h8))/(6.*jMax);

        profile.set(p0, v0, a0, {jMax, 0, -jMax, 0, -jMax, 0, jMax});
    }

    void time_UDDU_case8(Profile& profile, double p0, double v0, double a0, double pf, double vMax, double aMax, double jMax) {
        if (std::abs(v0) < 1e-16 && std::abs(a0) < 1e-16) {
            profile.t[0] = Power(-p0 + pf,1./3)/(Power(2,1./3)*Power(jMax,1./3));
            profile.t[1] = 0;
            profile.t[2] = Power(-p0 + pf,1./3)/(Power(2,1./3)*Power(jMax,1./3));
            profile.t[3] = 0;
            profile.t[4] = Power(-p0 + pf,1./3)/(Power(2,1./3)*Power(jMax,1./3));
            profile.t[5] = 0;
            profile.t[6] = Power(-p0 + pf,1./3)/(Power(2,1./3)*Power(jMax,1./3));

            profile.set(p0, v0, a0, {jMax, 0, -jMax, 0, -jMax, 0, jMax});
            return;
        }

        double h1 = 2*a0 + aMax;
        double h2 = 5*Power(a0,2) + 6*a0*aMax + Power(aMax,2) + 2*jMax*v0;
        double h3 = Power(a0,3)*jMax + 2*Power(a0,2)*aMax*jMax + a0*Power(aMax,2)*jMax + 2*a0*Power(jMax,2)*v0 + 2*aMax*Power(jMax,2)*v0;
        double h4 = 3*Power(a0,4) + 8*Power(a0,3)*aMax + 24*a0*aMax*jMax*v0 + 6*Power(a0,2)*(Power(aMax,2) + 2*jMax*v0) + 12*jMax*(2*aMax*jMax*(p0 - pf) + Power(aMax,2)*v0 + jMax*Power(v0,2));
        double h5 = 144*Power(jMax,4)*(4*Power(a0,4) + 8*Power(a0,3)*aMax + Power(aMax,4) + 24*aMax*Power(jMax,2)*(p0 - pf) - 24*a0*aMax*jMax*v0 - 8*Power(aMax,2)*jMax*v0 + 16*Power(jMax,2)*Power(v0,2) + 4*Power(a0,2)*(Power(aMax,2) - 4*jMax*v0));
        double h6 = Power(jMax,4)*(4*Power(a0,4) + 8*Power(a0,3)*aMax + Power(aMax,4) + 24*aMax*Power(jMax,2)*(p0 - pf) - 24*a0*aMax*jMax*v0 - 8*Power(aMax,2)*jMax*v0 + 16*Power(jMax,2)*Power(v0,2) + 4*Power(a0,2)*(Power(aMax,2) - 4*jMax*v0));
        double h7 = Power(186624*Power(h3,2)*Power(jMax,4) - 62208*h1*h2*h3*Power(jMax,5) + 3456*Power(h2,3)*Power(jMax,6) + 15552*Power(h1,2)*h4*Power(jMax,6) - 10368*h2*h4*Power(jMax,6) + Sqrt(-4*Power(h5,3) + 2985984*Power(jMax,8)*Power(108*Power(h3,2) - 36*h1*h2*h3*jMax + (2*Power(h2,3) + 9*Power(h1,2)*h4 - 6*h2*h4)*Power(jMax,2),2)),0.3333333333333333);
        double h8 = -Sqrt(-(-3*Power(a0,4) + 8*Power(a0,3)*aMax - 24*a0*aMax*jMax*v0 - 6*Power(a0,2)*(Power(aMax,2) - 2*jMax*v0) + 12*jMax*(2*aMax*jMax*(p0 - pf) + Power(aMax,2)*v0 - jMax*Power(v0,2))));
        double h9 = Power(a0,2) - 2*jMax*v0;
        double h10 = Power(jMax,3)*(2*Power(a0,3) + 3*Power(jMax,2)*(-p0 + pf) - 3*a0*jMax*v0);
        double h11 = Power(jMax,2)*(3*Power(a0,4) - 24*a0*Power(jMax,2)*(p0 - pf) + 4*Power(a0,2)*jMax*v0 - 4*Power(jMax,2)*Power(v0,2));
        double h12 = jMax*(Power(a0,5) - 24*Power(a0,2)*Power(jMax,2)*(p0 - pf) + 4*Power(a0,3)*jMax*v0 + 24*Power(jMax,3)*(-p0 + pf)*v0 + 12*a0*Power(jMax,2)*Power(v0,2));
        double h13 = Power(a0,6) - 48*Power(a0,3)*Power(jMax,2)*(p0 - pf) + 6*Power(a0,4)*jMax*v0 - 144*a0*Power(jMax,3)*(p0 - pf)*v0 + 36*Power(a0,2)*Power(jMax,2)*Power(v0,2) - 72*Power(jMax,3)*(jMax*Power(p0 - pf,2) - Power(v0,3));
        double h14 = -(Power(jMax,4)*(Power(a0,2) - 2*jMax*v0)*(Power(a0,6) - 192*Power(a0,3)*Power(jMax,2)*(p0 - pf) - 6*Power(a0,4)*jMax*v0 + 576*a0*Power(jMax,3)*(p0 - pf)*v0 + 108*Power(a0,2)*Power(jMax,2)*Power(v0,2) - 24*Power(jMax,3)*(12*jMax*Power(p0 - pf,2) + 11*Power(v0,3))));
        double h15 = 2*Power(a0,3) + 3*Power(jMax,2)*(-p0 + pf) - 3*a0*jMax*v0;
        double h16 = 6*Power(6,0.3333333333333333)*Power(9*Power(h11,3) - 72*h10*h11*h12 + 48*Power(h10,2)*h13 + 108*Power(h12,2)*h9*Power(jMax,4) - 36*h11*h13*h9*Power(jMax,4) + Sqrt(3)*Sqrt(-Power(h14,3) + 3*Power(3*Power(h11,3) + 4*(4*Power(h10,2)*h13 + 9*Power(h12,2)*h9*Power(jMax,4)) - 12*h11*(2*h10*h12 + h13*h9*Power(jMax,4)),2)),0.3333333333333333);
        double h17 = 3*Power(a0,4) - 24*a0*Power(jMax,2)*(p0 - pf) + 4*Power(a0,2)*jMax*v0 - 4*Power(jMax,2)*Power(v0,2);
        double h18 = Power(a0,5) - 24*Power(a0,2)*Power(jMax,2)*(p0 - pf) + 4*Power(a0,3)*jMax*v0 + 24*Power(jMax,3)*(-p0 + pf)*v0 + 12*a0*Power(jMax,2)*Power(v0,2);
        double h19 = (-24*h15 + Sqrt(6)*h9*jMax*Sqrt((216*Power(2,0.3333333333333333)*h14*h9 + h16*(36*h11*h9 + Power(2,0.6666666666666666)*h16*h9 + 12*(8*Power(h15,2) - 9*h17*h9)*Power(jMax,2)))/(h16*Power(h9,2)*Power(jMax,4))) + Sqrt(6)*h9*jMax*Sqrt(-((36*h11*Power(h9,2) + (216*Power(2,0.3333333333333333)*h14*Power(h9,2))/h16 + Power(2,0.6666666666666666)*h16*Power(h9,2) - 192*Power(h15,2)*h9*Power(jMax,2) + 108*h17*Power(h9,2)*Power(jMax,2) + (96*Sqrt(6)*(8*Power(h15,3) - 9*h15*h17*h9 + 9*h18*Power(h9,2))*jMax)/Sqrt((216*Power(2,0.3333333333333333)*h14*h9 + h16*(36*h11*h9 + Power(2,0.6666666666666666)*h16*h9 + 12*(8*Power(h15,2) - 9*h17*h9)*Power(jMax,2)))/(h16*Power(h9,2)*Power(jMax,4))))/(Power(h9,3)*Power(jMax,4)))))/(72.*h9*jMax);
        double h20 = (-24*h15 + Sqrt(6)*h9*jMax*Sqrt((216*Power(2,0.3333333333333333)*h14*h9 + h16*(36*h11*h9 + Power(2,0.6666666666666666)*h16*h9 + 12*(8*Power(h15,2) - 9*h17*h9)*Power(jMax,2)))/(h16*Power(h9,2)*Power(jMax,4))) - Sqrt(6)*h9*jMax*Sqrt(-((36*h11*Power(h9,2) + (216*Power(2,0.3333333333333333)*h14*Power(h9,2))/h16 + Power(2,0.6666666666666666)*h16*Power(h9,2) - 192*Power(h15,2)*h9*Power(jMax,2) + 108*h17*Power(h9,2)*Power(jMax,2) + (96*Sqrt(6)*(8*Power(h15,3) - 9*h15*h17*h9 + 9*h18*Power(h9,2))*jMax)/Sqrt((216*Power(2,0.3333333333333333)*h14*h9 + h16*(36*h11*h9 + Power(2,0.6666666666666666)*h16*h9 + 12*(8*Power(h15,2) - 9*h17*h9)*Power(jMax,2)))/(h16*Power(h9,2)*Power(jMax,4))))/(Power(h9,3)*Power(jMax,4)))))/(72.*h9*jMax);

        if (v0 < 0.0) { // Solution 4
            profile.t[0] = -h15/(3.*h9*jMax) + Sqrt((216*Power(2,0.3333333333333333)*h14*h9 + h16*(36*h11*h9 + Power(2,0.6666666666666666)*h16*h9 + 12*Power(jMax,2)*(8*Power(h15,2) - 9*h9*(3*Power(a0,4) - 24*a0*Power(jMax,2)*(p0 - pf) + 4*Power(a0,2)*jMax*v0 - 4*Power(jMax,2)*Power(v0,2)))))/(h16*Power(h9,2)*Power(jMax,4)))/(12.*Sqrt(6)) + Sqrt(-h11/(6.*h9*Power(jMax,4)) - (Power(2,0.3333333333333333)*h14)/(h16*h9*Power(jMax,4)) - h16/(108.*Power(2,0.3333333333333333)*h9*Power(jMax,4)) + (8*Power(h15,2))/(9.*Power(h9,2)*Power(jMax,2)) + (-3*Power(a0,4) + 24*a0*Power(jMax,2)*(p0 - pf) - 4*Power(a0,2)*jMax*v0 + 4*Power(jMax,2)*Power(v0,2))/(2.*h9*Power(jMax,2)) - (4*Sqrt(0.6666666666666666)*(8*Power(h15,3) - 9*h15*h9*(3*Power(a0,4) - 24*a0*Power(jMax,2)*(p0 - pf) + 4*Power(a0,2)*jMax*v0 - 4*Power(jMax,2)*Power(v0,2)) + 9*Power(h9,2)*(Power(a0,5) - 24*Power(a0,2)*Power(jMax,2)*(p0 - pf) + 4*Power(a0,3)*jMax*v0 + 24*Power(jMax,3)*(-p0 + pf)*v0 + 12*a0*Power(jMax,2)*Power(v0,2))))/(3.*Power(h9,3)*Power(jMax,3)*Sqrt((216*Power(2,0.3333333333333333)*h14*h9 + h16*(36*h11*h9 + Power(2,0.6666666666666666)*h16*h9 + 12*Power(jMax,2)*(8*Power(h15,2) - 9*h9*(3*Power(a0,4) - 24*a0*Power(jMax,2)*(p0 - pf) + 4*Power(a0,2)*jMax*v0 - 4*Power(jMax,2)*Power(v0,2)))))/(h16*Power(h9,2)*Power(jMax,4)))))/2.;
            profile.t[1] = 0;
            profile.t[2] = (-4*Power(a0,3) + 6*a0*h9 + 6*Power(jMax,2)*p0 - 6*Power(jMax,2)*pf + 6*a0*jMax*v0 + (h9*jMax*Sqrt((216*Power(2,0.3333333333333333)*h14*h9 + h16*(36*h11*h9 + Power(2,0.6666666666666666)*h16*h9 + 12*Power(jMax,2)*(8*Power(h15,2) - 9*h9*(3*Power(a0,4) - 24*a0*Power(jMax,2)*(p0 - pf) + 4*Power(a0,2)*jMax*v0 - 4*Power(jMax,2)*Power(v0,2)))))/(h16*Power(h9,2)*Power(jMax,4))))/(2.*Sqrt(6)) + 3*h9*jMax*Sqrt(-h11/(6.*h9*Power(jMax,4)) - (Power(2,0.3333333333333333)*h14)/(h16*h9*Power(jMax,4)) - h16/(108.*Power(2,0.3333333333333333)*h9*Power(jMax,4)) + (8*Power(h15,2))/(9.*Power(h9,2)*Power(jMax,2)) + (-3*Power(a0,4) + 24*a0*Power(jMax,2)*(p0 - pf) - 4*Power(a0,2)*jMax*v0 + 4*Power(jMax,2)*Power(v0,2))/(2.*h9*Power(jMax,2)) - (4*Sqrt(0.6666666666666666)*(8*Power(h15,3) - 9*h15*h9*(3*Power(a0,4) - 24*a0*Power(jMax,2)*(p0 - pf) + 4*Power(a0,2)*jMax*v0 - 4*Power(jMax,2)*Power(v0,2)) + 9*Power(h9,2)*(Power(a0,5) - 24*Power(a0,2)*Power(jMax,2)*(p0 - pf) + 4*Power(a0,3)*jMax*v0 + 24*Power(jMax,3)*(-p0 + pf)*v0 + 12*a0*Power(jMax,2)*Power(v0,2))))/(3.*Power(h9,3)*Power(jMax,3)*Sqrt((216*Power(2,0.3333333333333333)*h14*h9 + h16*(36*h11*h9 + Power(2,0.6666666666666666)*h16*h9 + 12*Power(jMax,2)*(8*Power(h15,2) - 9*h9*(3*Power(a0,4) - 24*a0*Power(jMax,2)*(p0 - pf) + 4*Power(a0,2)*jMax*v0 - 4*Power(jMax,2)*Power(v0,2)))))/(h16*Power(h9,2)*Power(jMax,4))))))/(6.*h9*jMax);
            profile.t[3] = 0;
            profile.t[4] = -((Power(a0,7) + 13*Power(a0,6)*h19*jMax + 6*Power(a0,5)*jMax*(7*Power(h19,2)*jMax + v0) + 6*Power(a0,4)*Power(jMax,2)*(3*Power(h19,3)*jMax - 8*p0 + 8*pf + h19*v0) - 12*Power(a0,3)*Power(jMax,2)*(10*h19*jMax*(p0 - pf) + 13*Power(h19,2)*jMax*v0 - Power(v0,2)) - 36*Power(a0,2)*Power(jMax,3)*(Power(h19,2)*jMax*(p0 - pf) + 2*Power(h19,3)*jMax*v0 + 2*(-p0 + pf)*v0 + 3*h19*Power(v0,2)) + 72*Power(jMax,4)*(Power(h19,2)*jMax*(p0 - pf)*v0 + Power(h19,3)*jMax*Power(v0,2) + 2*(p0 - pf)*Power(v0,2) + h19*(-(jMax*Power(p0 - pf,2)) + Power(v0,3))) - 72*a0*Power(jMax,3)*(Power(v0,3) + jMax*(Power(p0,2) + Power(pf,2) + 4*h19*pf*v0 - 2*Power(h19,2)*Power(v0,2) - 2*p0*(pf + 2*h19*v0))))/(jMax*(-Power(a0,6) + 48*Power(a0,3)*Power(jMax,2)*(p0 - pf) + 6*Power(a0,4)*jMax*v0 - 144*a0*Power(jMax,3)*(p0 - pf)*v0 - 36*Power(a0,2)*Power(jMax,2)*Power(v0,2) + 72*Power(jMax,3)*(jMax*Power(p0 - pf,2) + Power(v0,3)))));
            profile.t[5] = 0;
            profile.t[6] = -((Power(a0,7) + 13*Power(a0,6)*h19*jMax + 6*Power(a0,5)*jMax*(7*Power(h19,2)*jMax + v0) + 6*Power(a0,4)*Power(jMax,2)*(3*Power(h19,3)*jMax - 8*p0 + 8*pf + h19*v0) - 12*Power(a0,3)*Power(jMax,2)*(10*h19*jMax*(p0 - pf) + 13*Power(h19,2)*jMax*v0 - Power(v0,2)) - 36*Power(a0,2)*Power(jMax,3)*(Power(h19,2)*jMax*(p0 - pf) + 2*Power(h19,3)*jMax*v0 + 2*(-p0 + pf)*v0 + 3*h19*Power(v0,2)) + 72*Power(jMax,4)*(Power(h19,2)*jMax*(p0 - pf)*v0 + Power(h19,3)*jMax*Power(v0,2) + 2*(p0 - pf)*Power(v0,2) + h19*(-(jMax*Power(p0 - pf,2)) + Power(v0,3))) - 72*a0*Power(jMax,3)*(Power(v0,3) + jMax*(Power(p0,2) + Power(pf,2) + 4*h19*pf*v0 - 2*Power(h19,2)*Power(v0,2) - 2*p0*(pf + 2*h19*v0))))/(jMax*(-Power(a0,6) + 48*Power(a0,3)*Power(jMax,2)*(p0 - pf) + 6*Power(a0,4)*jMax*v0 - 144*a0*Power(jMax,3)*(p0 - pf)*v0 - 36*Power(a0,2)*Power(jMax,2)*Power(v0,2) + 72*Power(jMax,3)*(jMax*Power(p0 - pf,2) + Power(v0,3)))));

        } else { // Solution 3
            profile.t[0] = (-24*h15 + Sqrt(6)*h9*jMax*Sqrt((216*Power(2,0.3333333333333333)*h14*h9 + h16*(36*h11*h9 + Power(2,0.6666666666666666)*h16*h9 + 12*(8*Power(h15,2) - 9*h17*h9)*Power(jMax,2)))/(h16*Power(h9,2)*Power(jMax,4))) - Sqrt(6)*h9*jMax*Sqrt(-((36*h11*Power(h9,2) + (216*Power(2,0.3333333333333333)*h14*Power(h9,2))/h16 + Power(2,0.6666666666666666)*h16*Power(h9,2) - 192*Power(h15,2)*h9*Power(jMax,2) + 108*h17*Power(h9,2)*Power(jMax,2) + (96*Sqrt(6)*(8*Power(h15,3) - 9*h15*h17*h9 + 9*h18*Power(h9,2))*jMax)/Sqrt((216*Power(2,0.3333333333333333)*h14*h9 + h16*(36*h11*h9 + Power(2,0.6666666666666666)*h16*h9 + 12*(8*Power(h15,2) - 9*h17*h9)*Power(jMax,2)))/(h16*Power(h9,2)*Power(jMax,4))))/(Power(h9,3)*Power(jMax,4)))))/(72.*h9*jMax);
            profile.t[1] = 0;
            profile.t[2] = (-48*Power(a0,3) + 72*a0*h9 + Sqrt(6)*h9*jMax*Sqrt((216*Power(2,0.3333333333333333)*h14*h9 + h16*(36*h11*h9 + Power(2,0.6666666666666666)*h16*h9 + 12*(8*Power(h15,2) - 9*h17*h9)*Power(jMax,2)))/(h16*Power(h9,2)*Power(jMax,4))) - Sqrt(6)*h9*jMax*Sqrt(-((36*h11*Power(h9,2) + (216*Power(2,0.3333333333333333)*h14*Power(h9,2))/h16 + Power(2,0.6666666666666666)*h16*Power(h9,2) - 192*Power(h15,2)*h9*Power(jMax,2) + 108*h17*Power(h9,2)*Power(jMax,2) + (96*Sqrt(6)*(8*Power(h15,3) - 9*h15*h17*h9 + 9*h18*Power(h9,2))*jMax)/Sqrt((216*Power(2,0.3333333333333333)*h14*h9 + h16*(36*h11*h9 + Power(2,0.6666666666666666)*h16*h9 + 12*(8*Power(h15,2) - 9*h17*h9)*Power(jMax,2)))/(h16*Power(h9,2)*Power(jMax,4))))/(Power(h9,3)*Power(jMax,4)))) + 72*Power(jMax,2)*p0 - 72*Power(jMax,2)*pf + 72*a0*jMax*v0)/(72.*h9*jMax);
            profile.t[3] = 0;
            profile.t[4] = -((Power(a0,7) + 13*Power(a0,6)*h20*jMax + 6*Power(a0,5)*jMax*(7*Power(h20,2)*jMax + v0) + 6*Power(a0,4)*Power(jMax,2)*(3*Power(h20,3)*jMax - 8*p0 + 8*pf + h20*v0) - 12*Power(a0,3)*Power(jMax,2)*(10*h20*jMax*(p0 - pf) + 13*Power(h20,2)*jMax*v0 - Power(v0,2)) - 36*Power(a0,2)*Power(jMax,3)*(Power(h20,2)*jMax*(p0 - pf) + 2*Power(h20,3)*jMax*v0 + 2*(-p0 + pf)*v0 + 3*h20*Power(v0,2)) + 72*Power(jMax,4)*(Power(h20,2)*jMax*(p0 - pf)*v0 + Power(h20,3)*jMax*Power(v0,2) + 2*(p0 - pf)*Power(v0,2) + h20*(-(jMax*Power(p0 - pf,2)) + Power(v0,3))) - 72*a0*Power(jMax,3)*(Power(v0,3) + jMax*(Power(p0,2) + Power(pf,2) + 4*h20*pf*v0 - 2*Power(h20,2)*Power(v0,2) - 2*p0*(pf + 2*h20*v0))))/(jMax*(-Power(a0,6) + 48*Power(a0,3)*Power(jMax,2)*(p0 - pf) + 6*Power(a0,4)*jMax*v0 - 144*a0*Power(jMax,3)*(p0 - pf)*v0 - 36*Power(a0,2)*Power(jMax,2)*Power(v0,2) + 72*Power(jMax,3)*(jMax*Power(p0 - pf,2) + Power(v0,3)))));
            profile.t[5] = 0;
            profile.t[6] = -((Power(a0,7) + 13*Power(a0,6)*h20*jMax + 6*Power(a0,5)*jMax*(7*Power(h20,2)*jMax + v0) + 6*Power(a0,4)*Power(jMax,2)*(3*Power(h20,3)*jMax - 8*p0 + 8*pf + h20*v0) - 12*Power(a0,3)*Power(jMax,2)*(10*h20*jMax*(p0 - pf) + 13*Power(h20,2)*jMax*v0 - Power(v0,2)) - 36*Power(a0,2)*Power(jMax,3)*(Power(h20,2)*jMax*(p0 - pf) + 2*Power(h20,3)*jMax*v0 + 2*(-p0 + pf)*v0 + 3*h20*Power(v0,2)) + 72*Power(jMax,4)*(Power(h20,2)*jMax*(p0 - pf)*v0 + Power(h20,3)*jMax*Power(v0,2) + 2*(p0 - pf)*Power(v0,2) + h20*(-(jMax*Power(p0 - pf,2)) + Power(v0,3))) - 72*a0*Power(jMax,3)*(Power(v0,3) + jMax*(Power(p0,2) + Power(pf,2) + 4*h20*pf*v0 - 2*Power(h20,2)*Power(v0,2) - 2*p0*(pf + 2*h20*v0))))/(jMax*(-Power(a0,6) + 48*Power(a0,3)*Power(jMax,2)*(p0 - pf) + 6*Power(a0,4)*jMax*v0 - 144*a0*Power(jMax,3)*(p0 - pf)*v0 - 36*Power(a0,2)*Power(jMax,2)*Power(v0,2) + 72*Power(jMax,3)*(jMax*Power(p0 - pf,2) + Power(v0,3)))));
        }

        profile.set(p0, v0, a0, {jMax, 0, -jMax, 0, -jMax, 0, jMax});
    }

    void time_DUUD_case1(Profile& profile, double p0, double v0, double a0, double pf, double vMax, double aMax, double jMax) {
        profile.t[0] = (a0 - aMax)/jMax;
        profile.t[1] = (-Power(a0,2) + 2*Power(aMax,2) - 2*jMax*v0 + 2*jMax*vMax)/(2.*aMax*jMax);
        profile.t[2] = -(aMax/jMax);
        profile.t[3] = (3*Power(a0,4) - 8*Power(a0,3)*aMax + 6*Power(a0,2)*Power(aMax,2) - 24*aMax*Power(jMax,2)*p0 + 24*aMax*Power(jMax,2)*pf + 12*Power(a0,2)*jMax*v0 - 24*a0*aMax*jMax*v0 + 12*Power(aMax,2)*jMax*v0 + 12*Power(jMax,2)*Power(v0,2) + 24*Power(aMax,2)*jMax*vMax - 24*Power(jMax,2)*Power(vMax,2))/(24.*aMax*Power(jMax,2)*vMax);
        profile.t[4] = -(aMax/jMax);
        profile.t[5] = (Power(aMax,2) + jMax*vMax)/(aMax*jMax);
        profile.t[6] = -(aMax/jMax);

        profile.set(p0, v0, a0, {-jMax, 0, jMax, 0, jMax, 0, -jMax});
    }

    void time_DUUD_case2(Profile& profile, double p0, double v0, double a0, double pf, double vMax, double aMax, double jMax) {
        profile.t[0] = (a0 - aMax)/jMax;
        profile.t[1] = (-6*Power(a0,2) + 18*Power(aMax,2) - 12*jMax*v0 - (Sqrt(6)*Sqrt(Power(aMax,2)*Power(jMax,2)*(3*Power(a0,4) - 8*Power(a0,3)*aMax + 6*Power(a0,2)*Power(aMax,2) + 6*Power(aMax,4) - 24*aMax*Power(jMax,2)*p0 + 24*aMax*Power(jMax,2)*pf + 12*Power(a0,2)*jMax*v0 - 24*a0*aMax*jMax*v0 + 12*Power(aMax,2)*jMax*v0 + 12*Power(jMax,2)*Power(v0,2))))/(aMax*jMax))/(12.*aMax*jMax);
        profile.t[2] = -(aMax/jMax);
        profile.t[3] = 0;
        profile.t[4] = -(aMax/jMax);
        profile.t[5] = -(-72*Power(aMax,3)*jMax + Sqrt(5184*Power(aMax,6)*Power(jMax,2) + 96*Power(aMax,2)*Power(jMax,2)*(3*Power(a0,4) - 8*Power(a0,3)*aMax + 6*Power(a0,2)*Power(aMax,2) - 48*Power(aMax,4) - 24*aMax*Power(jMax,2)*p0 + 24*aMax*Power(jMax,2)*pf + 12*Power(a0,2)*jMax*v0 - 24*a0*aMax*jMax*v0 + 12*Power(aMax,2)*jMax*v0 + 12*Power(jMax,2)*Power(v0,2))))/(48.*Power(aMax,2)*Power(jMax,2));
        profile.t[6] = -(aMax/jMax);

        profile.set(p0, v0, a0, {-jMax, 0, jMax, 0, jMax, 0, -jMax});
    }

    void time_DUUD_case3(Profile& profile, double p0, double v0, double a0, double pf, double vMax, double aMax, double jMax) {
        profile.t[0] = (2*a0*jMax - Sqrt(2)*Sqrt(Power(a0,2)*Power(jMax,2) + 2*Power(jMax,3)*v0 - 2*Power(jMax,3)*vMax))/(2.*Power(jMax,2));
        profile.t[1] = 0;
        profile.t[2] = -(Sqrt(Power(jMax,2)*(Power(a0,2) + 2*jMax*v0 - 2*jMax*vMax))/(Sqrt(2)*Power(jMax,2)));
        profile.t[3] = (-4*Power(a0,3)*aMax - 12*aMax*Power(jMax,2)*p0 + 12*aMax*Power(jMax,2)*pf - 12*a0*aMax*jMax*v0 + 6*Power(aMax,2)*jMax*vMax - 6*Power(jMax,2)*Power(vMax,2) + (3*Sqrt(2)*Power(a0,2)*aMax*Sqrt(Power(jMax,2)*(Power(a0,2) + 2*jMax*v0 - 2*jMax*vMax)))/jMax + 6*Sqrt(2)*aMax*v0*Sqrt(Power(jMax,2)*(Power(a0,2) + 2*jMax*v0 - 2*jMax*vMax)) + 6*Sqrt(2)*aMax*vMax*Sqrt(Power(jMax,2)*(Power(a0,2) + 2*jMax*v0 - 2*jMax*vMax)))/(12.*aMax*Power(jMax,2)*vMax);
        profile.t[4] = -(aMax/jMax);
        profile.t[5] = (Power(aMax,2) + jMax*vMax)/(aMax*jMax);
        profile.t[6] = -(aMax/jMax);

        profile.set(p0, v0, a0, {-jMax, 0, jMax, 0, jMax, 0, -jMax});
    }

    void time_DUUD_case4(Profile& profile, double p0, double v0, double a0, double pf, double vMax, double aMax, double jMax) {
        profile.t[0] = (a0 - aMax)/jMax;
        profile.t[1] = (-Power(a0,2) + 2*Power(aMax,2) - 2*jMax*v0 + 2*jMax*vMax)/(2.*aMax*jMax);
        profile.t[2] = -(aMax/jMax);
        profile.t[3];
        profile.t[4];
        profile.t[5] = 0;
        profile.t[6];

        profile.set(p0, v0, a0, {-jMax, 0, jMax, 0, jMax, 0, -jMax});
    }

    void time_DUUD_case5(Profile& profile, double p0, double v0, double a0, double pf, double vMax, double aMax, double jMax) {
        profile.t[0] = (2*a0*jMax - Sqrt(2)*Sqrt(Power(a0,2)*Power(jMax,2) + 2*Power(jMax,3)*v0 - 2*Power(jMax,3)*vMax))/(2.*Power(jMax,2));
        profile.t[1] = 0;
        profile.t[2] = -(Sqrt(Power(jMax,2)*(Power(a0,2) + 2*jMax*v0 - 2*jMax*vMax))/(Sqrt(2)*Power(jMax,2)));
        profile.t[3];
        profile.t[4];
        profile.t[5] = 0;
        profile.t[6];

        profile.set(p0, v0, a0, {-jMax, 0, jMax, 0, jMax, 0, -jMax});
    }

    void time_DUUD_case6(Profile& profile, double p0, double v0, double a0, double pf, double vMax, double aMax, double jMax) {
        profile.t[0];
        profile.t[1] = 0;
        profile.t[2];
        profile.t[3] = 0;
        profile.t[4] = -(aMax/jMax);
        profile.t[5];
        profile.t[6] = -(aMax/jMax);

        profile.set(p0, v0, a0, {-jMax, 0, jMax, 0, jMax, 0, -jMax});
    }

    void time_DUUD_case7(Profile& profile, double p0, double v0, double a0, double pf, double vMax, double aMax, double jMax) {
        profile.t[0] = (a0 - aMax)/jMax;
        profile.t[1];
        profile.t[2] = -(aMax/jMax);
        profile.t[3] = 0;
        profile.t[4];
        profile.t[5] = 0;
        profile.t[6];

        profile.set(p0, v0, a0, {-jMax, 0, jMax, 0, jMax, 0, -jMax});
    }

    void time_DUUD_case8(Profile& profile, double p0, double v0, double a0, double pf, double vMax, double aMax, double jMax) {
        profile.t[0];
        profile.t[1] = 0;
        profile.t[2];
        profile.t[3] = 0;
        profile.t[4];
        profile.t[5] = 0;
        profile.t[6];

        profile.set(p0, v0, a0, {-jMax, 0, jMax, 0, jMax, 0, -jMax});
    }

    void calculate_times(Profile& profile, double p0, double v0, double a0, double pf, double vMax, double aMax, double jMax) {
        switch (profile.type) {
            case ProfileType::UP_1: {
                time_UDDU_case1(profile, p0, v0, a0, pf, vMax, aMax, jMax);
            } break;
            case ProfileType::UP_2: {
                time_UDDU_case2(profile, p0, v0, a0, pf, vMax, aMax, jMax);
            } break;
            case ProfileType::UP_3: {
                time_UDDU_case3(profile, p0, v0, a0, pf, vMax, aMax, jMax);
            } break;
            case ProfileType::UP_4: {
                time_UDDU_case4(profile, p0, v0, a0, pf, vMax, aMax, jMax);
            } break;
            case ProfileType::UP_5: {
                time_UDDU_case5(profile, p0, v0, a0, pf, vMax, aMax, jMax);
            } break;
            case ProfileType::UP_6: {
                time_UDDU_case6(profile, p0, v0, a0, pf, vMax, aMax, jMax);
            } break;
            case ProfileType::UP_7: {
                time_UDDU_case7(profile, p0, v0, a0, pf, vMax, aMax, jMax);
            } break;
            case ProfileType::UP_8: {
                time_UDDU_case8(profile, p0, v0, a0, pf, vMax, aMax, jMax);
            } break;
            case ProfileType::DOWN_1: {
                time_DUUD_case1(profile, p0, v0, a0, pf, vMax, aMax, jMax);
            } break;
            case ProfileType::DOWN_2: {
                time_DUUD_case2(profile, p0, v0, a0, pf, vMax, aMax, jMax);
            } break;
            case ProfileType::DOWN_3: {
                time_DUUD_case3(profile, p0, v0, a0, pf, vMax, aMax, jMax);
            } break;
            case ProfileType::DOWN_4: {
                time_DUUD_case4(profile, p0, v0, a0, pf, vMax, aMax, jMax);
            } break;
            case ProfileType::DOWN_5: {
                time_DUUD_case5(profile, p0, v0, a0, pf, vMax, aMax, jMax);
            } break;
            case ProfileType::DOWN_6: {
                time_DUUD_case6(profile, p0, v0, a0, pf, vMax, aMax, jMax);
            } break;
            case ProfileType::DOWN_7: {
                time_DUUD_case7(profile, p0, v0, a0, pf, vMax, aMax, jMax);
            } break;
            case ProfileType::DOWN_8: {
                time_DUUD_case8(profile, p0, v0, a0, pf, vMax, aMax, jMax);
            } break;
        }
    }

    ProfileType get_profile_type(double p0, double v0, double a0, double pf, double vMax, double aMax, double jMax) {
        Profile profile;

        // Test all cases

        time_UDDU_case1(profile, p0, v0, a0, pf, vMax, aMax, jMax);
        if (std::all_of(profile.t.begin(), profile.t.end(), [](double t){ return t > 0; })) {
            std::cout << "UP_1" << std::endl;
            return ProfileType::UP_1;  // If no max acc0, acc1, vel:
        }

        time_UDDU_case3(profile, p0, v0, a0, pf, vMax, aMax, jMax);
        if (std::all_of(profile.t.begin(), profile.t.end(), [](double t){ return t >= 0; })) {
            std::cout << "UP_3" << std::endl;
            return ProfileType::UP_3;  // If no max acc0
        }

        time_UDDU_case4(profile, p0, v0, a0, pf, vMax, aMax, jMax);
        if (std::all_of(profile.t.begin(), profile.t.end(), [](double t){ return t >= 0; })) {
            std::cout << "UP_4" << std::endl;
            return ProfileType::UP_4;  // // If no max acc1
        }

        time_UDDU_case2(profile, p0, v0, a0, pf, vMax, aMax, jMax);
        if (std::all_of(profile.t.begin(), profile.t.end(), [](double t){ return t >= 0; })) {
            std::cout << "UP_2" << std::endl;
            return ProfileType::UP_2;  // If no max vel
        }

        time_UDDU_case5(profile, p0, v0, a0, pf, vMax, aMax, jMax);
        if (std::all_of(profile.t.begin(), profile.t.end(), [](double t){ return t >= 0; })) {
            std::cout << "UP_5" << std::endl;
            return ProfileType::UP_5;  // If no max acc0, acc1
        }

        time_UDDU_case6(profile, p0, v0, a0, pf, vMax, aMax, jMax);
        if (std::all_of(profile.t.begin(), profile.t.end(), [](double t){ return t >= 0; })) {
            std::cout << "UP_6" << std::endl;
            return ProfileType::UP_6;  // If no max acc0, vel
        }

        time_UDDU_case7(profile, p0, v0, a0, pf, vMax, aMax, jMax);
        if (std::all_of(profile.t.begin(), profile.t.end(), [](double t){ return t >= 0; })) {
            std::cout << "UP_7" << std::endl;
            return ProfileType::UP_7;  // If no max acc1, vel
        }

        time_UDDU_case8(profile, p0, v0, a0, pf, vMax, aMax, jMax);
        if (std::all_of(profile.t.begin(), profile.t.end(), [](double t){ return t >= 0; })) {
            std::cout << "UP_8" << std::endl;
            return ProfileType::UP_8;  // If no limit:
        }

        throw std::runtime_error("Could not find trajectory.");
    }

    bool calculate(const InputParameter<DOFs>& input) {
        current_input = input;

        // Find profile first.
        for (size_t dof = 0; dof < DOFs; dof += 1) {
            profiles[dof].type = get_profile_type(input.current_position[dof], input.current_velocity[dof], input.current_acceleration[dof], input.target_position[dof], input.max_velocity[dof], input.max_acceleration[dof], input.max_jerk[dof]);
            calculate_times(profiles[dof], input.current_position[dof], input.current_velocity[dof], input.current_acceleration[dof], input.target_position[dof], input.max_velocity[dof], input.max_acceleration[dof], input.max_jerk[dof]);
            tf = profiles[dof].t_sum[6];
        }

        t = 0.0;

        return true;
    }

    double t, tf;
    std::array<Profile, DOFs> profiles;

public:
    double delta_time;

    explicit Ruckig(double delta_time): delta_time(delta_time) { }

    Result update(const InputParameter<DOFs>& input, OutputParameter<DOFs>& output) {
        t += delta_time;

        if (input != current_input && !calculate(input)) {
            return Result::Error;
        }

        auto p = profiles[0];
        auto index_ptr = std::upper_bound(p.t_sum.begin(), p.t_sum.end(), t);
        size_t index = std::distance(p.t_sum.begin(), index_ptr);

        if (t + delta_time > p.t_sum[6]) {
            output.new_position[0] = input.target_position[0];
            output.new_velocity[0] = input.target_velocity[0];
            output.new_acceleration[0] = input.target_acceleration[0];
            return Result::Finished;
        }

        double t_diff = t;
        if (index > 0) {
            t_diff -= p.t_sum[index - 1];
        }

        output.new_acceleration[0] = p.a[index] + t_diff * p.j[index];
        output.new_velocity[0] = p.v[index] + t_diff * p.a[index] + 0.5 * std::pow(t_diff, 2) * p.j[index];
        output.new_position[0] = p.p[index] + t_diff * p.v[index] + 0.5 * std::pow(t_diff, 2) * p.a[index] + 1. / 6 * std::pow(t_diff, 3) * p.j[index];

        current_input.current_position = output.new_position;
        current_input.current_velocity = output.new_velocity;
        current_input.current_acceleration = output.new_acceleration;
        return Result::Working;
    }
};

} // namespace movex
