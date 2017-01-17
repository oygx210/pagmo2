#define BOOST_TEST_MODULE ackley_test
#include <boost/test/included/unit_test.hpp>

#include <boost/lexical_cast.hpp>
#include <exception>
#include <iostream>
#include <string>

#include <pagmo/problem.hpp>
#include <pagmo/problems/ackley.hpp>
#include <pagmo/types.hpp>

using namespace pagmo;

BOOST_AUTO_TEST_CASE(ackley_test)
{
    // Problem construction
    ackley ack1{1u};
    ackley ack3{3u};
    BOOST_CHECK_THROW(ackley{0u}, std::invalid_argument);
    BOOST_CHECK_NO_THROW(problem{ack3});
    // Pick a few reference points
    vector_double x1 = {1.12};
    vector_double x3 = {-23.45, 12.34, 111.12};
    // Fitness test
    BOOST_CHECK_CLOSE(ack1.fitness(x1)[0], 4.659037611351948, 1e-13);
    BOOST_CHECK_CLOSE(ack1.fitness(x3)[0], 21.941495638130885, 1e-13);
    // Bounds Test
    BOOST_CHECK((ack3.get_bounds() == std::pair<vector_double, vector_double>{{-15, -15, -15}, {30, 30, 30}}));
    // Name and extra info tests
    BOOST_CHECK(ack3.get_name().find("Ackley") != std::string::npos);
    // Best known test
    auto x_best = ack3.best_known();
    BOOST_CHECK((x_best == vector_double{0., 0., 0.}));
}

BOOST_AUTO_TEST_CASE(ackley_serialization_test)
{
    problem p{ackley{4u}};
    // Call objfun to increase the internal counters.
    p.fitness({1., 1., 1., 1.});
    // Store the string representation of p.
    std::stringstream ss;
    auto before = boost::lexical_cast<std::string>(p);
    // Now serialize, deserialize and compare the result.
    {
        cereal::JSONOutputArchive oarchive(ss);
        oarchive(p);
    }
    // Change the content of p before deserializing.
    p = problem{null_problem{}};
    {
        cereal::JSONInputArchive iarchive(ss);
        iarchive(p);
    }
    auto after = boost::lexical_cast<std::string>(p);
    BOOST_CHECK_EQUAL(before, after);
}
