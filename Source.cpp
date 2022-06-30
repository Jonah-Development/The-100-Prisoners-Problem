/*
    The 100 Prisoners Riddle

    The 100 prisoners problem is a mathematical problem in probability theory and 
    combinatorics. In this problem, 100 numbered prisoners must find their own numbers 
    in one of 100 drawers in order to survive. The rules state that each prisoner may 
    open only 50 drawers and cannot communicate with other prisoners. 
    (source: https://en.wikipedia.org/wiki/100_prisoners_problem)

    I implemented 2 methods. One searches the boxes randomly, the other uses the method 
    explained in Veritasium's video: https://www.youtube.com/watch?v=iSNsgj1OCLA

    * Use [NUM_ITERATIONS] to control the number of simulated tries by the prisoners.
    * Use [NUM_PRISONERS] to change the number of prisoners. I use a unsigned 16-bit 
      integer, so the maximum number of prisoners would be UINT16_MAX.
    * Use [USE_MULTI_THREADING] to enable or disable multi threading.
*/

#include <iostream>
#include <algorithm>
#include <stdint.h>
#include <random>
#include <thread>
#include <vector>
#include <cstring>
#include <chrono>

constexpr auto USE_MULTI_THREADING = true;
constexpr auto NUM_ITERATIONS = 1'000'000;
constexpr uint16_t NUM_PRISONERS = 100;

constexpr auto NUM_BOXES = NUM_PRISONERS;
constexpr auto MAX_BOXES_TO_OPEN = NUM_PRISONERS / 2;


void RandomiseBoxes(std::vector<uint16_t>& boxes, const uint64_t seed);
void RunRandom(uint16_t* boxes, uint32_t& succeed, const uint64_t seed);
void RunMethod(uint16_t* boxes, uint32_t& succeed);
void Run(size_t startVal, size_t endVal, uint64_t* won);
template<typename T> int CountBuffer(T* buffer, T val, const size_t size);


int main()
{
    std::cout << "The 100 Prisoners Riddle\n"
        "\n"
        "The 100 prisoners problem is a mathematical problem in probability theory and\n"
        "combinatorics. In this problem, 100 numbered prisoners must find their own numbers\n"
        "in one of 100 drawers in order to survive. The rules state that each prisoner may\n"
        "open only 50 drawers and cannot communicate with other prisoners.\n"
        "(source: https://en.wikipedia.org/wiki/100_prisoners_problem)\n"
        "\n"
        "I implemented 2 methods. One searches the boxes randomly, the other uses the method\n"
        "explained in Veritasium's video: https://www.youtube.com/watch?v=iSNsgj1OCLA\n" << std::endl;

    // all but 2 threads, at least 1
    const auto processor_count = USE_MULTI_THREADING ? size_t(std::max<int64_t>(std::thread::hardware_concurrency() - 2, 2)) : 1;
    std::cout << "Initializing " << processor_count << " threads." << std::endl;
    std::thread* threads = new std::thread[processor_count];

    uint64_t* won = new uint64_t[(processor_count + 1) * 2];
    memset(won, 0, (processor_count + 1) * 2 * sizeof(uint64_t));

    size_t stepSize = NUM_ITERATIONS / processor_count;

    std::cout << std::endl << "Please wait . . .";

    auto init_time = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < processor_count; i++)
        threads[i] = std::thread(Run, stepSize * i, stepSize * (i + 1), &won[i * 2]);

    // Run for remainder
    Run(stepSize * processor_count, NUM_ITERATIONS, &won[processor_count]);
    
    for (size_t i = 0; i < processor_count; i++)
        threads[i].join();

    // add up all wins
    for (size_t i = 2; i < (processor_count + 1) * 2; )
    {
        won[0] += won[i++];
        won[1] += won[i++];
    }

    auto time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - init_time);

    std::cout << "\rIterations:    " << NUM_ITERATIONS << std::endl;
    std::cout << "Prisoners:     " << NUM_PRISONERS << std::endl;
    std::cout << "Boxes to open: " << MAX_BOXES_TO_OPEN << std::endl;
    std::cout << "Prisoners won randomly:    " << won[0] << " ->\t\t" << float(won[0]) / float(NUM_ITERATIONS) * 100 << '%' << std::endl;
    std::cout << "Prisoners won with method: " << won[1] << " ->\t" << float(won[1]) / float(NUM_ITERATIONS) * 100 << '%' << std::endl;
    std::cout << "Time:          " << time_elapsed.count() << "ms";
    std::cout << std::endl;

    
    delete[] threads;
    delete[] won;

    return 0;
}

void Run(size_t startVal, size_t endVal, uint64_t* won)
{
    std::vector<uint16_t> boxes(NUM_BOXES);
    for (uint16_t i = 0; i < NUM_BOXES; i++)
        boxes[i] = i;

    for (size_t it = startVal; it < endVal; it++)
    {
        RandomiseBoxes(boxes, it);

        uint32_t
            succeeded_rand = 0,
            succeeded_method = 0;

        RunRandom(boxes.data(), succeeded_rand, it);
        RunMethod(boxes.data(), succeeded_method);

        if (succeeded_rand == NUM_PRISONERS) won[0]++;
        if (succeeded_method == NUM_PRISONERS) won[1]++;
    }
}

void RandomiseBoxes(std::vector<uint16_t>& boxes, const uint64_t seed)
{
    std::mt19937_64 rng{ seed };
    std::shuffle(boxes.begin(), boxes.end(), rng);
}

void RunRandom(uint16_t* boxes, uint32_t& succeed, const uint64_t seed)
{
    std::mt19937_64 rng{ seed };

    // initialize vector
    std::vector<uint16_t> search_order(NUM_BOXES);
    for (uint16_t i = 0; i < NUM_BOXES; i++)
        search_order[i] = i;

    // run for all prisoners
    for (size_t prisoner = 0; prisoner < NUM_PRISONERS; prisoner++)
    {
        // create random search order vector
        std::shuffle(search_order.begin(), search_order.end(), rng);

        size_t i = 0;
        for (; i < MAX_BOXES_TO_OPEN; i++)
            if (boxes[search_order[i]] == prisoner)
            {
                succeed++;
                i = MAX_BOXES_TO_OPEN + 1;
                break;
            }

        // if just one prisoner didn't make it, we can return
        if (i != MAX_BOXES_TO_OPEN + 1) return;
    }
}

void RunMethod(uint16_t* boxes, uint32_t& succeed)
{
    // run for all prisoners
    for (size_t prisoner = 0; prisoner < NUM_PRISONERS; prisoner++)
    {
        size_t pos = prisoner;
        size_t i = 0;

        for (; i < MAX_BOXES_TO_OPEN; i++)
        {
            if (boxes[pos] == prisoner)
            {
                succeed++;
                i = MAX_BOXES_TO_OPEN + 1;
                break;
            }

            pos = boxes[pos];
        }

        // if just one prisoner doesn't make it, we don't need to continue
        if (i != MAX_BOXES_TO_OPEN + 1) return;
    }
}

template<typename T>
int CountBuffer(T* buffer, T val, const size_t size)
{
    int result = 0;
    for (size_t i = 0; i < size; i++)
        if (buffer[i] == val)
            result++;

    return result;
}