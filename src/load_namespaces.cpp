#include <Rcpp.h>
#include "baidugeo.h"
using namespace Rcpp;


// Variables used by both coords and address functions.
namespace global_vars {
  rapidjson::Document doc;
  NumericVector status;
  NumericVector lng;
  NumericVector lat;
}

// Variables used by the coords functions.
namespace coord_vars {
  IntegerVector precise;
  IntegerVector confidence;
  NumericVector comprehension;
  String level;
}

// Variables used by the address functions.
namespace addr_vars {
  double input_lng;
  double input_lat;
  String formatted_address;
  String business;
  String country;
  NumericVector country_code;
  String country_code_iso;
  String country_code_iso2;
  String province;
  String city;
  NumericVector city_level;
  String district;
  String town;
  std::string temp_str;
  IntegerVector ad_code;
  String street;
  String street_number;
  String direction;
  IntegerVector distance;
  String sematic_desc;
  NumericVector city_code;
}
