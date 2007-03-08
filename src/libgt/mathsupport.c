/*
  Copyright (c) 2006-2007 Gordon Gremme <gremme@zbh.uni-hamburg.de>
  Copyright (c) 2006-2007 Center for Bioinformatics, University of Hamburg
  See LICENSE file or http://genometools.org/license.html for license details.
*/

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <libgt/mathsupport.h>

#define EPSILON	0.0000000000001

double logsum(double p1, double p2)
{
  if (p1 > p2)
    return (p1-p2 > 50.0) ? p1 : p1 + log(1.0 + exp(p2-p1));
  return (p2-p1 > 50.0) ? p2 : p2 + log(1.0 + exp(p1-p2));
}

bool double_equals_one(double d)
{
  if (fabs(1.0 - d) <= EPSILON)
    return true;
  return false;
}

bool double_equals_double(double d1, double d2)
{
  if (fabs(d1 - d2) <= EPSILON)
    return true;
  return false;
}

unsigned long rand_max(unsigned long maximal_value)
{
  unsigned long r;
  assert(maximal_value);
  r = ((double) rand() / ((double) RAND_MAX + 1) * (maximal_value + 1));
  assert(r <= maximal_value);
  return r;
}

double rand_max_double(double maximal_value)
{
  double r;
  assert(maximal_value);
  r = ((double) rand() / RAND_MAX) * maximal_value; /* XXX */
  assert(r >= 0.0 && r <= maximal_value);
  return r;
}

double rand_0_to_1(void)
{
  double r;
  r = (double) rand() / RAND_MAX;
  assert(r >= 0.0 && r <= 1.0);
  return r;
}
