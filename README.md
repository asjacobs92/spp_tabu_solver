# SPP Tabu Search Solver

The Set Partitioning Problem (SPP) is one of the fundamental problems in combinatorial optimisation, being applied in many areas, especially in schedulings problems. The best-known application for the SPP mathematical model, used by many airlines, is the flight crew scheduling problem. Full mathematical model for the SPP and application for the flight cre scheduling problem can be found [here] (http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.38.902&rep=rep1&type=pdf).

This repositosy hosts the code for a SPP solver using the [Tabu Search] (https://en.wikipedia.org/wiki/Tabu_search) meta-heuristic, using a local search to find the best possible neighborhood. This approach work fine for small instances of the problem (up to 10 thousand subsets), but it does not scale for larger instances (larger than 100 thousand subsets). In order to improve the feasibility of the resulting solution, I applied the Heuristic Feasibility Operator, proposed by P.C. Chu and J.E. Beasley [here] (http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.31.2800), to Tabu Search initial solution.

This solver was calibrated and tested using several different randomic seeds, Tabu list sizes, maximum number of iterations and execution time limit, over several different instances. All instances used to test this solution can be found [here] (http://people.brunel.ac.uk/~mastjjb/jeb/orlib/sppinfo.html). All my results and conclusions are presented on my report and presentation (in portuguese, sorry).

TODO: push code and folder descriptions.

<br/>
Arthur Jacobs<br/>
asjacobs@inf.ufrgs.br<br/>
Computer Science Undergraduate Student<br/>
Federeal University of Rio Grande do Sul<br/>
