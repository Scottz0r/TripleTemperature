#include <boost/test/unit_test.hpp>

// File being tested:
#include "temperature_engine.h"

using namespace scottz0r::temperature;

BOOST_AUTO_TEST_SUITE(temperature_engine)

BOOST_AUTO_TEST_CASE(it_should_vote_agreement)
{
    TemperatureReading temp0{true, 2400};
    TemperatureReading temp1{true, 2425};
    TemperatureReading temp2{true, 2420};

    TemperatureVoteEngine engine(50);
    TemperatureVoteResult result;

    engine.vote_temperature(temp0, temp1, temp2, result);

    BOOST_CHECK(result.status == TemperatureVoteStatus::OK);
    BOOST_TEST(result.is_temp0_agree);
    BOOST_TEST(result.is_temp1_agree);
    BOOST_TEST(result.is_temp2_agree);
    BOOST_TEST(result.temp0 == 2400);
    BOOST_TEST(result.temp1 == 2425);
    BOOST_TEST(result.temp2 == 2420);
    BOOST_TEST(result.average == 2415);
}

BOOST_AUTO_TEST_CASE(it_should_vote_agreement_round_truncate)
{
    TemperatureReading temp0{true, 2400};
    TemperatureReading temp1{true, 2425};
    TemperatureReading temp2{true, 2422};

    // Sum = 7247, which is not divisible by 3.
    TemperatureVoteEngine engine(50);
    TemperatureVoteResult result;

    engine.vote_temperature(temp0, temp1, temp2, result);

    BOOST_CHECK(result.status == TemperatureVoteStatus::OK);
    BOOST_TEST(result.average == 2415);
}

BOOST_AUTO_TEST_CASE(it_should_vote_one_disagree)
{
    TemperatureReading temp0{true, 2400};
    TemperatureReading temp1{true, 5000};
    TemperatureReading temp2{true, 2420};

    TemperatureVoteEngine engine(100);
    TemperatureVoteResult result;

    engine.vote_temperature(temp0, temp1, temp2, result);

    BOOST_CHECK(result.status == TemperatureVoteStatus::OK);
    BOOST_TEST(result.is_temp0_agree);
    BOOST_TEST(!result.is_temp1_agree);
    BOOST_TEST(result.is_temp2_agree);
    BOOST_TEST(result.temp0 == 2400);
    BOOST_TEST(result.temp1 == 5000);
    BOOST_TEST(result.temp2 == 2420);
    BOOST_TEST(result.average == 2410);
}

BOOST_AUTO_TEST_CASE(it_should_vote_all_disagree)
{
    TemperatureReading temp0{true, 2400};
    TemperatureReading temp1{true, 2425};
    TemperatureReading temp2{true, 2375};

    TemperatureVoteEngine engine(20);
    TemperatureVoteResult result;

    engine.vote_temperature(temp0, temp1, temp2, result);

    BOOST_CHECK(result.status == TemperatureVoteStatus::Disagree);
    BOOST_TEST(!result.is_temp0_agree);
    BOOST_TEST(!result.is_temp1_agree);
    BOOST_TEST(!result.is_temp2_agree);
    BOOST_TEST(result.temp0 == 2400);
    BOOST_TEST(result.temp1 == 2425);
    BOOST_TEST(result.temp2 == 2375);
    BOOST_TEST(result.average == 0);
}

BOOST_AUTO_TEST_CASE(it_should_vote_one_invalid)
{
    TemperatureReading temp0{true, 2400};
    TemperatureReading temp1{true, 2424};
    TemperatureReading temp2{false, 9999};

    TemperatureVoteEngine engine(100);
    TemperatureVoteResult result;

    engine.vote_temperature(temp0, temp1, temp2, result);

    BOOST_CHECK(result.status == TemperatureVoteStatus::OK);
    BOOST_TEST(result.is_temp0_agree);
    BOOST_TEST(result.is_temp1_agree);
    BOOST_TEST(!result.is_temp2_agree);
    BOOST_TEST(result.temp0 == 2400);
    BOOST_TEST(result.temp1 == 2424);
    BOOST_TEST(result.temp2 == 0);
    BOOST_TEST(result.average == 2412);
}

BOOST_AUTO_TEST_CASE(it_should_vote_two_invalid)
{
    TemperatureReading temp0{true, 2400};
    TemperatureReading temp1{false, 2400};
    TemperatureReading temp2{false, 9999};

    TemperatureVoteEngine engine(100);
    TemperatureVoteResult result;

    engine.vote_temperature(temp0, temp1, temp2, result);

    BOOST_CHECK(result.status == TemperatureVoteStatus::SensorError);
    BOOST_TEST(!result.is_temp0_agree);
    BOOST_TEST(!result.is_temp1_agree);
    BOOST_TEST(!result.is_temp2_agree);
    BOOST_TEST(result.temp0 == 2400);
    BOOST_TEST(result.temp1 == 0);
    BOOST_TEST(result.temp2 == 0);
    BOOST_TEST(result.average == 0);
}

BOOST_AUTO_TEST_CASE(it_should_vote_three_invalid)
{
    TemperatureReading temp0{false, 2425};
    TemperatureReading temp1{false, 2400};
    TemperatureReading temp2{false, 9999};

    TemperatureVoteEngine engine(100);
    TemperatureVoteResult result;

    engine.vote_temperature(temp0, temp1, temp2, result);

    BOOST_CHECK(result.status == TemperatureVoteStatus::SensorError);
    BOOST_TEST(!result.is_temp0_agree);
    BOOST_TEST(!result.is_temp1_agree);
    BOOST_TEST(!result.is_temp2_agree);
    BOOST_TEST(result.temp0 == 0);
    BOOST_TEST(result.temp1 == 0);
    BOOST_TEST(result.temp2 == 0);
    BOOST_TEST(result.average == 0);
}

BOOST_AUTO_TEST_CASE(it_should_vote_one_invalid_disagree)
{
    TemperatureReading temp0{true, -250};
    TemperatureReading temp1{true, 1000};
    TemperatureReading temp2{false, -16021};

    TemperatureVoteEngine engine(50);
    TemperatureVoteResult result;

    engine.vote_temperature(temp0, temp1, temp2, result);

    BOOST_CHECK(result.status == TemperatureVoteStatus::Disagree);
    BOOST_TEST(!result.is_temp0_agree);
    BOOST_TEST(!result.is_temp1_agree);
    BOOST_TEST(!result.is_temp2_agree);
    BOOST_TEST(result.temp0 == -250);
    BOOST_TEST(result.temp1 == 1000);
    BOOST_TEST(result.temp2 == 0);
    BOOST_TEST(result.average == 0);
}

BOOST_AUTO_TEST_CASE(it_should_set_output_default_values)
{
    TemperatureReading temp0{false, 999};
    TemperatureReading temp1{false, 888};
    TemperatureReading temp2{false, -777};

    TemperatureVoteEngine engine(100);
    TemperatureVoteResult result;

    engine.vote_temperature(temp0, temp1, temp2, result);

    BOOST_CHECK(result.status == TemperatureVoteStatus::SensorError);
    BOOST_TEST(!result.is_temp0_agree);
    BOOST_TEST(!result.is_temp1_agree);
    BOOST_TEST(!result.is_temp2_agree);
    BOOST_TEST(result.temp0 == 0);
    BOOST_TEST(result.temp1 == 0);
    BOOST_TEST(result.temp2 == 0);
    BOOST_TEST(result.average == 0);
}

BOOST_AUTO_TEST_SUITE_END()
