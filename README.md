# BDDSampler: a uniform random sampler from BDD files in dddmp format.

BDDSampler is a software artifact of the paper:

[*R. Heradio, D. Fernandez-Amoros, J. Galindo, D. Benavides, and D. Batory, "Uniform and Scalable Sampling of Highly Configurable Systems", Empirical Software Engineering, vol. 27, no. 44, 2022.*](https://doi.org/10.1007/s10664-021-10102-5)

## Instructions 

The instructions for compiling are in the `INSTALL.txt` file.

This program generates random samples for a BDD according to the explanation given by Donald 
Knuth in section 7.1.4 of The Art of Computer Programming. First, all the nodes are decorated
with probabilities, and then each random solution is obtained performing a random walk among
the valid solutions using the probabilities.

The usage of the tool is very simple, just invoke it on the command line with two arguments; 
the size of the desired sample and the `.dddmp` file.

The results are shown in the standard output. Each solution is shown in one line. Each solution
is a sequence of 0s and 1s, which correspond to the field `.varnames` in the `dddmp` file.

You can also use the `"-names"` option to display the products using variables names such as `X`,
or `not X`. 

This program uses `dddmp` file format version 3.0 and will not work with `dddmp` files of prior
versions. The difference between versions 2.0 and 3.0 is the addition of the `.varnames` field.

## A Python wrapper of BDDSampler

A Python wrapper of BDDSampler is available at [https://github.com/rheradio/bdd4va/](https://github.com/rheradio/bdd4va/).
