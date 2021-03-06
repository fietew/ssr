#include "apf/stringtools.h"

#include "catch/catch.hpp"

using namespace apf::str;

TEST_CASE("stringtools", "Test all functions of apf::str namespace")
{

SECTION("A2S", "Anything to String")
{
  CHECK(A2S(42) == "42");
  CHECK(A2S(123.4) == "123.4");
  CHECK(A2S(-123.4) == "-123.4");
  CHECK(A2S("char array") == "char array");
  CHECK(A2S(std::string("string")) == "string");
  CHECK(A2S(true) == "true");
  CHECK(A2S(false) == "false");
}

SECTION("S2A", "String to Anything")
{
  int res_int;
  double res_dbl;
  std::string res_str;
  bool res_bool = false;

  CHECK(S2A("42", res_int));
  CHECK(res_int == 42);
  CHECK(S2A(" 42    ", res_int));
  CHECK(res_int == 42);
  CHECK(S2A(" 42    ", res_dbl));
  CHECK(res_dbl == 42.0);
  CHECK(S2A(" 42    ", res_str));
  CHECK(res_str == " 42    ");

  CHECK_FALSE(S2A(" - 42    ", res_int));
  CHECK(S2A(" -42    ", res_int));
  CHECK(res_int == -42);

  CHECK_FALSE(S2A("  true ", res_int));
  CHECK_FALSE(S2A("  true ", res_dbl));
  CHECK(S2A("  true ", res_str));
  CHECK(res_str == "  true ");
  CHECK(S2A("true", res_bool));
  CHECK(res_bool == true);
  CHECK(S2A("  true ", res_bool));
  CHECK(res_bool == true);
  CHECK(S2A(" false ", res_bool));
  CHECK(res_bool == false);
  CHECK(S2A("false", res_bool));
  CHECK(res_bool == false);
  CHECK(S2A(" 1 ", res_bool));
  CHECK(res_bool == true);
  CHECK(S2A("1", res_bool));
  CHECK(res_bool == true);
  CHECK(S2A(" 0 ", res_bool));
  CHECK(res_bool == false);
  CHECK(S2A("0", res_bool));
  CHECK(res_bool == false);
  CHECK_FALSE(S2A("True", res_bool));
  CHECK_FALSE(S2A("False", res_bool));
  CHECK_FALSE(S2A("42", res_bool));

  CHECK_FALSE(S2A(" 42 3 ", res_int));
  CHECK_FALSE(S2A("42!", res_int));
  CHECK_FALSE(S2A("42 .", res_dbl));

  // too lazy to repeat all tests for std::string ...
  CHECK(S2A(std::string(" 42    "), res_int));
  CHECK(res_int == 42);
  CHECK(S2A(std::string(" 42    "), res_str));
  CHECK(res_str == " 42    ");
}

SECTION("S2RV", "String to Return Value")
{
  // first the version with default values

  CHECK(S2RV("  42  ", 0) == 42);
  CHECK(S2RV("  42.42 ", 0) == 0);
  CHECK(S2RV("  42.42 ", 0.0) == 42.42);
  CHECK(S2RV("  42  ", "") == "  42  ");
  CHECK(S2RV("  42  ", std::string("")) == "  42  ");
  CHECK(S2RV("  0 ", true) == false);
  CHECK(S2RV("  42 ", true) == true);
  CHECK(S2RV(" false ", true) == false);
  CHECK(S2RV(" false . ", true) == true);

  // now the throwing version

  CHECK(S2RV<int>(" 42 ") == 42);
  CHECK(S2RV<float>(" 42 ") == 42.0);
  CHECK(S2RV<std::string>(" 42 ") == " 42 ");
  CHECK_THROWS_AS(S2RV<int>(" 42.0 "), std::invalid_argument);
}

SECTION("convert_chars()", "")
{
  std::istringstream iss;
  int i;

  iss.str("2");
  CHECK_FALSE(convert_chars<1>(iss, i).fail());
  CHECK(i == 2);

  iss.str(" 2");
  iss >> std::noskipws;
  CHECK(convert_chars<1>(iss, i).fail());

  iss.clear();
  iss.str(" 7");
  iss >> std::skipws;
  CHECK_FALSE(convert_chars<1>(iss, i).fail());
  CHECK(i == 7);

  iss.str("a");
  CHECK(convert_chars<1>(iss, i).fail());

  iss.clear();
  iss.str("123");
  CHECK_FALSE(convert_chars<3>(iss, i).fail());
  CHECK(i == 123);

  // empty stream:
  CHECK(convert_chars<1>(iss, i).fail());
}

SECTION("remove_char()", "")
{
  std::istringstream iss;

  iss.str("a");
  CHECK_FALSE(remove_char(iss, 'a').fail());
  CHECK(remove_char(iss, 'a').fail());

  iss.clear();
  iss.str(" a");
  CHECK_FALSE(remove_char(iss, 'a').fail());

  iss.str(" a");
  iss >> std::noskipws;
  CHECK(remove_char(iss, 'a').fail());
  iss.clear();
  CHECK_FALSE(remove_char(iss, 'a').fail());

  iss.str("a");
  CHECK(remove_char(iss, 'b').fail());

  // TODO: check if iss is empty now
}

SECTION("remove_colon()", "")
{
  std::istringstream iss;

  iss.str("::");
  iss >> remove_colon;
  CHECK_FALSE(iss.fail());
  iss >> remove_colon;
  CHECK_FALSE(iss.fail());
  iss >> remove_colon;
  CHECK(iss.fail());

  iss.clear();
  iss.str("a");
  iss >> remove_colon;
  CHECK(iss.fail());

  iss.clear();
  iss.str(" :");
  iss >> remove_colon;
  CHECK_FALSE(iss.fail());

  iss.clear();
  iss.str(" :");
  iss >> std::noskipws;
  iss >> remove_colon;
  CHECK(iss.fail());
}

SECTION("string2time", "String to Time in Seconds")
{
  double res;
  int res2;
  CHECK_FALSE(string2time(" 4 : 33 ", res));
  CHECK_FALSE(string2time(" 4:33.2 ", res2));
  CHECK(string2time(" 4:33 ", res));
  CHECK(res == 273);
  CHECK(string2time(std::string(" 01:33.3 "), res));
  CHECK(res == 93.3);
  CHECK(string2time("-2:11:33", res));
  CHECK(res == -7893);
  CHECK(string2time("33", res2));
  CHECK(res2 == 33);
  CHECK(string2time("33h", res2));
  CHECK(res2 == 118800);
  CHECK(string2time(" 33h ", res2));
  CHECK(res2 == 118800);
  CHECK(string2time(" 33min ", res));
  CHECK(res == 1980);
  CHECK(string2time(" 33 min ", res));
  CHECK(res == 1980);
  CHECK(string2time(" 33s ", res));
  CHECK(res == 33);
  CHECK(string2time(" 33 ms ", res));
  CHECK(res == 0.033);
  CHECK(string2time("-  1:59.9", res));
  CHECK(res == -119.9);

  CHECK_FALSE(string2time("1:60.0", res));
  CHECK_FALSE(string2time("0:00:0", res));
  CHECK_FALSE(string2time("0:0:0", res));
  CHECK_FALSE(string2time("71:33", res));
  CHECK_FALSE(string2time("4:33 min", res));
  CHECK_FALSE(string2time("2: 11:33.3", res));
  CHECK_FALSE(string2time("2:11:33.", res));
  CHECK_FALSE(string2time("2:11 33", res));
  CHECK_FALSE(string2time("2 11:33", res));
  CHECK_FALSE(string2time("2:60:33", res));
  CHECK_FALSE(string2time("2:11:.33", res));
  CHECK_FALSE(string2time(" - - 2:33 ", res));
}

} // TEST_CASE

// Settings for Vim (http://www.vim.org/), please do not remove:
// vim:softtabstop=2:shiftwidth=2:expandtab:textwidth=80:cindent
