# The-100-Prisoners-Problem
The 100 prisoners problem is a mathematical problem in probability theory and combinatorics. In this problem, 100 numbered prisoners must find their own numbers in one of 100 drawers in order to survive. The rules state that each prisoner may open only 50 drawers and cannot communicate with other prisoners. (source: https://en.wikipedia.org/wiki/100_prisoners_problem)

I implemented 2 methods. One searches the boxes randomly, the other uses the method explained in Veritasium's video: https://www.youtube.com/watch?v=iSNsgj1OCLA

* Use [NUM_ITERATIONS] to control the number of simulated tries by the prisoners.
* Use [NUM_PRISONERS] to change the number of prisoners. I use a unsigned 16-bit integer, so the maximum number of prisoners would be UINT16_MAX.
* Use [USE_MULTI_THREADING] to enable or disable multi threading.
