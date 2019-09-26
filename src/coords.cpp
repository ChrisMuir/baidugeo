#include <Rcpp.h>
#include "baidugeo.h"
using namespace Rcpp;


// Get coords data from the json of a single API request.
void from_json_coords(std::string& json) {
  if(json != "") {
    global_vars::doc.Parse(json.c_str());
  } else {
    global_vars::doc.Parse("{\"empty\":\"json\"}");
  }
  
  if(global_vars::doc.HasParseError()) {
    //printf("json parse error: %d at %zu\n", static_cast<int>(doc.GetParseError()), doc.GetErrorOffset());
    Rcerr << "parse error for json string: "<< json << std::endl;
    stop("json parse error");
  }
  
  // status
  if(global_vars::doc.HasMember("status")) {
    global_vars::status[0] = global_vars::doc["status"].GetDouble();
  } else {
    global_vars::status[0] = NA_REAL;
  }
  
  // longitude
  if(global_vars::doc["result"]["location"].HasMember("lng")) {
    global_vars::lng[0] = global_vars::doc["result"]["location"]["lng"].GetDouble();
  } else {
    global_vars::lng[0] = NA_REAL;
  }
  
  // latitude
  if(global_vars::doc["result"]["location"].HasMember("lat")) {
    global_vars::lat[0] = global_vars::doc["result"]["location"]["lat"].GetDouble();
  } else {
    global_vars::lat[0] = NA_REAL;
  }
  
  // precise
  if(global_vars::doc["result"].HasMember("precise")) {
    coord_vars::precise[0] = global_vars::doc["result"]["precise"].GetInt();
  } else {
    coord_vars::precise[0] = NA_INTEGER;
  }
  
  // confidence
  if(global_vars::doc["result"].HasMember("confidence")) {
    coord_vars::confidence[0] = global_vars::doc["result"]["confidence"].GetInt();
  } else {
    coord_vars::confidence[0] = NA_INTEGER;
  }
  
  // comprehension
  if(global_vars::doc["result"].HasMember("comprehension")) {
    coord_vars::comprehension[0] = global_vars::doc["result"]["comprehension"].GetDouble();
  } else {
    coord_vars::comprehension[0] = NA_REAL;
  }
  
  // level
  if(global_vars::doc["result"].HasMember("level")) {
    coord_vars::level = global_vars::doc["result"]["level"].GetString();
  } else {
    coord_vars::level = NA_STRING;
  }
}


// [[Rcpp::export]]
List from_json_coords_vector(CharacterVector location, 
                             std::vector<std::string> json_vect) {
  int json_len = json_vect.size();
  NumericVector status(json_len);
  NumericVector lng(json_len);
  NumericVector lat(json_len);
  IntegerVector precise(json_len);
  IntegerVector confidence(json_len);
  NumericVector comprehension(json_len);
  CharacterVector level(json_len);
  
  for(int i = 0; i < json_len; ++i) {
    // Parse json, assign values from the parsed json.
    from_json_coords(json_vect[i]);
    status[i] = global_vars::status[0];
    lng[i] = global_vars::lng[0];
    lat[i] = global_vars::lat[0];
    precise[i] = coord_vars::precise[0];
    confidence[i] = coord_vars::confidence[0];
    comprehension[i] = coord_vars::comprehension[0];
    level[i] = coord_vars::level;
  }
  
  // Create List output that has the necessary attributes to make it a
  // data.frame object.
  List out = List::create(
    Named("location") = location,
    Named("lon") = lng,
    Named("lat") = lat,
    Named("status") = status,
    Named("precise") = precise,
    Named("confidence") = confidence,
    Named("comprehension") = comprehension,
    Named("level") = level
  );
  
  out.attr("class") = "data.frame";
  out.attr("row.names") = seq(1, json_len);
  
  return out;
}


// [[Rcpp::export]]
List get_coords_pkg_data(Environment& coord_hash_map,
                         CharacterVector& keys) {
  int cache_len = Rf_length(coord_hash_map);
  CharacterVector location(cache_len);
  NumericVector status(cache_len);
  NumericVector lng(cache_len);
  NumericVector lat(cache_len);
  IntegerVector precise(cache_len);
  IntegerVector confidence(cache_len);
  NumericVector comprehension(cache_len);
  CharacterVector level(cache_len);
  
  CharacterVector curr_res;
  String curr_key;
  std::string curr_json;
  
  for(int i = 0; i < cache_len; ++i) {
    
    curr_key = keys[i];
    curr_res = coord_hash_map[curr_key];
    
    //location
    location[i] = curr_res[0];
    
    // Parse json, assign values from the parsed json.
    curr_json = as<std::string>(curr_res[1]);
    from_json_coords(curr_json);
    status[i] = global_vars::status[0];
    lng[i] = global_vars::lng[0];
    lat[i] = global_vars::lat[0];
    precise[i] = coord_vars::precise[0];
    confidence[i] = coord_vars::confidence[0];
    comprehension[i] = coord_vars::comprehension[0];
    level[i] = coord_vars::level;
  }
  
  // Create List output that has the necessary attributes to make it a
  // data.frame object.
  List out = List::create(
    Named("location") = location,
    Named("lon") = lng,
    Named("lat") = lat,
    Named("status") = status,
    Named("precise") = precise,
    Named("confidence") = confidence,
    Named("comprehension") = comprehension,
    Named("level") = level
  );
  
  out.attr("class") = "data.frame";
  if(cache_len > 0) {
    out.attr("row.names") = seq(1, cache_len);
  } else {
    out.attr("row.names") = 0;
  }
  
  return out;
}
