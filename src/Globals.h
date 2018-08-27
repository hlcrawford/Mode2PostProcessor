#ifndef __GLOBALS_H
#define __GLOBALS_H

#include "TH1.h"

#define DECOMP 1

#define MAX_SEGS 8
#define MAX_INTPTS (2*MAX_SEGS)

#define GRID_SEGS 37
#define GRID_PTS  229138

struct globalHeader {
  Int_t type;
  Int_t length;
  long long timestamp;
};

struct ip {
  Float_t x, y, z, e; /* Here e refers to the int. pt. energy */
  Int_t seg; /* Segment # hit */
  Float_t seg_energy; /* Total energy of hit segment */
};

struct mode2Struct {
  Int_t type;
  Int_t crystal_id;
  Int_t num;
  Float_t tot_e;
  Int_t core_e[4];
  long long int timestamp;
  long long trig_time;
  Float_t t0; 
  Float_t cfd;
  Float_t chisq;
  Float_t norm_chisq;
  Float_t baseline;
  Float_t prestep;
  Float_t poststep;
  Int_t pad;
  ip intpts[MAX_INTPTS];
};

struct basis_point {
  char iseg, ir, ip, iz;
  Float_t x, y, z;
  Float_t signal[GRID_SEGS][50];
  Int_t lo_time[GRID_SEGS], hi_time[GRID_SEGS];
};

/*************** Data structures ****************/

extern mode2Struct g2;
extern globalHeader gHeader;

extern Float_t resA[120];
extern Float_t resB[120];

extern Double_t xA[36];
extern Double_t yA[36];
extern Double_t zA[36];
extern Double_t xB[36];
extern Double_t yB[36];
extern Double_t zB[36];

#endif
