#pragma once

#include <vector>
#include <iostream>
#include <limits>
#include <iomanip>
#include <cstddef>
#include <sys/time.h>
#include <stdint.h>
#include <string>



namespace Hayai
{
    /// Static helper class for outputting to a terminal/console.
    class Console
    {
    public:
        /// Console text colors.
        enum TextColor
        {
            /// Default console color.
            
            /// Used for resets.
            TextDefault = 0,
            
            
            /// Black.
            
            /// @warning Avoid using black unless it is absolutely necesssary.
            TextBlack = 30,
            
            
            /// Blue.
            TextBlue = 34,


            /// Green.
            TextGreen = 32,
            
            
            /// Cyan.
            TextCyan = 36,
            

            /// Red.
            TextRed = 31,
            

            /// Purple.
            TextPurple = 35,
            

            /// Yellow.
            TextYellow = 33,
            

            /// White.
            
            /// @warning Avoid using white unless it is absolutely necessary.
            TextWhite = 37
        };
    };

    inline std::ostream& operator <<(std::ostream& stream,
                                     const Console::TextColor& color)
    {
        if (color == Console::TextDefault)
            return stream << "\033[m";
        else
            return stream << "\033[0;" << int(color) << "m";
    }



    /// Base test class.
    
    /// @ref SetUp is invoked before each run, and @ref TearDown is invoked 
    /// once the run is finished. Iterations rely on the same fixture 
    /// for every run.
    /// 
    /// The default test class does not contain any actual code in the 
    /// SetUp and TearDown methods, which means that tests can inherit 
    /// this class directly for non-fixture based benchmarking tests.
    class Test
    {
    public:
        /// Set up the testing fixture for execution of a run.
        virtual void SetUp()
        {
            
        }
        
        
        /// Tear down the previously set up testing fixture after the 
        /// execution run.
        virtual void TearDown()
        {
            
        }
        
        
        /// Run the test.
        
        /// @param iterations Number of iterations to gather data for.
        /// @returns the number of microseconds the run took.
        int64_t Run(std::size_t iterations)
        {
            // Set up the testing fixture.
            this->SetUp();
            
            // Get the starting time.
            struct timeval startTime,
                           endTime;
            
            gettimeofday(&startTime,
                         NULL);
            
            // Run the test body for each iteration.
            std::size_t iteration = iterations;
            while (iteration--)
                this->TestBody();
            
            // Get the ending time.
            gettimeofday(&endTime,
                         NULL);
            
            // Tear down the testing fixture.
            this->TearDown();
            
            // Return the duration in microseconds.
            return (endTime.tv_sec - startTime.tv_sec) * 1000000 + 
                   (endTime.tv_usec - startTime.tv_usec);
        }
    protected:
        /// Test body.
        
        /// Executed for each iteration the benchmarking test is run.
        virtual void TestBody()
        {
            
        }
    };


    /// Base class for test factory implementations.
    class TestFactory
    {
    public:
        /// Virtual destructor
        
        /// Has no function in the base class.
        virtual ~TestFactory()
        {
            
        }
        
        
        /// Creates a test instance to run.
        
        /// @returns a pointer to an initialized test.
        virtual Test* CreateTest() = 0;
    };



    /// Test descriptor.
    
    /// Describes the test
    class TestDescriptor
    {
    public:
        /// Initialize a new test descriptor.
        
        /// @param fixtureName Name of the fixture.
        /// @param testName Name of the test.
        /// @param runs Number of runs for the test.
        /// @param iterations Number of iterations per run.
        /// @param testFactory Test factory implementation for the test.
        TestDescriptor(const char* fixtureName,
                       const char* testName,
                       std::size_t runs,
                       std::size_t iterations,
                       TestFactory* testFactory)
            :   FixtureName(fixtureName),
                TestName(testName),
                Runs(runs),
                Iterations(iterations),
                Factory(testFactory)
        {
            
        }
        
        
        /// Dispose of a test descriptor.
        ~TestDescriptor()
        {
            delete this->Factory;
        }
        
        
        /// Fixture name.
        std::string FixtureName;
        
        
        /// Test name.
        std::string TestName;
        
        
        /// Test runs.
        std::size_t Runs;
        
        
        /// Iterations per test run.
        std::size_t Iterations;
        
        
        /// Test factory.
        TestFactory* Factory;
    };



    /// Benchmarking execution controller singleton.
    class Benchmarker
    {
    public:
        /// Get the singleton instance of @ref Benchmarker.
            
        /// @returns a reference to the singleton instance of the 
        /// benchmarker execution controller.
        static Benchmarker& Instance()
        {
            static Benchmarker singleton;
            return singleton;
        }
            
            
        /// Register a test with the benchmarker instance.
            
