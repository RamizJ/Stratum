#ifndef MATHFUNCTIONS_H
#define MATHFUNCTIONS_H

#include <QString>


namespace StVirtualMachine {

extern void setupMathFunctions();

extern void incAfter();
extern void inc();
extern void incByValue();
extern void dec();
extern void decByValue();

extern void addF();
extern void subF();
extern void mulF();
extern void divF();

extern void sin_();
extern void cos_();
extern void tan_();
extern void asin_();
extern void acos_();
extern void atan_();

extern void exp_();
extern void sqrt_();
extern void sqr_();
extern void ln_();
extern void lg_();
extern void log_();
extern void pow_();

extern void ed_();
extern void v_edi();
extern void delta_();

extern void max_();
extern void min_();
extern void average_();
extern void limit_();
extern void mod_();
extern void round_();
extern void trunk_();
extern void abs_();
extern void sgn_();
extern void rad_();
extern void deg_();

extern void rnd_();
extern void randomize_();

extern void unMinus_();
extern void getAngleByXY();

extern void throwDomainError(const QString& str);

}


#endif // MATHFUNCTIONS_H
