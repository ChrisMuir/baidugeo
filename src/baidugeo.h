#include <Rcpp.h>

// [[Rcpp::depends(rapidjsonr)]]
#include "rapidjson/document.h"

using namespace Rcpp;


#ifndef _ANAGRAMS_H
#define _ANAGRAMS_H

// Variables used by both coords and address functions.
namespace global_vars {
  extern rapidjson::Document doc;
  extern NumericVector status;
  extern NumericVector lng;
  extern NumericVector lat;
}

// Variables used by the coords functions.
namespace coord_vars {
  extern IntegerVector precise;
  extern IntegerVector confidence;
  extern NumericVector comprehension;
  extern String level;
}

// Variables used by the address functions.
namespace addr_vars {
  extern double input_lng;
  extern double input_lat;
  extern String formatted_address;
  extern String business;
  extern String country;
  extern NumericVector country_code;
  extern String country_code_iso;
  extern String country_code_iso2;
  extern String province;
  extern String city;
  extern NumericVector city_level;
  extern String district;
  extern String town;
  extern std::string temp_str;
  extern IntegerVector ad_code;
  extern String street;
  extern String street_number;
  extern String direction;
  extern IntegerVector distance;
  extern String sematic_desc;
  extern NumericVector city_code;
}


bool is_json_parsable(const char * json);
std::string get_message_value(const char * json);
void get_coords_from_uri(std::string& uri);


#endif /* _ANAGRAMS_H */