        /// @param fixtureName Name of the fixture.
        /// @param testName Name of the test.
        /// @param runs Number of runs for the test.
        /// @param iterations Number of iterations per run.
        /// @param testFactory Test factory implementation for the test.
        /// @returns a pointer to a @ref TestDescriptor instance
        /// representing the given test.
        static TestDescriptor* RegisterTest(const char* fixtureName,
                                            const char* testName,
                                            std::size_t runs,
                                            std::size_t iterations,
                                            TestFactory* testFactory)
        {
            TestDescriptor* descriptor = new TestDescriptor(fixtureName,
                                                            testName,
                                                            runs,
                                                            iterations,
                                                            testFactory);
                
            Instance()._tests.push_back(descriptor);

            return descriptor;
        }
            
            
        /// Run all benchmarking tests.
        static void RunAllTests()
        {
            Benchmarker& instance = Instance();
            
            // Initial output
            std::cout << std::fixed;
            std::cout << Console::TextGreen << "[==========]"
                      << Console::TextDefault << " Running "
                      << instance._tests.size()
                      << (instance._tests.size() == 1 ?
                          " benchmark." : 
                          " benchmarks.")
                      << std::endl;

            // Run through all the tests in ascending order.
#define PAD(x) std::cout << std::setw(34) << x << std::endl;
#define PAD_DEVIATION(description,                                      \
                      deviated,                                         \
                      average,                                          \
                      unit)                                             \
            {                                                           \
                double _d_ =                                            \
                    double(deviated) - double(average);                 \
                                                                        \
                PAD(description <<                                      \
                    deviated << " " << unit << " (" <<                  \
                    (deviated < average ?                               \
                     Console::TextRed :                                 \
                     Console::TextGreen) <<                             \
                    (deviated > average ? "+" : "") <<                  \
                    _d_ << " " << unit << " / " <<                      \
                    (deviated > average ? "+" : "") <<                  \
                    (_d_ * 100.0 / average) << " %" <<                  \
                    Console::TextDefault << ")");                       \
            }
            
#define PAD_DEVIATION_INVERSE(description,                              \
                              deviated,                                 \
                              average,                                  \
                              unit)                                     \
            {                                                           \
                double _d_ =                                            \
                    double(deviated) - double(average);                 \
                                                                        \
                PAD(description <<                                      \
                    deviated << " " << unit << " (" <<                  \
                    (deviated > average ?                               \
                     Console::TextRed :                                 \
                     Console::TextGreen) <<                             \
                    (deviated > average ? "+" : "") <<                  \
                    _d_ << " " << unit << " / " <<                      \
                    (deviated > average ? "+" : "") <<                  \
                    (_d_ * 100.0 / average) << " %" <<                  \
                    Console::TextDefault << ")");                       \
            }


            std::size_t index = 0;
            while (index < instance._tests.size())
            {
                // Get the test descriptor.
                TestDescriptor* descriptor = instance._tests[index++];
               
                // Describe the beginning of the run.
                std::cout << Console::TextGreen << "[ RUN      ]"
                          << Console::TextDefault << " "
                          << descriptor->FixtureName << "."
                          << descriptor->TestName
                          << " (" << descriptor->Runs
                          << (descriptor->Runs == 1 ? " run, " : " runs, ") 
                          << descriptor->Iterations
                          << (descriptor->Iterations == 1 ?
                              " iteration per run)" :
                              " iterations per run)")
                          << std::endl;
                
                // Execute each individual run.
                int64_t timeTotal = 0,
                        timeRunMin = std::numeric_limits<int64_t>::max(),
                        timeRunMax = std::numeric_limits<int64_t>::min();

                std::size_t run = descriptor->Runs;
                while (run--)
                {
                    // Construct a test instance.
                    Test* test = descriptor->Factory->CreateTest();
                        
                    // Run the test.
                    int64_t time = test->Run(descriptor->Iterations);
                    
                    // Store the test time.
                    timeTotal += time;
                    if (timeRunMin > time)
                        timeRunMin = time;
                    if (timeRunMax < time)
                        timeRunMax = time;
                    
                    // Dispose of the test instance.
                    delete test;
                }
                
                // Calculate different metrics.
                double timeRunAverage = double(timeTotal) / double(descriptor->Runs);

                double runsPerSecondAverage = 1000000.0 / timeRunAverage;
                double runsPerSecondMax = 1000000.0 / double(timeRunMin);
                double runsPerSecondMin = 1000000.0 / double(timeRunMax);
                
                double timeIterationAverage = timeRunAverage / double(descriptor->Iterations);
                double timeIterationMin = double(timeRunMin) / double(descriptor->Iterations);
                double timeIterationMax = double(timeRunMax) / double(descriptor->Iterations);

                double iterationsPerSecondAverage = 1000000.0 / timeIterationAverage;
                double iterationsPerSecondMax = 1000000.0 / timeIterationMin;
                double iterationsPerSecondMin = 1000000.0 / timeIterationMax;

                // Describe the end of the run.
                std::cout << Console::TextGreen << "[     DONE ]"
                          << Console::TextDefault << " "
                          << descriptor->FixtureName << "."
                          << descriptor->TestName << " ("
                          << (double(timeTotal) / 1000.0) << " ms)"
                          << std::endl;
                
                std::cout << Console::TextYellow << "[   RUNS   ] "
                          << Console::TextDefault
                          << "       Average time: " << timeRunAverage
                          << " us" << std::endl;
                
                PAD_DEVIATION_INVERSE("Fastest: ",
                              timeRunMin,
                              timeRunAverage,
                              "us");
                PAD_DEVIATION_INVERSE("Slowest: ",
                              timeRunMax,
                              timeRunAverage,
                              "us");
                PAD("");
                PAD("Average performance: " << runsPerSecondAverage << " runs/s");
                PAD_DEVIATION("Best performance: ",
                              runsPerSecondMax,
                              runsPerSecondAverage,
                              "runs/s");
                PAD_DEVIATION("Worst performance: ",
                              runsPerSecondMin,
                              runsPerSecondAverage,
                              "runs/s");

                std::cout << Console::TextYellow << "[ITERATIONS] "
                          << Console::TextDefault
                          << "       Average time: " << timeIterationAverage
                          << " us" << std::endl;
                
                PAD_DEVIATION_INVERSE("Fastest: ",
                              timeIterationMin,
                              timeIterationAverage,
                              "us");
                PAD_DEVIATION_INVERSE("Slowest: ",
                              timeIterationMax,
                              timeIterationAverage,
                              "us");
                PAD("");
                PAD("Average performance: " << iterationsPerSecondAverage << " iterations/s");
                PAD_DEVIATION("Best performance: ",
                              iterationsPerSecondMax,
                              iterationsPerSecondAverage,
                              "iterations/s");
                PAD_DEVIATION("Worst performance: ",
                              iterationsPerSecondMin,
                              iterationsPerSecondAverage,
                              "iterations/s");
            }

#undef PAD

