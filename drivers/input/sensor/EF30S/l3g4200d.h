#ifndef _L3G4200D_H_
#define _L3G4200D_H_

typedef struct {
	int x;
	int y;
	int z;
} axes_t;

int l3g4200d_control_enable(int enable);
int l3g4200d_measure(axes_t *val);

#endif // _L3G4200D_H_
