About
-----

In this repo you will find my afterburner for pre-calculating angular
distributions for pluto events.

You must be aware that this program is only for channels, where the particle
governing the angular distribution decays inside Pluto.

E.g. for reaction: p+p -> p K+ Lambda the angular distribution is governed by
Lambda so the particle which exists in the final state, so you don't need to use
the program. You can always get info about direction of Lambda inside you
analysis.

But for example with reaction, e.g. p + p -> Lambda Delta++ K0s where D++
governs the angular distribution (I have the distribution in respect to D++'s
cosTheta) decays inside Pluto to proton + pi, then in Pluto in final state
you will have following particles:

```
4vec    pid      parent         weight
...     18       reaction_id    something    // Lambda
...     16       reaction_id    something    // K0s
...     14       35             something    // proton from D++, D++ has PID 35
...     8        35             something    // pion from D++
```

In that case, you must reconstruct first the D++ from the daughter particles,
boost it back to p+p cms frame, and find out the angle. Most of the time we use
cosTheta not Theta. So one can use the event weight information which usually is
1.0 or another not relevant for us value and overwrite it with the cosTheta
which is further forwarded by simulation and can be read out in the analysis.

Usage
-----

So how to use the program. In cas case above you run:

`after_angular my_ect_file.evt -c 35:14:8`

The -c option is following: you give a chain of particles where the first
particle is the one you are looking for, and all the other particles are the
daughter particles. In this case it will find all the particles with PID of 14
and 8 which are daughter of particle with PID 35. After that new file where the
weight is equal to cosTheta of particle with PID 35 in the p+p cms frame will be
created. If the input file is `xxx.evt`, the output is `xxx.ang.evt`.

By default, the boost for cms system is calculated for p+p @ 3.5 GeV and is
hardcoded in the code. You can calculate it for your beam energy and update the
code or execute the app with option `-b boost_value`.
