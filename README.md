# hw2
This is a description of assignment 2 for HPC spring 2015.

Summary for my final project:

An implementation of a parallel MCMC algorithm

The project is to implement a parallel MCMC algorithm developbed by Jonathan Goodman and Jonathan Weare in 2010.
The algorithm is good at providing high quality independent samples and can be significantly faster than standard
MCMC methods on highly skewed distributions. Another advantage  of the algorithm is that it requires much fewer
parameters than a traditional algorithm.

I want to implement this parallel MCMC algorithm using MPI and test its performance compared to traditional methods.
Autocorrelation time and acceptance rate can be very important measures for MCMC algorithms. And I will also test 
whether the parallel algorithm is scalling or not.