            // Final output.
            std::cout << Console::TextGreen << "[==========]"
                      << Console::TextDefault << " Ran "
                      << instance._tests.size()
                      << (instance._tests.size() == 1 ?
                          " benchmark." : 
                          " benchmarks.")
                      << std::endl;
        }
    private:
        /// Private constructor.
        Benchmarker()
        {

        }
            

        /// Private destructor.
        ~Benchmarker()
        {
            // Release all test descriptors.
            std::size_t index = this->_tests.size();
            while (index--)
                delete this->_tests[index];
        }
            
            
        std::vector<TestDescriptor*> _tests; ///< Registered tests.
    };



    /// Default test factory implementation.
    
    /// Simply constructs an instance of a the test of class @ref T with no 
    /// constructor parameters.
    /// 
    /// @tparam T Test class.
    template<class T>
    class TestFactoryDefault
        :   public TestFactory
    {
    public:
        /// Create a test instance with no constructor parameters.
        
        /// @returns a pointer to an initialized test.
        virtual Test* CreateTest()
        {
            return new T();
        }
    };

    typedef Test Fixture;

}

#define BENCHMARK_CLASS_NAME_(fixture_name, benchmark_name) \
    fixture_name ## _ ## benchmark_name ## _Benchmark

#define BENCHMARK_(fixture_name,                                        \
                   benchmark_name,                                      \
                   fixture_class_name,                                  \
                   runs,                                                \
                   iterations)                                          \
    class BENCHMARK_CLASS_NAME_(fixture_name, benchmark_name)           \
        :   public fixture_class_name                                   \
    {                                                                   \
    public:                                                             \
        BENCHMARK_CLASS_NAME_(fixture_name, benchmark_name)()           \
        {                                                               \
                                                                        \
        }                                                               \
    protected:                                                          \
        virtual void TestBody();                                        \
    private:                                                            \
        static const ::Hayai::TestDescriptor* _descriptor;              \
    };                                                                  \
                                                                        \
    const ::Hayai::TestDescriptor* BENCHMARK_CLASS_NAME_(fixture_name, benchmark_name)::_descriptor = \
        ::Hayai::Benchmarker::Instance().RegisterTest(#fixture_name, \
                                                                     #benchmark_name, \
                                                                     runs, \
                                                                     iterations, \
                                                                     new ::Hayai::TestFactoryDefault<BENCHMARK_CLASS_NAME_(fixture_name, benchmark_name)>()); \
                                                                        \
    void BENCHMARK_CLASS_NAME_(fixture_name, benchmark_name)::TestBody()

#define BENCHMARK_F(fixture_name,                        \
                    benchmark_name,                      \
                    runs,                                \
                    iterations)                          \
    BENCHMARK_(fixture_name,                             \
               benchmark_name,                           \
               fixture_name,                             \
               runs,                                     \
               iterations)

#define BENCHMARK(fixture_name,                          \
                  benchmark_name,                        \
                  runs,                                  \
                  iterations)                            \
    BENCHMARK_(fixture_name,                             \
               benchmark_name,                           \
               ::Hayai::Test,                            \
               runs,                                     \
               iterations)

