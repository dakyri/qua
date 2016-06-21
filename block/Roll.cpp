#include "qua_version.h"

#include "StdDefs.h"


#include <stdio.h>
#include <stdlib.h>

#include "StdDefs.h"

#include "QuaTime.h"	// so other doesn't barf
#include "Other.h"

flag		roll_debug=0;

float		frandom();

#define MAX_SELECT_LIST	128

/*
 * select an item from {0..n-1}, where relative probabilities are given in "pr".
 */
int
select_list(int n, float pr[])
{
    int		i;
    float	cum[MAX_SELECT_LIST];
	float	toss;
    int		cnt;

    if (roll_debug)  {
		fprintf(stderr, "prob map:");
		for (i=0; i<n; i++) {
		    if (i % 10 == 0) fprintf(stderr, "\n");
		    fprintf(stderr, "%g ", pr[i]);
		}
		fprintf(stderr, "\n");
    }
    if (n > MAX_SELECT_LIST) {
		internalError("Error in SelectList: sample too big\n");
	exit(1);
    }
    cum[0] = pr[0];
    for (i=1; i<n; i++) {
		cum[i] = cum[i-1] + pr[i];
		if (roll_debug)
			fprintf(stderr, "cum %d %g %g\n", i, cum[i], pr[i]);
    }
    if (cum[n-1] == 0.0) {  /* Perhaps this should return rather than exit ? */
		internalError("**** flatwall against the laws of statistics\n");
		exit(0);
    }
    cnt = 0;
    do {
	toss = frandom();
	for (i=0; i<n; i++) {
	    if (toss < cum[i]) {
			break;
	    }
		}
		if (cnt ++ > 100) {
		    internalError("**** statistical anomaly in select list\n");
			exit(1);
		}
    } while (i==n);	/* in case the cumulative probs dont sum to 1 */
    return i;
}

/*
 * a random float between 0 and 1
 * cute things happened on the first Be run, with rands
 * divided by 0x7ffffff. The lower probabilities generated
 * shorter quirkier loops.
 */
float
frandom()
{
    return (float)((float) rand()) / ((float) RAND_MAX);
}

bool
roll(float chance)
{
    return (bool)(frandom() < chance);
}

