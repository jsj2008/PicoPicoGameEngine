#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "FlMML_Number.h"
#include "FlMML_Vector.h"

namespace FlMML {

/**
 * This class was created based on "Formant filter" that programmed by alex.
 See following URL; http://www.musicdsp.org/showArchiveComment.php?ArchiveID=110
 Thanks to his great works!
*/
class MFormant {
public:
	static const int VOWEL_A=0;
	static const int VOWEL_E=1;
	static const int VOWEL_I=2;
	static const int VOWEL_O=3;
	static const int VOWEL_U=4;
private:
	// ca = filter coefficients of 'a'
	static const double m_ca0;
	static const double m_ca1;
	static const double m_ca2;
	static const double m_ca3;
	static const double m_ca4;
	static const double m_ca5;
	static const double m_ca6;
	static const double m_ca7;
	static const double m_ca8;
	static const double m_ca9;
	static const double m_caA;
	
	// ce = filter coefficients of 'e'
	static const double m_ce0;
	static const double m_ce1;
	static const double m_ce2;
	static const double m_ce3;
	static const double m_ce4;
	static const double m_ce5;
	static const double m_ce6;
	static const double m_ce7;
	static const double m_ce8;
	static const double m_ce9;
	static const double m_ceA;
	
	// ci = filter coefficients of 'i'
	static const double m_ci0;
	static const double m_ci1;
	static const double m_ci2;
	static const double m_ci3;
	static const double m_ci4;
	static const double m_ci5;
	static const double m_ci6;
	static const double m_ci7;
	static const double m_ci8;
	static const double m_ci9;
	static const double m_ciA;
	
	// co = filter coefficients of 'o'
	static const double m_co0;
	static const double m_co1;
	static const double m_co2;
	static const double m_co3;
	static const double m_co4;
	static const double m_co5;
	static const double m_co6;
	static const double m_co7;
	static const double m_co8;
	static const double m_co9;
	static const double m_coA;
	
	// cu = filter coefficients of 'u'
	static const double m_cu0;
	static const double m_cu1;
	static const double m_cu2;
	static const double m_cu3;
	static const double m_cu4;
	static const double m_cu5;
	static const double m_cu6;
	static const double m_cu7;
	static const double m_cu8;
	static const double m_cu9;
	static const double m_cuA;
	
	double m_m0;double m_m1;double m_m2;double m_m3;double m_m4;
	double m_m5;double m_m6;double m_m7;double m_m8;double m_m9;
	
	int m_vowel;
	bool m_power;
public:
	MFormant() {
		m_vowel=VOWEL_A;
		m_power=false;
		reset();
	}
	
	void setVowel(int vowel) {
		m_power=true;
		m_vowel=vowel;
	}
	
	void disable() {
		m_power=false;
		reset();
	}
	
	void reset() {
		m_m0=m_m1=m_m2=m_m3=m_m4=m_m5=m_m6=m_m7=m_m8=m_m9=0;
	}
	
	
	// 無音入力時に何かの信号を出力するかのチェック
	bool checkToSilence() {
		return m_power&&(
						 -0.000001<=m_m0&&m_m0<=0.000001&&
						 -0.000001<=m_m1&&m_m1<=0.000001&&
						 -0.000001<=m_m2&&m_m2<=0.000001&&
						 -0.000001<=m_m3&&m_m3<=0.000001&&
						 -0.000001<=m_m4&&m_m4<=0.000001&&
						 -0.000001<=m_m5&&m_m5<=0.000001&&
						 -0.000001<=m_m6&&m_m6<=0.000001&&
						 -0.000001<=m_m7&&m_m7<=0.000001&&
						 -0.000001<=m_m8&&m_m8<=0.000001&&
						 -0.000001<=m_m9&&m_m9<=0.000001
						 );
	}
	
	void run(VNumber* samples,int start,int end) {
		if (!m_power)return ;
		int i;
		switch(m_vowel){
			case 0:
				for (i=start;i<end;i++){
					double t = (samples)[i];
					t=m_ca0*t+
					m_ca1*m_m0+m_ca2*m_m1+
					m_ca3*m_m2+m_ca4*m_m3+
					m_ca5*m_m4+m_ca6*m_m5+
					m_ca7*m_m6+m_ca8*m_m7+
					m_ca9*m_m8+m_caA*m_m9;
					m_m9=m_m8;
					m_m8=m_m7;
					m_m7=m_m6;
					m_m6=m_m5;
					m_m5=m_m4;
					m_m4=m_m3;
					m_m3=m_m2;
					m_m2=m_m1;
					m_m1=m_m0;
					m_m0=t;
					(samples)[i]=t;
				}
				return ;
			case 1:
				for (i=start;i<end;i++){
					double t = (samples)[i];
					t=m_ce0*t+
					m_ce1*m_m0+m_ce2*m_m1+
					m_ce3*m_m2+m_ce4*m_m3+
					m_ce5*m_m4+m_ce6*m_m5+
					m_ce7*m_m6+m_ce8*m_m7+
					m_ce9*m_m8+m_ceA*m_m9;
					m_m9=m_m8;
					m_m8=m_m7;
					m_m7=m_m6;
					m_m6=m_m5;
					m_m5=m_m4;
					m_m4=m_m3;
					m_m3=m_m2;
					m_m2=m_m1;
					m_m1=m_m0;
					m_m0=t;
					(samples)[i]=t;
				}
				return ;
			case 2:
				for (i=start;i<end;i++){
					double t = (samples)[i];
					t=m_ci0*t+
					m_ci1*m_m0+m_ci2*m_m1+
					m_ci3*m_m2+m_ci4*m_m3+
					m_ci5*m_m4+m_ci6*m_m5+
					m_ci7*m_m6+m_ci8*m_m7+
					m_ci9*m_m8+m_ciA*m_m9;
					m_m9=m_m8;
					m_m8=m_m7;
					m_m7=m_m6;
					m_m6=m_m5;
					m_m5=m_m4;
					m_m4=m_m3;
					m_m3=m_m2;
					m_m2=m_m1;
					m_m1=m_m0;
					m_m0=t;
					(samples)[i]=t;
				}
				return ;
			case 3:
				for (i=start;i<end;i++){
					double t = (samples)[i];
					t=m_co0*t+
					m_co1*m_m0+m_co2*m_m1+
					m_co3*m_m2+m_co4*m_m3+
					m_co5*m_m4+m_co6*m_m5+
					m_co7*m_m6+m_co8*m_m7+
					m_co9*m_m8+m_coA*m_m9;
					m_m9=m_m8;
					m_m8=m_m7;
					m_m7=m_m6;
					m_m6=m_m5;
					m_m5=m_m4;
					m_m4=m_m3;
					m_m3=m_m2;
					m_m2=m_m1;
					m_m1=m_m0;
					m_m0=t;
					(samples)[i]=t;
				}
				return ;
			case 4:
				for (i=start;i<end;i++){
					double t = (samples)[i];
					t=m_cu0*t+
					m_cu1*m_m0+m_cu2*m_m1+
					m_cu3*m_m2+m_cu4*m_m3+
					m_cu5*m_m4+m_cu6*m_m5+
					m_cu7*m_m6+m_cu8*m_m7+
					m_cu9*m_m8+m_cuA*m_m9;
					m_m9=m_m8;
					m_m8=m_m7;
					m_m7=m_m6;
					m_m6=m_m5;
					m_m5=m_m4;
					m_m4=m_m3;
					m_m3=m_m2;
					m_m2=m_m1;
					m_m1=m_m0;
					m_m0=t;
					(samples)[i]=t;
				}
				return ;
		}
	}
};

}
